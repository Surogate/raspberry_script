#ifndef SOURCE_TOKEN_HPP
#define SOURCE_TOKEN_HPP

#include <utility>
#include "boost/utility/string_ref.hpp"
#include "boost/lexical_cast.hpp"

#include "anime_db.hpp"

struct source_token
{
   bool valid;
   std::pair<std::size_t, std::size_t> place;
   std::vector<std::string> values;
   boost::string_ref type;
   boost::string_ref name;

   enum
   {
      IMAGE_MAX_NUM = 100
   };

   source_token(const astd::string_view source, const astd::string_view name, const astd::string_view number)
   {
      this->name = name;
      std::string token_name = std::string("<").append(name.data(), name.size());
      place = get_token_place(source, token_name);
      values = {};
      valid = false;
      if (place.first && place.second)
      {
         auto type_result = get_type(source, place);
         if (type_result.first)
         {
            type = std::move(type_result.second);
            auto values_result = setup_token(name, type, number);
            if (values_result.first)
            {
               values = std::move(values_result.second);
               valid = true;
            }
         }
      }
   }

   std::size_t size() const { return place.second - place.first; }
   std::string full_name_type() const { return std::string("<").append(name.data(), name.size()).append(":").append(type.data(), type.size()).append(">"); }

   static std::string remplace_token(const boost::string_ref source, const source_token& token, std::size_t value_index)
   {
      return source.to_string().replace(source.find(token.full_name_type()), token.size() + 1, token.values[value_index]); //size() + 1 to remove the last '>'
   }

   static std::pair<std::size_t, std::size_t> parse_dotted(const boost::string_ref number)
   {
      std::pair<std::size_t, std::size_t> result;

      auto dot = number.find('.');
      result.first = boost::lexical_cast<std::size_t>(number.substr(0, dot));
      result.second = 0;
      if (dot != boost::string_ref::npos)
      {
         result.second = boost::lexical_cast<std::size_t>(number.substr(dot + 1, number.size()));
      }
      return result;
   }

   static std::string serialize_dotted(const std::pair<std::size_t, std::size_t>& dotted_num)
   {
      if (dotted_num.second)
         return boost::lexical_cast<std::string>(dotted_num.first) + '.' + boost::lexical_cast<std::string>(dotted_num.second);
      return boost::lexical_cast<std::string>(dotted_num.first);
   }

   template <std::size_t INTEGER_FORMAT_SIZE>
   static std::vector<std::string> image_token_integer(boost::string_ref number)
   {
      std::vector<std::string> values;
      for (std::size_t i = 0; i < IMAGE_MAX_NUM; ++i)
      {
         std::string value = boost::lexical_cast<std::string>(i);
         while (value.size() < INTEGER_FORMAT_SIZE)
            value = std::string("0") + value;
         values.emplace_back(value);
      }

      std::size_t i = 0;
      std::size_t size = values.size();
      while (i < size - 1)
      {
         values.push_back(values[i] + "-" + values[i + 1]);
         ++i;
      }
      std::sort(values.begin(), values.end());
      return values;
   }

   static std::pair<bool, std::vector<std::string>> setup_token(boost::string_ref token_name, boost::string_ref token_type, boost::string_ref number)
   {
      typedef std::map<
         /*underlying type*/  boost::string_ref,
         /*function ptr*/     std::vector<std::string>(*)(boost::string_ref)
      > string_func_ptr_map;
      static std::map< /*token name*/boost::string_ref, string_func_ptr_map > state_machine = 
      {
         { "number", //pair & token name
            { //string_func_ptr_map
               { "doted", [](boost::string_ref number) //pair & token type 
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
               { "integer", [](boost::string_ref number) //pair & token type 
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

   static std::pair<std::size_t, std::size_t> get_token_place(boost::string_ref source, boost::string_ref token_name)
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

   static std::pair<bool, boost::string_ref> get_type(boost::string_ref source, const std::pair<std::size_t, std::size_t>& token_place)
   {
      auto token = source.substr(token_place.first + 1, token_place.second - (token_place.first + 1));
      auto place = token.find(':');
      if (place != boost::string_ref::npos)
      {
         return{ true, token.substr(place + 1) };
      }
      return{ false,{} };
   }

};

#endif //!SOURCE_TOKEN_HPP