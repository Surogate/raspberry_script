#ifndef ERROR_CODE_HPP
#define ERROR_CODE_HPP

#include <vector>
#include <stdexcept>
#include "astring_view.hpp"

namespace Error_code
{
   enum Type : int
   {
      NONE,
      CREATE_DESTINATION_FAILED,
      DESTINATION_INVALID,
      FILE_NOT_FOUND,
      LOAD_DB_FAILED,
      SOURCE_PATH_PARSING_FAILED,
      DONT_FIND_CHAPTER_NUMBER,
      CREATE_IMAGE_FILE_FAILED,
      CHAPTER_NOT_FOUND,
      IMAGE_NOT_FOUND
   };

const std::vector<astd::string_view>& convert_array();

astd::string_view to_string(const Error_code::Type& errnum);

Error_code::Type parse(const astd::string_view str);

}

#endif