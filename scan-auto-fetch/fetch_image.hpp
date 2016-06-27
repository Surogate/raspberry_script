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
   enum
   {
      MAX_ERROR_ON_CHAPTER = 15,
      FILE_BUFFER_SIZE = 2018
   };

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

   static void erase_directory(const astd::filesystem::path& path);

   static bool is_404(const std::vector<char>& buffer);
   
   static image fetch_chapter_image(std::string partial_url
      , const source_token& image_token
      , std::size_t image_index
      , astd::string_view directory_name);

   static int write_chapter(const chapter& chap);

   static void update_perms(const astd::filesystem::path path);

   static int fetch_chapter(anime_database& db, const source_token& number_token, const source_token& image_token, std::size_t source_index, std::size_t number_index);

   static int fetch_from_source(anime_database& db, std::size_t source_index, const astd::string_view starting_number);

   static int fetch_next_number(anime_database& db);

   static std::pair<int, std::size_t> fetch(const configuration_input& input);
};



#endif //!FETCH_IMAGE_HPP
