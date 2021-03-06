
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "boost/program_options.hpp"
#include "afilesystem.hpp"
#include "astring_view.hpp"

#include "input.hpp"

std::ostream& operator<<(std::ostream& stream, const input& in)
{
   stream << "interval " << in.interval.count() << " second" << std::endl;
   stream << "thread " << (in.threaded_script ? "true" : "false"); stream << std::endl;
   stream << "ips" << std::endl;
   for (auto& ip : in.ips)
      stream << ipv4::serialize_to_string(ip) << std::endl;
   stream << "script_all" << std::endl;
   for (auto& sc : in.script_all)
      stream << sc << std::endl;
   stream << "script_some" << std::endl;
   for (auto& sc : in.script_some)
      stream << sc << std::endl;
   stream << "script_none" << std::endl;
   for (auto& sc : in.script_none)
      stream << sc << std::endl;
   return stream;
}

xts::return_status<boost::program_options::variables_map> parse_program_option(int argc, char** argv)
{
   int status = EXIT_SUCCESS;
   boost::program_options::options_description desc("Allowed options");
   desc.add_options()("help", "produce help message")
      ("ip", boost::program_options::value<astd::filesystem::path>(), "path to the file containing ip addresses")
      ("t", boost::program_options::value<uint32_t>(), "time in second between ip tests")
      ("no_concurrency", "scripts are not launch concurrently")
      ("script_all", boost::program_options::value<astd::filesystem::path>(), "path to the file containing the list of script to launch when all ip answer to ping")
      ("script_none", boost::program_options::value<astd::filesystem::path>(), "path to the file containing the list of script to launch when all ip does not answer to ping")
      ("script_some", boost::program_options::value<astd::filesystem::path>(), "path to the file containing the list of script to launch when at least one ip answer to ping")
      ;

   boost::program_options::variables_map vm;

   try {
      boost::program_options::store(
         boost::program_options::command_line_parser(argc, argv)
         .options(desc)
         .style(boost::program_options::command_line_style::unix_style
            | boost::program_options::command_line_style::
            allow_long_disguise)
         .allow_unregistered()
         .run(),
         vm);
      boost::program_options::notify(vm);
   }
   catch (boost::program_options::error e)
   {
      status = EXIT_FAILURE;
      std::cout << desc << std::endl;
   }

   if (vm.count("help"))
   {
      status = EXIT_FAILURE;
      std::cout << desc << std::endl;
   }

   return xts::return_status<boost::program_options::variables_map>(status, std::move(vm));
}

xts::return_status<std::vector<std::string>> fetch_file_line(const astd::filesystem::path& path)
{
   int status = EXIT_FAILURE;
   std::cout << path << std::endl;
   std::ifstream stream(path.c_str());
   std::vector<std::string> result;
   if (stream)
   {
      std::string buffer;

      while (stream && getline(stream, buffer))
      {
         result.push_back(buffer);
      }
      status = EXIT_SUCCESS;
   }

   return xts::return_status<std::vector<std::string>>(status, std::move(result));
}

xts::return_status<std::vector<std::string>> parse_param(const boost::program_options::variables_map& vm, const astd::string_view param_name)
{
   auto it = vm.find(param_name.data());
   if (it != vm.end())
   {
      std::cout << param_name << " ";
      return fetch_file_line(it->second.as<astd::filesystem::path>());
   }
   return xts::return_status<std::vector<std::string>>();
}

xts::return_status<std::vector<ipv4>> parse_to_ips(const xts::return_status<std::vector<std::string>>& input)
{
	std::vector<ipv4> result;
	if (input)
	{
		for (const auto& line : input.value())
		{
			result.emplace_back(ipv4::parse_from_string(line));
		}
	}
	return xts::return_status<std::vector<ipv4>>( input.status() , std::move(result) );
}

xts::return_status<input> parse_input(const boost::program_options::variables_map& vm)
{
   int result = EXIT_SUCCESS;
   input input_instance;

   auto ip_param = parse_to_ips(parse_param(vm, "ip"));
   if (ip_param)
   {
      input_instance.ips = ip_param.value();
   }
   else { result = EXIT_FAILURE; }

   auto t_param = vm.find("t");
   if (t_param != vm.end())
   {
      input_instance.interval = std::chrono::seconds(t_param->second.as<uint32_t>());
   }
   else {
      std::cerr << "error: no timer found !" << std::endl;
      result = EXIT_FAILURE; 
   }

   input_instance.threaded_script = vm.find("no_concurrency") == vm.end();

   auto script_allp = parse_param(vm, "script_all");
   if (script_allp)
   {
      input_instance.script_all = script_allp.value();
   }

   auto script_somep = parse_param(vm, "script_some");
   if (script_somep)
   {
      input_instance.script_some = script_somep.value();
   }

   auto script_nonep = parse_param(vm, "script_none");
   if (script_nonep)
   {
      input_instance.script_none = script_nonep.value();
   }

   if (!input_instance.ips.size())
   {
      result = EXIT_FAILURE;
      std::cerr << "error: no ips found !" << std::endl;
   }

   if (!input_instance.script_all.size() && !input_instance.script_some.size() && !input_instance.script_none.size())
   {
      result = EXIT_FAILURE;
      std::cerr << "error: no script found !" << std::endl;
   }

   std::cout << input_instance << std::endl;

   return xts::return_status<input>(result, std::move(input_instance));
}