#ifndef CONFIGURATION_INPUT_HPP
#define CONFIGURATION_INPUT_HPP

#include <string>
#include <fstream>
#include <unordered_map>
#include <iostream>
#include <string>

#include "afilesystem.hpp"
#include "astring_view.hpp"
#include "boost/tokenizer.hpp"

struct configuration_input
{
   std::string name;
   std::vector<std::string> sources;
   astd::filesystem::path destination;
   std::string starting_number = "0";
   std::string language;

   static void dump_value(std::ostream& stream, const astd::string_view name, const astd::string_view value)
   {
      if (name.size() && value.size())
      {
         stream << name << " = " << value << std::endl;
      }
   }

   static bool dump_config(std::ostream& stream, const configuration_input& value)
   {
      if (stream)
      {
         dump_value(stream, "name", value.name);
         for (auto& s : value.sources)
            dump_value(stream, "source", s);
         dump_value(stream, "destination", value.destination.string());
         dump_value(stream, "starting_number", value.starting_number);
         dump_value(stream, "language", value.language);
         return true;
      }
      return false;
   }

   static bool parse_line(const std::string& str, configuration_input& input)
   {
      boost::char_separator<char> sep{" "};
      boost::tokenizer<boost::char_separator<char>> tok(str, sep);
      std::array<std::string, 3> values;
      std::size_t i = 0;
      for (auto& t : tok)
      {
         values[i % values.size()] = t;
         ++i;
      }
      if (std::all_of(values.begin(), values.end(), [](auto& string) {return string.size() != 0; })
         && values[1] == "=")
      {
         typedef std::unordered_map< astd::string_view, void(*)(configuration_input& input, std::string& value) > statemachine_map;
         static statemachine_map state_machine = {
            { "name", [](configuration_input& input, std::string& value) { input.name = std::move(value); } },
            { "source", [](configuration_input& input, std::string& value) { input.sources.emplace_back(std::move(value)); } },
            { "destination", [](configuration_input& input, std::string& value) { input.destination = std::move(value); } },
            { "starting_number", [](configuration_input& input, std::string& value) { input.starting_number = std::move(value); } },
            { "language", [](configuration_input& input, std::string& value) { input.language = std::move(value); } }
         };

         auto it = state_machine.find(values.front());
         if (it != state_machine.end())
         {
            it->second(input, values.back());
            return true;
         }
         else
         {
            std::cerr << "error on key: " << values.front() << std::endl;
         }
      }
      return false;
   }

   static std::pair<bool, configuration_input> parse_config(const astd::filesystem::path& config_path)
   {
      std::pair<bool, configuration_input> result{ false, configuration_input() };
      std::ifstream stream{ config_path.c_str() };
      
      if (stream)
      {
         bool ok = true;
         result.first = true;
         std::string line;
         while (ok && std::getline(stream, line))
         {
            ok = parse_line(line, result.second);
         }
      }

      return result;
   }
};




#endif //!CONFIGURATION_INPUT_HPP