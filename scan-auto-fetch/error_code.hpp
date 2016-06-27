#ifndef ERROR_CODE_HPP
#define ERROR_CODE_HPP

#include <vector>
#include <stdexcept>
#include "astring_view.hpp"

enum ERROR_CODE : int
{
   NONE,
   CREATE_DESTINATION_FAILED,
   DESTINATION_INVALID,
   LOAD_DB_FAILED,
   SOURCE_PATH_PARSING_FAILED,
   DONT_FIND_CHAPTER_NUMBER,
   CREATE_IMAGE_FILE_FAILED,
   CHAPTER_NOT_FOUND,
   IMAGE_NOT_FOUND
};

inline const std::vector<astd::string_view>& convert_array();

inline astd::string_view to_string(const ERROR_CODE& errnum);

inline ERROR_CODE parse(const astd::string_view str);

#endif