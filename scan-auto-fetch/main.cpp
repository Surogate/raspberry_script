#include <iostream>
#include <utility>
#include <future>
#include "afilesystem.hpp"
#include "boost/program_options/options_description.hpp"
#include "boost/program_options/positional_options.hpp"
#include "boost/program_options/variables_map.hpp"
#include "boost/program_options/parsers.hpp"

#include "configuration_input.hpp"
#include "fetch_image.hpp"
#include "curl_getter.hpp"
#include "error_code.hpp"

int main(int argc, char** argv)
{
   boost::program_options::options_description desc("Allowed options");
   desc.add_options()("help", "produce help message")
      ("c", boost::program_options::value<std::vector<astd::filesystem::path>>(), "path to configuration file");

   boost::program_options::positional_options_description p;
   p.add("c", -1);

   int result = 0;

   boost::program_options::variables_map vm;
   try {
      boost::program_options::store(
         boost::program_options::command_line_parser(argc, argv)
         .options(desc)
         .positional(p)
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
      std::cout << desc << std::endl;
      result = 0;
   }

   if (vm.count("help"))
   {
      std::cout << desc << std::endl;
   }
   else
   {
      auto path = vm.find("c");
      if (path != vm.end())
      {
         std::vector<std::future<int>> fetch_results;
         for (auto& config_path : path->second.as<std::vector<astd::filesystem::path>>())
         {
            fetch_results.emplace_back(std::async(
               [](const astd::filesystem::path& config_path)
            {
               auto result = 0;
               auto conf_input = configuration_input::parse_config(config_path);
               if (conf_input.first)
               {
                  auto fetch_result = fetch_image::fetch(conf_input.second);
                  std::cout << "fetching " << conf_input.second.name << std::endl;
                  if (fetch_result.second == 0)
                  {
                     result = fetch_result.first;
                     std::cerr << "Error: " << to_string(ERROR_CODE(result)) << std::endl;
                  }
                  else
                  {
                     std::cout << "successfuly retrieved " << fetch_result.second << " new chapter" << std::endl;
                  }
               }
               return result;               
            }, config_path));
         }

         for (auto& fetch_result : fetch_results)
         {
            result = result & fetch_result.get();
         }
      }
   }

   return result;
}