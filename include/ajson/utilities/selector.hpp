#ifndef MYREGEXSELECTOR
#define MYREGEXSELECTOR

#include <iostream>

namespace std
{
    template <typename T>
    struct selector;

    template <>
    struct selector<char>
    {
        using streamtype = std::ostream;
        static inline streamtype &stream() { return std::cout; }
    };

    template <>
    struct selector<wchar_t>
    {
        using streamtype = std::wostream;
        static inline streamtype &stream() { return std::wcout; }
    };
} // namespace std

#endif