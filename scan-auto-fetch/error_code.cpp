#include "error_code.hpp"
#include <algorithm>

namespace Error_code
{
   const std::vector<astd::string_view>& convert_array()
   {
      static std::vector<astd::string_view> arr
      {
         "NONE",
         "CREATE_DESTINATION_FAILED",
         "DESTINATION_INVALID",
         "FILE_NOT_FOUND",
         "LOAD_DB_FAILED",
         "SOURCE_PATH_PARSING_FAILED",
         "DONT_FIND_CHAPTER_NUMBER",
         "CREATE_IMAGE_FILE_FAILED",
         "CHAPTER_NOT_FOUND",
         "IMAGE_NOT_FOUND"
      };
      return arr;
   }

   astd::string_view to_string(const Error_code::Type& errnum)
   {
      auto& arr = convert_array();
      if (errnum > 0 && errnum < (int)arr.size())
      {
         return arr[errnum];
      }
      return{};
   }

   Error_code::Type parse(const astd::string_view str)
   {
      auto& arr = convert_array();
      auto it = std::find(arr.begin(), arr.end(), str);
      if (it != arr.end())
      {
         return Error_code::Type(std::distance(arr.begin(), it));
      }
      throw std::runtime_error("bad ERROR_CODE");
   }

}
