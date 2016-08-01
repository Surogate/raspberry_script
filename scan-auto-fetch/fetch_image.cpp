#include "fetch_image.hpp"
#include <string>
#include "afilesystem.hpp"
#include <vector>
#include "astring_view.hpp"
#include <regex>
#include "uri.hpp"
#include "uri_builder.hpp"

void fetch_image::erase_directory(const astd::filesystem::path& path)
{
   if (astd::filesystem::exists(path))
   {
      astd::filesystem::error_code ec;
      auto it = astd::filesystem::directory_iterator(path);
      auto ite = astd::filesystem::directory_iterator();

      while (it != ite)
      {
         if (!astd::filesystem::remove(*it, ec))
         {
            std::cerr << "error removing " << it->path() << " : " << ec.message() << std::endl;
         }
         ++it;
      }
      if (!astd::filesystem::remove(path, ec))
         std::cerr << "error removing " << path << " : " << ec.message() << std::endl;
   }
}

bool fetch_image::is_404(const std::vector<char>& buffer)
{
   static std::vector<astd::string_view> file_not_found{ "404 Not Found", "Error 404 - Not found", "300 Multiple Choices", "301 Moved Permanently" };
   for (auto& error : file_not_found)
   {
      if (std::search(buffer.begin(), buffer.end(), error.begin(), error.end()) != buffer.end())
         return true;
   }
   return buffer.size() == 0;
}

astd::string_view find_image_name(const xts::uri& image_uri)
{
   astd::string_view result;
   for (auto& p : image_uri.paths())
   {
      result = p;
   }
   return result;
}

xts::uri find_image_url(const std::vector<char>& buffer, const std::string& pattern, const astd::string_view& chapter_num, const astd::string_view& image_num)
{
   typedef std::regex_iterator<std::vector<char>::const_iterator> vregex_iterator;

   xts::uri result;
   bool keep_searching = true;
   auto beg = buffer.begin();

   while (keep_searching)
   {
      auto search_result = std::search(beg, buffer.end(), pattern.begin(), pattern.end());

      if (search_result != buffer.end())
      {
         auto end_quote = std::find(search_result, buffer.end(), '"');
         auto url_image = xts::uri(std::string{ search_result, end_quote });
         
         bool chapte_num_found = false;
         bool image_num_validated = false;

         for (auto& path : url_image.paths())
         {
            if (!chapte_num_found && std::search(path.begin(), path.end(), chapter_num.begin(), chapter_num.end()) != path.end())
            {
               chapte_num_found = true;
            }
         }

         auto image_name = find_image_name(url_image);
         auto start = image_name.find_first_of("0123456789");
         auto pos = image_name.find_first_of('.');
         if (chapte_num_found && pos != astd::string_view::npos)
         {
            image_name = image_name.substr(start, pos - start);
            image_num_validated = image_name.find('-') != astd::string_view::npos
               || boost::lexical_cast<std::size_t>(image_name) >= boost::lexical_cast<std::size_t>(image_num);
         }
         
         if (chapte_num_found && image_num_validated)
         {
            keep_searching = false;
            result = url_image;
         }
         else
         {
            beg = search_result + pattern.size();
         }
      }
      else
      {
         keep_searching = false;
      }
   }
   return result;
}

xts::uri find_image_url(const std::vector<char>& buffer, const std::vector<std::string>& pattern, const astd::string_view& chapter_num, const astd::string_view& image_num)
{
   xts::uri result;
   for (auto& p : pattern)
   {
      result = find_image_url(buffer, p, chapter_num, image_num);
      if (result.size())
         return result;
   }
   return{};
}

xts::uri insure_complete_image_url(const xts::uri& image_uri, const xts::uri& source_uri)
{
   if (image_uri.absolute())
      return image_uri;

   xts::uri_builder builder(image_uri);
   builder.scheme = source_uri.scheme().to_string();
   builder.username = source_uri.user().to_string();
   builder.password = source_uri.password().to_string();
   builder.hostname = source_uri.hostname().to_string();
   builder.port = source_uri.port();
   return builder.assemble();
}

//return: image_name, image_content

Error_code::Type fetch_image::write_chapter(const fetch_image::chapter& chap)
{
   Error_code::Type result = Error_code::CHAPTER_NOT_FOUND;
   if (chap.images.size())
   {
      result = Error_code::NONE;

      if (!astd::filesystem::exists(chap.directory_full_path))
      {
         astd::filesystem::error_code ec;
         if (!astd::filesystem::create_directories(chap.directory_full_path, ec))
         {
            std::cerr << ec.message() << std::endl;
            result = Error_code::CREATE_DESTINATION_FAILED;
         }
      }

      if (result == Error_code::NONE)
      {
         for (auto& image : chap.images)
         {
            auto image_full_path = chap.directory_full_path / image.name;

            std::ofstream stream{ image_full_path.c_str(), std::ios::out | std::ios::trunc | std::ios::binary };
            if (!stream)
            {
               result = Error_code::CREATE_IMAGE_FILE_FAILED;
            }
            else
            {
               stream.write(image.content.data(), image.content.size());
            }
         }
      }

      if (result == Error_code::CREATE_IMAGE_FILE_FAILED)
      {
         erase_directory(chap.directory_full_path);
      }
   }

   return result;
}

void fetch_image::update_perms(const astd::filesystem::path path)
{
#ifdef __linux
{
   std::stringstream command;
   command << "sudo chown -R btsync:syncapp " << path;
   int result = std::system(command.str().c_str());
   if (result != EXIT_SUCCESS)
      std::cout << "chown failed" << std::endl;
}
{
   std::stringstream command;
   command << "sudo chmod +w " << path;
   int result = std::system(command.str().c_str());
   if (result != EXIT_SUCCESS)
      std::cout << "chmod failed" << std::endl;
}
#endif
}

Error_code::Type fetch_image::fetch_chapter(anime_database& db, const source_token& number_token, const source_token& image_token, std::size_t source_index, std::size_t number_index, const astd::filesystem::path& destination)
{
   chapter_getter getter(db, number_token, image_token, source_index, number_index, destination);
   auto chap = getter();

   Error_code::Type result = write_chapter(chap);
   if (result == Error_code::NONE)
   {
      std::cout << "push value " << number_token.values[number_index] << std::endl;
      db.number_fetched.push_back(number_token.values[number_index]);
      update_perms(chap.directory_full_path);
   }
   return result;
}

Error_code::Type fetch_image::fetch_from_source(anime_database& db, std::size_t source_index, const astd::string_view starting_number, const astd::filesystem::path& destination)
{
   source_token number_token(db.input.sources[source_index], "number", starting_number);
   if (!number_token.valid)
   {
      return Error_code::SOURCE_PATH_PARSING_FAILED;
   }

   source_token image_token(db.input.sources[source_index], "image", "0");
   if (!image_token.valid)
   {
      return Error_code::SOURCE_PATH_PARSING_FAILED;
   }

   Error_code::Type result = Error_code::CHAPTER_NOT_FOUND;
   for (std::size_t number_index = 0; 
      number_index < number_token.values.size() && result == Error_code::CHAPTER_NOT_FOUND; 
      ++number_index)
   {
      result = fetch_chapter(db, number_token, image_token, source_index, number_index, destination);
   }
   return result;
}

Error_code::Type fetch_image::fetch_next_number(anime_database& db, const astd::filesystem::path& destination)
{
   std::string starting_number;
   if (db.number_fetched.size())
   {
      starting_number = db.number_fetched.back();
   }
   else
   {
      starting_number = db.input.starting_number;
   }

   Error_code::Type result = Error_code::CHAPTER_NOT_FOUND;
   for (std::size_t source_index = 0; 
      source_index < db.input.sources.size() && result == Error_code::CHAPTER_NOT_FOUND; 
      ++source_index)
   {
      result = fetch_from_source(db, source_index, starting_number, destination);
   }
   return result;
}

std::pair<Error_code::Type, std::size_t> fetch_image::fetch(const configuration_input& input)
{
   Error_code::Type result = Error_code::NONE;
   anime_database db;
   
   auto valid_destination = std::find_if(input.destination.begin(), input.destination.end(), [](auto& possible_destination) 
   { 
#ifdef WIN32   
      return xts::uri(possible_destination.string()).scheme().size() > 0; 
#else //!WIN32
      return xts::uri(possible_destination.string()).scheme().size() == 0;
#endif
   });


   astd::filesystem::path destination;
   if (valid_destination != input.destination.end())
   {
      destination = *valid_destination;
   }
   else
   {
      return{ Error_code::DESTINATION_INVALID, 0 };
   }

   astd::filesystem::path db_path = destination / (input.name + ".db");

   auto path_string = astd::basic_string_view<astd::filesystem::path::value_type>(db_path.c_str());
   if (std::any_of(path_string.begin(), path_string.end(), [](auto val)
   { return val == '~'; }))
   {
      return{ Error_code::DESTINATION_INVALID, 0 };
   }

   astd::filesystem::error_code ec;
   if (!astd::filesystem::exists(destination)
      && !astd::filesystem::create_directories(destination, ec))
   {
      std::cerr << "create target directory failed! (dir: " << destination << ")" << std::endl;
      std::cerr << ec.message() << std::endl;
      return{ Error_code::CREATE_DESTINATION_FAILED, 0 };
   }

   if (astd::filesystem::exists(db_path))
   {
      bool parse_anime_ok;

      std::tie(parse_anime_ok, db) = anime_database::parse_anime_db(db_path);
      if (!parse_anime_ok)
      {
         std::cerr << "load db failed: " << db_path << std::endl;
         return{ Error_code::LOAD_DB_FAILED, 0 };
      }
   }

   db.input = input;

   std::size_t num_fetched = 0;

   while ((result = fetch_next_number(db, destination)) == Error_code::NONE)
   {
      num_fetched++;
      anime_database::dump_anime_db(db_path, db);
   }

   return{ result, num_fetched };
}

fetch_image::chapter_getter::chapter_getter(const anime_database& db, const source_token& number_token, const source_token& image_token, std::size_t source_index, std::size_t number_index, const astd::filesystem::path& destination)
   : _db(db), _number_token(number_token), _image_token(image_token), _source_index(source_index), _number_index(number_index), _destination(destination)
{}

bool confirm_page(const std::vector<char>& buffer, const xts::uri& full_url)
{
   auto paths = full_url.paths().data();
   return std::search(buffer.begin(), buffer.end(), paths.begin(), paths.end()) != buffer.end();
}

bool confirm_image(const xts::uri& image_uri, const astd::string_view& expected_chapter_num)
{
   auto paths = image_uri.paths().data();
   return std::search(paths.begin(), paths.end(), expected_chapter_num.begin(), expected_chapter_num.end()) != paths.end();
}

fetch_image::image fetch_image::chapter_getter::fetch_chapter_image(
   astd::string_view partial_url
   , std::size_t image_index
   , astd::string_view directory_name) const
{
   std::vector<char> webpage_buffer;
   webpage_buffer.reserve(FILE_BUFFER_SIZE);
   auto full_url = xts::uri(source_token::remplace_token(partial_url, _image_token, image_index));

   webpage_buffer.clear();
   bool get_success;
   std::string error_message;
   std::tie(get_success, error_message) = http::curl_get(full_url, [&webpage_buffer](astd::string_view buffer) mutable
   { webpage_buffer.insert(webpage_buffer.end(), buffer.begin(), buffer.end()); });

   if (get_success && !is_404(webpage_buffer))
   {
      auto image_url = find_image_url(webpage_buffer, _db.input.search_paths, _number_token.values[_number_index], _image_token.values[image_index]);
      if (image_url.size())
      {
         image_url = insure_complete_image_url(image_url, full_url);
         webpage_buffer.clear();

         std::tie(get_success, error_message) = http::curl_get(image_url, [&webpage_buffer](astd::string_view buffer) mutable
         { webpage_buffer.insert(webpage_buffer.end(), buffer.begin(), buffer.end()); });

         if (get_success && !is_404(webpage_buffer))
         {
            std::string image_name = directory_name.to_string() + "_" + find_image_name(image_url);

            return{ image_name, webpage_buffer };
         }
      }
      else
      {
         error_message = "image url not found in " + full_url.data();
      }
   }

   //std::cerr << "error while fetching a chapter at " << full_url << " error : " << error_message << std::endl;
   return{};
}


fetch_image::chapter fetch_image::chapter_getter::operator()() const
{
   std::string directory_name = _db.input.name + "_" + _db.input.language + "_" + _number_token.values[_number_index];
   std::string partial_url = source_token::remplace_token(_db.input.sources[_source_index], _number_token, _number_index);

   std::size_t image_index = 0;
   std::size_t size = _image_token.values.size();
   std::size_t batch_success_count = 1;

   chapter chap;
   chap.directory_full_path = _destination / directory_name;

   while (image_index < size && batch_success_count > 0)
   {
      std::vector< std::future< image > > image_get;
      for (std::size_t i = 0; i < MAX_ERROR_ON_CHAPTER && image_index < size; ++i)
      {         
         image_get.emplace_back(std::async(
            &chapter_getter::fetch_chapter_image, this, astd::string_view(partial_url), image_index, astd::string_view(directory_name)));
         ++image_index;
      }

      batch_success_count = 0;
      for (auto& image_get_result : image_get)
      {
         auto image = image_get_result.get();
         if (image.name.size())
         {
            batch_success_count++;
            if (std::find_if(chap.images.begin(), chap.images.end(), [&image](auto& inplace)
            { return inplace.name == image.name; }) == chap.images.end())
            {
               chap.images.emplace_back(image);
            }
         }
      }
   }

   return chap;
}