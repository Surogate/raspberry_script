#ifndef CURL_GETTER_HPP
#define CURL_GETTER_HPP

#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include "astring_view.hpp"
#include "curl.h"

namespace http
{
   template <typename T>
   class Curl_Getter
   {
      struct curl_static {
         curl_static() { curl_global_init(CURL_GLOBAL_ALL); }
         ~curl_static() { curl_global_cleanup(); }
      };

      static std::size_t writer_callback(const char* buffer, std::size_t size, std::size_t nmemb, Curl_Getter* this_ptr)
      {
         if (size * nmemb)
         {
            this_ptr->callback(astd::string_view(buffer, size * nmemb));
         }
         return size * nmemb;
      }

   public:

      Curl_Getter(T& callback_)
         : callback(callback_), error_message(CURL_ERROR_SIZE)
      {
         static curl_static curl_globals;
         handler = curl_easy_init();
      }

      ~Curl_Getter()
      {
         if (handler != nullptr)
            curl_easy_cleanup(handler);
      }

      std::pair<bool, std::string> get(const std::string& url)
      {
         std::pair<bool, std::string> result{ false, std::string() };

         if (handler == nullptr)
         {
            result.second = "curl_easy_init return nullptr";
            return result;
         }

         curl_easy_setopt(handler, CURLOPT_URL, url.c_str());
         curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, writer_callback);
         curl_easy_setopt(handler, CURLOPT_WRITEDATA, this);
         curl_easy_setopt(handler, CURLOPT_ERRORBUFFER, error_message.data());
         result.first = curl_easy_perform(handler) == CURLE_OK;
         if (!result.first)
         {
            result.second = error_message.data();
         }
      
         return result;
      }

      T& callback;
      std::vector<char> error_message;
      CURL* handler;
   };

   template <typename T>
   std::pair<bool, std::string> curl_get(const astd::string_view url, T& callback)
   {
      Curl_Getter<T> getter(callback);
      return getter.get({ url.data(), url.size() });
   }
}
#endif //!CURL_GETTER_HPP