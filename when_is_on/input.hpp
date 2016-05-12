#ifndef INPUT_HPP_
#define INPUT_HPP_

#include <chrono>
#include <vector>
#include <ostream>

#include "boost/program_options.hpp"
#include "afilesystem_fwd.hpp"
#include "astring_view.hpp"

struct input
{
   std::chrono::seconds interval;
   std::vector<std::string> ips;
   std::vector<std::string> script_all;
   std::vector<std::string> script_none;
   std::vector<std::string> script_some;
   bool threaded_script;
};

std::ostream& operator<<(std::ostream& stream, const input& in);
std::pair<int, boost::program_options::variables_map> parse_program_option(int argc, char** argv);
std::pair<int, input> parse_input(const boost::program_options::variables_map& vm);
std::pair<int, std::vector<std::string>> parse_param(const boost::program_options::variables_map& vm, const astd::string_view param_name);
std::pair<int, std::vector<std::string>> fetch_file_line(const astd::filesystem::path& path);

#endif //!INPUT_HPP_