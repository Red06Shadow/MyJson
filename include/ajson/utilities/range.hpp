#ifndef ITERATORS
#define ITERATORS

#include <string>

template <typename CharT>
class basic_string_range
{
public: // struts
    using iterator = typename std::__cxx11::basic_string<CharT>::const_iterator;

private: // Data
    const typename std::__cxx11::basic_string<CharT>::const_iterator __begin;
    typename std::__cxx11::basic_string<CharT>::const_iterator __iterator;
    const typename std::__cxx11::basic_string<CharT>::const_iterator __end;

public: // Funtions
    basic_string_range(const std::__cxx11::basic_string<CharT> &str) : __begin(str.begin()), __iterator(__begin), __end(str.end()) {}
    basic_string_range(const CharT *begin, size_t size) : __begin(begin), __iterator(begin), __end(begin + size) {}
    inline const typename std::__cxx11::basic_string<CharT>::const_iterator &
    begin() const { return __begin; }
    inline const typename std::__cxx11::basic_string<CharT>::const_iterator 
    next() { return __iterator++; }
    inline const typename std::__cxx11::basic_string<CharT>::const_iterator &
    peak() const { return __iterator; }
    inline const typename std::__cxx11::basic_string<CharT>::const_iterator 
    offset(size_t position) const { return __iterator + position; }
    inline const typename std::__cxx11::basic_string<CharT>::const_iterator &
    end() const { return __end; }
    inline size_t position(
        const typename std::__cxx11::basic_string<CharT>::const_iterator& it,
        size_t offset = 0
    ) const { return (it + offset) - __begin; }
    ~basic_string_range() {}
};

using stringrange = basic_string_range<char>;
/////////////////////////////////////////////////////////////////////
using wstringrange = basic_string_range<wchar_t>;

#endif