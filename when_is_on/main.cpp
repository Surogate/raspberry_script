
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <utility>
#include <future>
#include <fstream>

#include "input.hpp"

enum ip_test_result
{
   IP_NONE, //none of the ips responded
   IP_ALL, //all ips responded
   IP_SOME, //only some ips responded
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
     ping_results.emplace_back(std::async(std::launch::deferred, [&ip]() { 
         return std::system((std::string("ping ") + ip).c_str()); 
      }));
   }


   bool all = true;
   bool some = false;
   for (auto& fut : ping_results)
   {
      if (fut.get() == EXIT_SUCCESS)
      {
         some = true;
      }
      else
      {
         all = false;
      }
   }
   if (all) return IP_ALL;
   if (some) return IP_SOME;
   return IP_NONE;
}

int main(int argc, char** argv)
{
   auto vm = parse_program_option(argc, argv);

   if (vm)
   {
      auto result = parse_input(vm.value());
      if (result)
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
            ip_state_machine[test_ips(result.value())](result.value());
	    if (result.value().interval.count())
	      std::this_thread::sleep_for(result.value().interval);
	    else
	      loop = false;
         } while (loop);
      }
      return result.status();
   }

   return EXIT_FAILURE;
}
