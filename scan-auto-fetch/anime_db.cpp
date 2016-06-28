#include <fstream>
#include <algorithm>

#include "anime_db.hpp"
#include "trim.hpp"

bool anime_database::dump_anime_db(const astd::filesystem::path& db_path, const anime_database& db)
{
   bool result = false;
   std::ofstream stream{ db_path.c_str(), std::ios::out | std::ios::trunc };

   if (stream)
   {
      result = true;
      configuration_input::dump_config(stream, db.input);
      for (auto& num : db.number_fetched)
      {
         stream << num << std::endl;
      }
   }
   return result;
}

std::pair<bool, anime_database> anime_database::parse_anime_db(const astd::filesystem::path& db_path)
{
   std::pair<bool, anime_database> result{ false, anime_database() };

   std::ifstream stream{ db_path.c_str() };
   if (stream)
   {
      std::string line;
      result.first = true;
      bool ok = true;
      std::size_t line_num = 1;
      while (result.first && std::getline(stream, line))
      {
         auto trimmed_line = xts::trim<char>(line);
         ok = ok && configuration_input::parse_line(trimmed_line, result.second.input);
         if (!ok)
         {
            if (std::all_of(trimmed_line.begin(), trimmed_line.end(), [](auto& c) { return std::isdigit(c) != 0 || c == '.'; })) //if the line is a interger/float
            {
               result.second.number_fetched.emplace_back(trimmed_line.to_string());
            }
            else //something when bad
            {
               result.first = false;
            }
         }

         if (!result.first)
            std::cerr << "error at line : no " << line_num << " : " << line << std::endl;

         line_num++;
      }
   }
   return result;
}