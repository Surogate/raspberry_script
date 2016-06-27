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
#include "trim.hpp"

struct configuration_input
{
   std::string name;
   std::vector<std::string> sources;
   astd::filesystem::path destination;
   std::string starting_number = "0";
   std::string language;

   static void dump_value(std::ostream& stream, const astd::string_view name, const astd::string_view value);

   static bool dump_config(std::ostream& stream, const configuration_input& value);

   static bool parse_line(astd::string_view str, configuration_input& input);

   static std::pair<bool, configuration_input> parse_config(const astd::filesystem::path& config_path);
};




#endif //!CONFIGURATION_INPUT_HPP
