
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <utility>
#include <future>
#include <fstream>

#include "input.hpp"

enum ip_test_result
{
   IP_ALL, //all ips responded
   IP_SOME, //only some ips responded
   IP_NONE, //none of the ips responded
   SIZE //enum size, keep it at the end
};

void launch_script_linear(const std::vector<std::string>& scripts)
{
   for (auto& sc : scripts)
   {
      std::system(sc.c_str());
   }
}

void launch_script_thread(const std::vector<std::string>& scripts)
{
   std::vector<std::future<void>> thread_result;
   thread_result.reserve(scripts.size());

   for (auto& sc : scripts)
   {
      thread_result.emplace_back(std::async([&sc]() {
         std::system(sc.c_str());
      }));
   }
   for (auto& fu : thread_result)
   {
      fu.wait();
   }
}

void launch_script(const std::vector<std::string>& scripts, bool thread)
{
   if (thread)
   {
      launch_script_thread(scripts);
   }
   else
   {
      launch_script_linear(scripts);
   }
}

ip_test_result test_ips(const input& in)
{
   std::vector<std::future<int>> ping_results;
   ping_results.reserve(in.ips.size());

   for (auto& ip : in.ips)
   {
      ping_results.emplace_back(std::async([&ip]() { 
         return std::system((std::string("ping ") + ip).c_str()); 
      }));
   }

   if (std::all_of(ping_results.begin(), ping_results.end(), 
      [](auto& fut) { return fut.get() == EXIT_SUCCESS; }))
      return IP_ALL;

   if (std::any_of(ping_results.begin(), ping_results.end(), 
      [](auto& fut) { return fut.get() == EXIT_SUCCESS; }))
      return IP_SOME;

   return IP_NONE;
}

int main(int argc, char** argv)
{
   auto vm = parse_program_option(argc, argv);

   if (vm.first == EXIT_SUCCESS)
   {
      input input_instance;

      auto result = parse_input(vm.second);
      if (result.first == EXIT_SUCCESS)
      {
         std::array<void(*)(const input&), SIZE> ip_state_machine = []() {
            std::array<void(*)(const input&), SIZE> result;
            result[IP_ALL] = [](const input& in) { launch_script(in.script_all, in.threaded_script); };
            result[IP_SOME] = [](const input& in) { launch_script(in.script_some, in.threaded_script); };
            result[IP_NONE] = [](const input& in) { launch_script(in.script_none, in.threaded_script); };
            return result;
         }();

         bool loop = true;
         do {
            ip_state_machine[test_ips(result.second)](input_instance);
            std::this_thread::sleep_for(input_instance.interval);
         } while (loop);
      }
      return result.first;
   }
   

   return EXIT_SUCCESS;
}