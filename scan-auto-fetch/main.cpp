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

int run(const std::vector <astd::filesystem::path>& config_paths)
{
   int result = EXIT_SUCCESS;

   std::vector<std::future<Error_code::Type>> fetch_results;
   for (auto& config_path : config_paths)
   {
      fetch_results.emplace_back(std::async(
#ifdef _DEBUG
         std::launch::deferred,
#endif
         [](const astd::filesystem::path& config_path)
      {
         Error_code::Type ec;
         configuration_input config;

         std::tie(ec, config) = configuration_input::parse_config(config_path);
         if (ec == Error_code::NONE)
         {
            std::size_t chapter_found;
            std::tie(ec, chapter_found) = fetch_image::fetch(config);
            std::cout << "fetching " << config.name << std::endl;
            if (chapter_found == 0)
            {
               std::cerr << Error_code::to_string(ec) << std::endl;
            }
            else
            {
               std::cout << "successfuly retrieved " << chapter_found << " new chapter" << std::endl;
            }
         }
         else
         {
            std::cout << "parsing configuration " << config_path << " failed" << std::endl;
            std::cout << "error : " << Error_code::to_string(ec) << std::endl;
         }
         return ec;
      }, config_path));
   }

   for (auto& fetch_result : fetch_results)
   {
      result = result & int(fetch_result.get());
   }
   return result;
}

int main(int argc, char** argv)
{
   boost::program_options::options_description desc("Allowed options");
   desc.add_options()("help", "produce help message")
      ("c", boost::program_options::value<std::vector<astd::filesystem::path>>(), "path to configuration file");

   boost::program_options::positional_options_description p;
   p.add("c", -1);

   int result = EXIT_SUCCESS;

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
   catch (boost::program_options::error&)
   {
      std::cerr << "parsing command line failed" << std::endl;
      std::cout << desc << std::endl;
      result = EXIT_FAILURE;
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
         result = run(path->second.as<std::vector<astd::filesystem::path>>());
      }
   }

   return result;
}
