#ifndef ANIME_DB_HPP
#define ANIME_DB_HPP

#include <vector>
#include <string>

#include "afilesystem.hpp"
#include "configuration_input.hpp"

struct anime_database
{
   configuration_input input;
   std::vector<std::string> number_fetched;

   static bool dump_anime_db(const astd::filesystem::path& db_path, const anime_database& db);

   static std::pair<bool, anime_database> parse_anime_db(const astd::filesystem::path& db_path);
};



#endif //!ANIME_DB_HPP
