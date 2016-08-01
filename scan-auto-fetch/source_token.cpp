
#include "source_token.hpp"

#include <map>
#include <string>
#include "astring_view.hpp"
#include <utility>


source_token::source_token(const astd::string_view source, const astd::string_view name, const astd::string_view number)
{
   this->name = name;
   std::string token_name = std::string("<") + name;
   place = get_token_place(source, token_name);
   values = {};
   valid = false;
   if (place.first && place.second)
   {
      bool success;
      std::tie(success, type) = get_type(source, place);
      if (success)
      {
         std::tie(valid, values) = setup_token(name, type, number);         
      }
   }
}

std::size_t source_token::size() const { return place.second - place.first; }
std::string source_token::full_name_type() const { return std::string("<").append(name.data(), name.size()).append(":").append(type.data(), type.size()).append(">"); }

std::string source_token::remplace_token(astd::string_view source, const source_token& token, std::size_t value_index)
{
   return source.to_string().replace(source.find(token.full_name_type()), token.size() + 1, token.values[value_index]); //size() + 1 to remove the last '>'
}

std::pair<std::size_t, std::size_t> source_token::parse_dotted(astd::string_view number)
{
   std::pair<std::size_t, std::size_t> result;

   auto dot = number.find('.');
   result.first = boost::lexical_cast<std::size_t>(number.substr(0, dot));
   result.second = 0;
   if (dot != astd::string_view::npos)
   {
      result.second = boost::lexical_cast<std::size_t>(number.substr(dot + 1, number.size()));
   }
   return result;
}

std::string source_token::serialize_dotted(const std::pair<std::size_t, std::size_t>& dotted_num)
{
   if (dotted_num.second)
      return boost::lexical_cast<std::string>(dotted_num.first) + '.' + boost::lexical_cast<std::string>(dotted_num.second);
   return boost::lexical_cast<std::string>(dotted_num.first);
}

std::pair<bool, std::vector<std::string>> source_token::setup_token(astd::string_view token_name, astd::string_view token_type, astd::string_view number)
{
   typedef std::map<
      /*underlying type*/  astd::string_view,
      /*function ptr*/     std::vector<std::string>(*)(astd::string_view)
   > string_func_ptr_map;
   static std::map< /*token name*/astd::string_view, string_func_ptr_map > state_machine =
   {
      { "number", //pair & token name
      { //string_func_ptr_map
         { "doted", [](astd::string_view number) //pair & token type 
   {
      std::vector<std::string> values(3);
      auto dotted = parse_dotted(number);
      {
         auto next = dotted;
         next.second++;
         values[0].append(serialize_dotted(next));
      }
      {
         auto next = dotted;
         next.second += 2;
         values[1].append(serialize_dotted(next));
      }
      {
         auto next = dotted;
         next.first++;
         next.second = 0;
         values[2].append(serialize_dotted(next));
      }
      return values;
   }
         }, //!pair & token type
         { "integer", [](astd::string_view number) //pair & token type 
   {
      std::vector<std::string> values;
      values.emplace_back(boost::lexical_cast<std::string>(boost::lexical_cast<int>(number) + 1));
      return values;
   }
         } //!pair & token type
      } //!string_func_ptr_map
      },
      { "image", //pair & token name
      { //string_func_ptr_map
         { "2_integer", &image_token_integer<2> },
         { "3_integer", &image_token_integer<3> },
         { "4_integer", &image_token_integer<4> },
         { "5_integer", &image_token_integer<5> },
         { "integer", &image_token_integer<0> }
      }//!string_func_ptr_map 
      } //!pair              
   };

   auto it_name = state_machine.find(token_name);
   if (it_name != state_machine.end())
   {
      auto it_type = it_name->second.find(token_type);
      if (it_type != it_name->second.end())
         return{ true, it_type->second(number) };
   }
   return{ false,{} };
}

std::pair<std::size_t, std::size_t> source_token::get_token_place(astd::string_view source, astd::string_view token_name)
{
   auto start = source.find(token_name);
   if (start != std::string::npos)
   {
      auto end = start;
      while (end != source.size() && source[end] != '>')
         end++;
      if (end != source.size())
      {
         return{ start, end };
      }
   }
   return{ 0, 0 };
}

std::pair<bool, astd::string_view> source_token::get_type(astd::string_view source, const std::pair<std::size_t, std::size_t>& token_place)
{
   auto token = source.substr(token_place.first + 1, token_place.second - (token_place.first + 1));
   auto place = token.find(':');
   if (place != astd::string_view::npos)
   {
      return{ true, token.substr(place + 1) };
   }
   return{ false,{} };
}
