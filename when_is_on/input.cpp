
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
   stream << "thread " << in.threaded_script ? "true" : "false"; stream << std::endl;
   stream << "ips" << std::endl;
   for (auto& ip : in.ips)
      stream << ip << std::endl;
   stream << "script_all" << std::endl;
   for (auto& sc : in.script_all)
      stream << sc << std::endl;
   stream << "script_some" << std::endl;
   for (auto& sc : in.script_some)
      stream << sc << std::endl;
   stream << "script_none" << std::endl;
   for (auto& sc : in.script_none)
      stream << sc << std::endl;
}

std::pair<int, boost::program_options::variables_map> parse_program_option(int argc, char** argv)
{
   int status = EXIT_SUCCESS;
   boost::program_options::options_description desc("Allowed options");
   desc.add_options()("help", "produce help message")
      ("ip", boost::program_options::value<std::vector<astd::filesystem::path>>(), "path to the file containing ip addresses")
      ("t", boost::program_options::value<uint32_t>(), "time in second between ip tests")
      ("no_concurrency", "scripts are not launch concurrently")
      ("script_all", boost::program_options::value<std::vector<astd::filesystem::path>>(), "path to the file containing the list of script to launch when all ip answer to ping")
      ("script_none", boost::program_options::value<std::vector<astd::filesystem::path>>(), "path to the file containing the list of script to launch when all ip does not answer to ping")
      ("script_some", boost::program_options::value<std::vector<astd::filesystem::path>>(), "path to the file containing the list of script to launch when at least one ip answer to ping")
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

   return std::make_pair(status, vm);
}

std::pair<int, std::vector<std::string>> fetch_file_line(const astd::filesystem::path& path)
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

   return std::make_pair(status, result);
}

std::pair<int, std::vector<std::string>> parse_param(const boost::program_options::variables_map& vm, const astd::string_view param_name)
{
   auto it = vm.find(param_name.data());
   if (it != vm.end())
   {
      std::cout << param_name;
      return fetch_file_line(it->second.as<astd::filesystem::path>());
   }
   return std::make_pair(EXIT_FAILURE, std::vector<std::string>());
}

std::pair<int, input> parse_input(const boost::program_options::variables_map& vm)
{
   int result = EXIT_SUCCESS;
   input input_instance;

   auto ip_param = parse_param(vm, "ip");
   if (ip_param.first == EXIT_SUCCESS)
   {
      input_instance.ips = ip_param.second;
   }
   else { result = EXIT_FAILURE; }

   auto t_param = vm.find("t");
   if (t_param != vm.end())
   {
      input_instance.interval = std::chrono::seconds(t_param->second.as<uint32_t>());
   }
   else { result = EXIT_FAILURE; }

   input_instance.threaded_script = vm.find("no_concurrency") == vm.end();

   auto script_allp = parse_param(vm, "script_all");
   if (script_allp.first == EXIT_SUCCESS)
   {
      input_instance.script_all = script_allp.second;
   }
   else { result = EXIT_FAILURE; }

   auto script_somep = parse_param(vm, "script_some");
   if (script_somep.first == EXIT_SUCCESS)
   {
      input_instance.script_some = script_somep.second;
   }
   else { result = EXIT_FAILURE; }

   auto script_nonep = parse_param(vm, "script_none");
   if (script_nonep.first == EXIT_SUCCESS)
   {
      input_instance.script_none = script_nonep.second;
   }
   else { result = EXIT_FAILURE; }

   return std::make_pair(result, std::move(input_instance));
}