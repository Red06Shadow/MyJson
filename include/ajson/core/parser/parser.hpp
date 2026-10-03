// Version 1.0.0 beta
#ifndef PARSER
#define PARSER


#include <variant>
#include <iostream>
#include <map>
#include <vector>
#include <core/lexer/ylexer.hpp>
#include <core/object/object.hpp>
#include <core/object/matrix.hpp>
#include <core/object/value.hpp>

namespace ajson
{
    template <typename CharT>
    class parser
    {
    private:
    
    public:
        parser();
    };
    
    class json
    {
    private:
        
    public:
        json(/* args */) {}
        ~json() {}
    };
} // namespace ajson

#include "parser.tpp"
#endif