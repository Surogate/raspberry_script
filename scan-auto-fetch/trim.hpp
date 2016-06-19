#ifndef TRIM_HPP
#define TRIM_HPP

#include <cctype>
#include "astring_view.hpp"

template<typename CHAR_T>
static bool is_trimmable(CHAR_T val)
{
   return std::iscntrl(val) || std::isspace(val);
}

template <typename CHAR_T>
static astd::basic_string_view<CHAR_T> trim(astd::basic_string_view<CHAR_T> str, bool (*functor)(CHAR_T) = &is_trimmable<CHAR_T>)
{
   std::size_t start_trim = 0;
   std::size_t end_trim = 0;
   auto beg = str.begin();
   auto end = str.end();

   for (; beg != end && (*functor)(*beg); ++beg)
   {
      start_trim++;
   }

   auto rbeg = str.rbegin();
   auto rend = str.rend();

   for (; rbeg != rend
      && (*functor)(*rbeg); ++rbeg)
   {
      end_trim++;
   }

   str.remove_prefix(start_trim);
   str.remove_suffix(end_trim);
   return str;
}

static astd::string_view trim(astd::string_view str)
{
   return trim<char>(str);
}



#endif //!TRIM_HPP