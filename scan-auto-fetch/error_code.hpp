#ifndef ERROR_CODE_HPP
#define ERROR_CODE_HPP

#include <vector>
#include <stdexcept>
#include "astring_view.hpp"

enum ERROR_CODE : int
{
   NONE,
   CREATE_DESTINATION_FAILED,
   LOAD_DB_FAILED,
   SOURCE_PATH_PARSING_FAILED,
   DONT_FIND_CHAPTER_NUMBER,
   CREATE_IMAGE_FILE_FAILED,
   CHAPTER_NOT_FOUND,
   IMAGE_NOT_FOUND
};

const std::vector<astd::string_view>& convert_array()
{
   static std::vector<astd::string_view> arr
   {
      "NONE",
      "CREATE_DESTINATION_FAILED",
      "LOAD_DB_FAILED",
      "SOURCE_PATH_PARSING_FAILED",
      "DONT_FIND_CHAPTER_NUMBER",
      "CREATE_IMAGE_FILE_FAILED",
      "CHAPTER_NOT_FOUND",
      "IMAGE_NOT_FOUND"
   };
   return arr;
}

astd::string_view to_string(const ERROR_CODE& errnum)
{
   auto& arr = convert_array();
   if (errnum > 0 && errnum < (int)arr.size())
   {
      return arr[errnum];
   }
   return{};
}

ERROR_CODE parse(const astd::string_view str)
{
   auto& arr = convert_array();
   auto it = std::find(arr.begin(), arr.end(), str);
   if (it != arr.end())
   {
      return ERROR_CODE(std::distance(arr.begin(), it));
   }
   throw std::runtime_error("bad ERROR_CODE");
}

#endif