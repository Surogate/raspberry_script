#ifndef SOURCE_TOKEN_HPP
#define SOURCE_TOKEN_HPP

#include <utility>
#include "astring_view.hpp"
#include <boost/lexical_cast.hpp>

#include "anime_db.hpp"

struct source_token
{
   bool valid;
   std::pair<std::size_t, std::size_t> place;
   std::vector<std::string> values;
   astd::string_view type;
   astd::string_view name;

   enum
   {
      IMAGE_MAX_NUM = 100
   };

   source_token(const astd::string_view source, const astd::string_view name, const astd::string_view number);

   std::size_t size() const;
   std::string full_name_type() const;

   static std::string remplace_token(astd::string_view source, const source_token& token, std::size_t value_index);

   static std::pair<std::size_t, std::size_t> parse_dotted(astd::string_view number);

   static std::string serialize_dotted(const std::pair<std::size_t, std::size_t>& dotted_num);

   template <std::size_t INTEGER_FORMAT_SIZE>
   static std::vector<std::string> image_token_integer(astd::string_view number)
   {
      std::vector<std::string> values;
      for (std::size_t i = 0; i < IMAGE_MAX_NUM; ++i)
      {
         std::string value = boost::lexical_cast<std::string>(i);
         while (value.size() < INTEGER_FORMAT_SIZE)
            value = std::string("0") + value;
         values.emplace_back(value);
      }

      return values;
   }

   static std::pair<bool, std::vector<std::string>> setup_token(astd::string_view token_name, astd::string_view token_type, astd::string_view number);

   static std::pair<std::size_t, std::size_t> get_token_place(astd::string_view source, astd::string_view token_name);

   static std::pair<bool, astd::string_view> get_type(astd::string_view source, const std::pair<std::size_t, std::size_t>& token_place);
};

#endif //!SOURCE_TOKEN_HPP
