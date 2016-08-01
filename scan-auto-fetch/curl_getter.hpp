#ifndef CURL_GETTER_HPP
#define CURL_GETTER_HPP

#include <string>
#include <fstream>
#include <vector>
#include <utility>
#include "uri.hpp"
#include <curl/curl.h>

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

      Curl_Getter(const T& callback_)
         : callback(callback_), curl_message(CURL_ERROR_SIZE)
      {
         static curl_static curl_globals;
         handler = curl_easy_init();
      }

      ~Curl_Getter()
      {
         if (handler != nullptr)
            curl_easy_cleanup(handler);
      }

      std::pair<bool, std::string> get(const xts::uri& url)
      {
         bool success = false;
         std::string error_message;

         if (handler == nullptr)
         {
            error_message = "curl_easy_init return nullptr";
         }
         else if (!url.absolute())
         {
            error_message = "url " + url.data() + " is invalid";
         }
         else
         {
            std::string plain = url.data();

            curl_easy_setopt(handler, CURLOPT_URL, url.data().data());
            curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, writer_callback);
            curl_easy_setopt(handler, CURLOPT_WRITEDATA, this);
            curl_easy_setopt(handler, CURLOPT_ERRORBUFFER, curl_message.data());
            success = curl_easy_perform(handler) == CURLE_OK;
            if (!success && curl_message.size())
            {
               error_message = curl_message.data();
            }
         }
      
         return{ success, std::move(error_message) };
      }

      T callback;
      std::vector<char> curl_message;
      CURL* handler;
   };

   template <typename T>
   std::pair<bool, std::string> curl_get(const xts::uri& url, const T& callback)
   {
      Curl_Getter<T> getter(callback);
      return getter.get(url);
   }
}
#endif //!CURL_GETTER_HPP
