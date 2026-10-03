#ifndef AJSON
#define AJSON

#include <iostream>
#include "core/object/matrix.hpp"
#include "core/object/object.hpp"
#include "core/object/value.hpp"
#include "core/lexer/ylexer.hpp"

namespace ajson
{

    std::ostream &operator<<(std::ostream &out, const ajson::value &other)
    {
        switch (other.id())
        {
        case ajson::value::Id::__nullobj:
            out << "null";
            break;
        case ajson::value::Id::__boolean:
            out << std::boolalpha << const_cast<ajson::value &>(other).get<bool>();
            break;
        case ajson::value::Id::__integer:
            out << const_cast<ajson::value &>(other).get<int>();
            break;
        case ajson::value::Id::__longer:
            out << const_cast<ajson::value &>(other).get<long long>();
            break;
        case ajson::value::Id::__double:
            out << const_cast<ajson::value &>(other).get<double>();
            break;
        case ajson::value::Id::__string:
            out << '"' << const_cast<ajson::value &>(other).get<std::string>() << '"';
            break;
        case ajson::value::Id::__matrix:
        {
            auto iterator = const_cast<ajson::value &>(other).get<ajson::matrix>().begin();
            auto end = const_cast<ajson::value &>(other).get<ajson::matrix>().end();
            out << "[ ";
            while (iterator != end)
            {
                out << *iterator << (iterator + 1 == end ? " " : ", ");
                iterator++;
            }
            out << ']';
            break;
        }
        case ajson::value::Id::__object:
            auto iterator = const_cast<ajson::value &>(other).get<ajson::object>().begin();
            auto end = const_cast<ajson::value &>(other).get<ajson::object>().end();
            out << '{' << std::endl;
            while (iterator != end)
            {
                out << '\"' << iterator->first << "\" : " << iterator->second;
                iterator++;
                if (iterator != end)
                    out << ',';
                out << std::endl;
            }
            out << '}';
            break;
        }
        return out;
    }

    class json
    {
    private:
        ajson::matrix _M_value;
        json(ajson::matrix &&matrix) : _M_value(std::move(matrix)) {}
        static ajson::matrix _M_parser_matrix(slexer::tokenstream<lexerid> &);
        static ajson::object _M_parser_object(slexer::tokenstream<lexerid> &);
        static ajson::value _M_parser_value(slexer::tokenstream<lexerid> &);

    public:
        static json parser(const std::string &);
        static json parser(std::ifstream&);
        json(const std::initializer_list<ajson::value> &list) : _M_value(list) {}
        template <typename T>
        T &get()
        {
            if (_M_value.size() != 1)
                throw ajson::critical_error("this is't a one element matrix", 1);
            return _M_value.get<T>(0);
        }
        template <typename T>
        T &get(size_t position) { return _M_value.get<T>(position); }

        template <typename T>
        void set(const T &_value)
        {
            if (_M_value.size() != 1)
                throw ajson::critical_error("this is't a one element matrix", 1);
            return _M_value.set<T>(0, _value);
        }
        template <typename T>
        void set(size_t position, const T &_value) { return _M_value.set<T>(position, _value); }

        template <typename T>
        void insert(size_t position, const T &_value) { return _M_value.insert<T>(position, _value); }

        inline size_t size() const { return _M_value.size(); }
        inline bool empty() const { return _M_value.empty(); }

        friend std::ostream &operator<<(std::ostream &out, const json &other)
        {
            if (other._M_value.size() == 1)
                out << *other._M_value.begin();
            else
            {
                out << '[' << std::endl;
                for (auto &&value : other._M_value)
                    out << value << std::endl;
                out << ']';
            }
            return out;
        }
        ~json() {}
    };

    json json::parser(const std::string &str)
    {
        slexer::tokenstream<lexerid> tokens = ylexer.tokenize(str);
        if (tokens.empty())
            return json({});
        lexerid id = tokens.next()->id();
        if (id != lexerid::_S_openarray && id != lexerid::_S_openobject)
            throw ajson::critical_error("parser error: no puedes inicializar el arc.hivo con un valor diferente al de un arreglo o un objecto", 1);
        if (id == lexerid::_S_openarray)
            return json(std::move(json::_M_parser_matrix(tokens)));
        return json({ajson::value(std::move(json::_M_parser_object(tokens)))});
    }

    json json::parser(std::ifstream &stream)
    {
        slexer::tokenstream<lexerid> tokens = ylexer.tokenize(stream);
        tokens.reset();
        if (tokens.empty())
            return json({});
        lexerid id = tokens.next()->id();
        if (id != lexerid::_S_openarray && id != lexerid::_S_openobject)
            throw ajson::critical_error("parser error: no puedes inicializar el arc.hivo con un valor diferente al de un arreglo o un objecto", 1);
        if (id == lexerid::_S_openarray)
            return json(std::move(json::_M_parser_matrix(tokens)));
        return json({ajson::value(std::move(json::_M_parser_object(tokens)))});
    }

    ajson::matrix json::_M_parser_matrix(slexer::tokenstream<lexerid> &tokens)
    {
        ajson::matrix m{};
        lexerid valueid = lexerid::_S_none;
        while (true)
        {
            if (tokens.end() == tokens.peak())
                throw ajson::critical_error("parser error: No-end matrix.", 2);

            valueid = tokens.peak()->id();
            if (valueid >= lexerid::_S_string && valueid <= lexerid::_S_openobject)
                m.insert(m.size(), json::_M_parser_value(tokens));

            //////////////////////////////////////////////////////////////////////////////////
            if (tokens.end() == tokens.peak())
                throw ajson::critical_error("parser error: Inespered end.", 10);
            //////////////////////////////////////////////////////////////////////////////////
            if (tokens.peak()->id() == lexerid::_S_closearray)
                return std::move(m);
            if (tokens.peak()->id() == lexerid::_S_separator)
            {
                tokens.next();
                continue;
            }
            throw ajson::critical_error("parser error: Unespexct symbol." + tokens.peak()->text(), 3);
        }
    }
    ajson::object json::_M_parser_object(slexer::tokenstream<lexerid> &tokens)
    {
        ajson::object o{};
        lexerid valueid = lexerid::_S_none;
        while (true)
        {
            if (tokens.end() == tokens.peak())
                throw ajson::critical_error("parser error: No-end object.", 4);
            //////////////////////////////////////////////////////////////////////////////////
            if (tokens.peak()->id() != lexerid::_S_string)
                throw ajson::critical_error("parser error: key need to initialize object.", 5);
            std::string key = tokens.next()->text();
            key.pop_back();
            key.erase(key.begin());
            if(key.empty())
                throw ajson::critical_error("parser error: kunvalided key.", 9);
            //////////////////////////////////////////////////////////////////////////////////
            if (tokens.end() == tokens.peak())
                throw ajson::critical_error("parser error: Inespered end.", 10);
            //////////////////////////////////////////////////////////////////////////////////
            if (tokens.next()->id() != lexerid::_S_separator_object)
                throw ajson::critical_error("parser error: value on object require ':'", 6);
            valueid = tokens.peak()->id();
            //////////////////////////////////////////////////////////////////////////////////
            if (tokens.end() == tokens.peak())
                throw ajson::critical_error("parser error: Inespered end.", 10);
            //////////////////////////////////////////////////////////////////////////////////
            if (valueid >= lexerid::_S_string && valueid <= lexerid::_S_openobject)
                o.insert(key, json::_M_parser_value(tokens));
            //////////////////////////////////////////////////////////////////////////////////
            if (tokens.end() == tokens.peak())
                throw ajson::critical_error("parser error: Inespered end.", 10);
            //////////////////////////////////////////////////////////////////////////////////
            if (tokens.peak()->id() == lexerid::_S_closeobject)
                return std::move(o);
            if (tokens.peak()->id() == lexerid::_S_separator)
            {
                tokens.next();
                continue;
            }
            throw ajson::critical_error("parser error: Unespexct symbol." + tokens.peak()->text(), 3);
        }
    }
    ajson::value json::_M_parser_value(slexer::tokenstream<lexerid> &tokens)
    {
        ajson::value val;
        if (tokens.end() == tokens.peak())
            throw ajson::critical_error("parser error: No-value inserted.", 7);
        if (tokens.peak()->id() == lexerid::_S_null)
            val = ajson::value();
        else if (tokens.peak()->id() == lexerid::_S_number)
            val = ajson::value(std::strtod(tokens.peak()->text().c_str(), nullptr));
        else if (tokens.peak()->id() == lexerid::_S_boolean)
            val = ajson::value(tokens.peak()->text() == "true");
        else if (tokens.peak()->id() == lexerid::_S_string)
        {
            std::string lt = std::move(tokens.peak()->text());
            lt.pop_back();
            lt.erase(lt.begin());
            val = ajson::value(std::move(lt));
        }
        else if (tokens.peak()->id() == lexerid::_S_openarray)
        {
            tokens.next();
            val = ajson::value(std::move(json::_M_parser_matrix(tokens)));
        }
        else if (tokens.peak()->id() == lexerid::_S_openobject)
        {
            tokens.next();
            val = ajson::value(std::move(json::_M_parser_object(tokens)));
        }
        else
            throw ajson::critical_error("parser error: Unknow symbol", 8);
        tokens.next();
        return val;
    }
} // namespace ajson

#endif