#ifndef FETCH_IMAGE_HPP
#define FETCH_IMAGE_HPP

#include <string>
#include <vector>
#include <map>
#include <future>
#include "afilesystem.hpp"
#include "configuration_input.hpp"
#include "anime_db.hpp"
#include "source_token.hpp"
#include "curl_getter.hpp"
#include "error_code.hpp"
#include "astring_view.hpp"

struct fetch_image 
{
   
   struct image
   {
      std::string name;
      std::vector<char> content;
   };

   struct chapter
   {
      astd::filesystem::path directory_full_path;
      std::vector<image> images;
   };

   struct chapter_getter
   {
      chapter_getter(const anime_database& db
         , const source_token& number_token
         , const source_token& image_token
         , std::size_t source_index
         , std::size_t number_index
         , const astd::filesystem::path& destination);

      chapter operator()() const;

   private:
      image fetch_chapter_image(
          astd::string_view partial_url
         , std::size_t image_index
         , astd::string_view directory_name) const;

      enum
      {
         MAX_ERROR_ON_CHAPTER = 5,
         FILE_BUFFER_SIZE = 2018
      };

      const anime_database& _db;
      const source_token& _number_token;
      const source_token& _image_token;
      std::size_t _source_index; 
      std::size_t _number_index;
      const astd::filesystem::path& _destination;
   };

   static void erase_directory(const astd::filesystem::path& path);

   static bool is_404(const std::vector<char>& buffer);

   static Error_code::Type write_chapter(const chapter& chap);

   static void update_perms(const astd::filesystem::path path);

   static Error_code::Type fetch_chapter(anime_database& db, const source_token& number_token, const source_token& image_token, std::size_t source_index, std::size_t number_index, const astd::filesystem::path& destination);

   static Error_code::Type fetch_from_source(anime_database& db, std::size_t source_index, const astd::string_view starting_number, const astd::filesystem::path& destination);

   static Error_code::Type fetch_next_number(anime_database& db, const astd::filesystem::path& destination);

   static std::pair<Error_code::Type, std::size_t> fetch(const configuration_input& input);
};



#endif //!FETCH_IMAGE_HPP
