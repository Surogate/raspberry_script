#ifndef INPUT_HPP_
#define INPUT_HPP_

#include "afilesystem_fwd.hpp"
#include "astring_view.hpp"
#include "return_status.hpp"

#include "boost/program_options/variables_map.hpp"

#include <chrono>
#include <vector>
#include <sstream>
#include <ostream>
#include <cstdint>

struct ipv4
{
	std::uint8_t first;
	std::uint8_t second;
	std::uint8_t third;
	std::uint8_t fourth;

	ipv4() = default;
	ipv4(const ipv4&) = default;
	ipv4(ipv4&&) noexcept = default;
	ipv4& operator=(const ipv4&) = default;
	ipv4& operator=(ipv4&&) noexcept= default;

	static ipv4 parse_from_string(const std::string& view)
	{
		ipv4 result;
		std::stringstream parser(view);
		parser >> result.first;
		parser >> result.second;
		parser >> result.third;
		parser >> result.fourth;
		return result;
	}

	static std::string serialize_to_string(const ipv4& ip)
	{
		std::stringstream str;
		str << ip.first << ' ' << ip.second << ' ' << ip.third << ' ' << ip.fourth;
		return str.str();
	}
};
	

struct input
{
   std::chrono::seconds interval;
   std::vector<ipv4> ips;
   std::vector<std::string> script_all;
   std::vector<std::string> script_none;
   std::vector<std::string> script_some;
   bool threaded_script;
};

std::ostream& operator<<(std::ostream& stream, const input& in);
xts::return_status<boost::program_options::variables_map> parse_program_option(int argc, char** argv);
xts::return_status<input> parse_input(const boost::program_options::variables_map& vm);
xts::return_status<std::vector<std::string>> parse_param(const boost::program_options::variables_map& vm, const astd::string_view param_name);
xts::return_status<std::vector<std::string>> fetch_file_line(const astd::filesystem::path& path);

#endif //!INPUT_HPP_