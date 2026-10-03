#ifndef AJSONUNION
#define AJSONUNION

#include <vector>
#include <unistd.h>
#include <variant>
#include "ajson/core/exceptions/exceptions.hpp"
#include "ajson/utilities/memory.hpp"

namespace ajson
{
    using null = std::monostate;
    class matrix;
    class object;
    class json;

    class value
    {
    public:
        enum class Id : unsigned char
        {
            __nullobj,
            __boolean,
            __integer,
            __longer,
            __double,
            __string,
            __matrix,
            __object
        };
    private:
        value::Id _M_id;
        static inline const char *_M_id_representation[8] = {
            "null",
            "boolean",
            "number",
            "number",
            "number",
            "string",
            "matrix",
            "object",
        };
        union _M_intern_union
        {
            null _M_null;
            bool _M_boolean;
            int _M_integer;
            long long _M_longer;
            double _M_doubles;
            std::basic_ptr<std::string> _M_string;
            std::basic_ptr<matrix> _M_matrix;
            std::basic_ptr<object> _M_object;
            _M_intern_union() { *(double*)(this) = 0; }
            ~_M_intern_union() {}
        } _M_representation;

    public:
        value() : _M_id(Id::__nullobj) {}
        value(const value &other) : _M_representation(), _M_id(other._M_id)
        {
            if (this != &other)
            {
                switch (other._M_id)
                {
                case Id::__string:
                    this->_M_representation._M_string = other._M_representation._M_string;
                    break;
                case Id::__matrix:
                    this->_M_representation._M_matrix = other._M_representation._M_matrix;
                    break;
                case Id::__object:
                    this->_M_representation._M_object = other._M_representation._M_object;
                    break;
                case Id::__nullobj:
                    this->_M_representation._M_null = other._M_representation._M_null;
                    break;
                case Id::__integer:
                    this->_M_representation._M_integer = other._M_representation._M_integer;
                    break;
                case Id::__longer:
                    this->_M_representation._M_longer = other._M_representation._M_longer;
                    break;
                case Id::__double:
                    this->_M_representation._M_doubles = other._M_representation._M_doubles;
                    break;
                case Id::__boolean:
                    this->_M_representation._M_boolean = other._M_representation._M_boolean;
                    break;
                }
            }
        }
        value(value &&other) : _M_representation(), _M_id(other._M_id)
        {
            if (this != &other)
            {
                switch (other._M_id)
                {
                case Id::__string:
                    this->_M_representation._M_string = std::move(other._M_representation._M_string);
                    break;
                case Id::__matrix:
                    this->_M_representation._M_matrix = std::move(other._M_representation._M_matrix);
                    break;
                case Id::__object:
                    this->_M_representation._M_object = std::move(other._M_representation._M_object);
                    break;
                case Id::__nullobj:
                    this->_M_representation._M_null = std::move(other._M_representation._M_null);
                    break;
                case Id::__integer:
                    this->_M_representation._M_integer = std::move(other._M_representation._M_integer);
                    break;
                case Id::__longer:
                    this->_M_representation._M_longer = std::move(other._M_representation._M_longer);
                    break;
                case Id::__double:
                    this->_M_representation._M_doubles = std::move(other._M_representation._M_doubles);
                    break;
                case Id::__boolean:
                    this->_M_representation._M_boolean = std::move(other._M_representation._M_boolean);
                    break;
                }
            }
        }

        value &operator=(const value &other)
        {
            if (this != &other)
            {
                if (this->_M_id == Id::__string)
                    this->_M_representation._M_string.~basic_ptr();
                else if (this->_M_id == Id::__matrix)
                    this->_M_representation._M_matrix.~basic_ptr();
                else if (this->_M_id == Id::__object)
                    this->_M_representation._M_object.~basic_ptr();

                this->_M_id = other._M_id;

                switch (other._M_id)
                {
                case Id::__string:
                    this->_M_representation._M_string = other._M_representation._M_string;
                    break;
                case Id::__matrix:
                    this->_M_representation._M_matrix = other._M_representation._M_matrix;
                    break;
                case Id::__object:
                    this->_M_representation._M_object = other._M_representation._M_object;
                    break;
                case Id::__nullobj:
                    this->_M_representation._M_null = other._M_representation._M_null;
                    break;
                case Id::__integer:
                    this->_M_representation._M_integer = other._M_representation._M_integer;
                    break;
                case Id::__longer:
                    this->_M_representation._M_longer = other._M_representation._M_longer;
                    break;
                case Id::__double:
                    this->_M_representation._M_doubles = other._M_representation._M_doubles;
                    break;
                case Id::__boolean:
                    this->_M_representation._M_boolean = other._M_representation._M_boolean;
                    break;
                }
            }
            return *this;
        }
        value &operator=(value &&other)
        {
            if (this != &other)
            {
                if (this->_M_id == Id::__string)
                    this->_M_representation._M_string.~basic_ptr();
                else if (this->_M_id == Id::__matrix)
                    this->_M_representation._M_matrix.~basic_ptr();
                else if (this->_M_id == Id::__object)
                    this->_M_representation._M_object.~basic_ptr();

                this->_M_id = other._M_id;

                switch (other._M_id)
                {
                case Id::__string:
                    this->_M_representation._M_string = std::move(other._M_representation._M_string);
                    break;
                case Id::__matrix:
                    this->_M_representation._M_matrix = std::move(other._M_representation._M_matrix);
                    break;
                case Id::__object:
                    this->_M_representation._M_object = std::move(other._M_representation._M_object);
                    break;
                case Id::__nullobj:
                    this->_M_representation._M_null = std::move(other._M_representation._M_null);
                    break;
                case Id::__integer:
                    this->_M_representation._M_integer = std::move(other._M_representation._M_integer);
                    break;
                case Id::__longer:
                    this->_M_representation._M_longer = std::move(other._M_representation._M_longer);
                    break;
                case Id::__double:
                    this->_M_representation._M_doubles = std::move(other._M_representation._M_doubles);
                    break;
                case Id::__boolean:
                    this->_M_representation._M_boolean = std::move(other._M_representation._M_boolean);
                    break;
                }
            }
            return *this;
        }

        value(ajson::null _value) : _M_id(Id::__nullobj), _M_representation() { _M_representation._M_null = _value; }
        value(bool _value) : _M_id(Id::__boolean), _M_representation()  { _M_representation._M_boolean = _value; }
        value(int _value) : _M_id(Id::__integer), _M_representation()  { _M_representation._M_integer = _value; }
        value(long long _value) : _M_id(Id::__longer), _M_representation() { _M_representation._M_longer = _value; }
        value(double _value) : _M_id(Id::__double), _M_representation() { _M_representation._M_doubles = _value; }
        value(const char *_value) : _M_id(Id::__string), _M_representation() { _M_representation._M_string = std::string(_value); }
        value(const std::string &_value) : _M_id(Id::__string), _M_representation() { _M_representation._M_string = _value; }
        value(const std::basic_ptr<ajson::matrix>& _value) : _M_id(Id::__matrix), _M_representation() { _M_representation._M_matrix = _value; }
        value(const std::basic_ptr<ajson::object>& _value) : _M_id(Id::__object), _M_representation() { _M_representation._M_object = _value; }

        template <typename T>
        T &get();

        friend std::ostream &operator<<(std::ostream &, const ajson::value &);

        template <typename T>
        void set(const T &_value);

        inline value::Id id() const noexcept { return _M_id;}

        ~value();

        friend json;
    };
    template <>
    ajson::null &value::get<ajson::null>()
    {
        if (_M_id != Id::__nullobj)
            throw ajson::critical_error("The type of data don't coinciden to type of allocator, type allocator: " + std::string(value::_M_id_representation[static_cast<int>(_M_id)]), 0);
        return _M_representation._M_null;
    }
    template <>
    bool &value::get<bool>()
    {
        if (_M_id != Id::__boolean)
            throw ajson::critical_error("The type of data don't coinciden to type of allocator, type allocator: " + std::string(value::_M_id_representation[static_cast<int>(_M_id)]), 0);
        return _M_representation._M_boolean;
    }
    template <>
    int &value::get<int>()
    {
        if (_M_id == Id::__integer)
            return _M_representation._M_integer;
        else if (_M_id == Id::__longer)
            return (int &)(_M_representation._M_longer);
        else if (_M_id == Id::__double)
            return (int &)(_M_representation._M_doubles);
        else
            throw ajson::critical_error("The type of data don't coinciden to type of allocator, type allocator: " + std::string(value::_M_id_representation[static_cast<int>(_M_id)]), 0);
    }
    template <>
    long long &value::get<long long>()
    {
        if (_M_id == Id::__integer)
            return (long long &)(_M_representation._M_integer);
        else if (_M_id == Id::__longer)
            return _M_representation._M_longer;
        else if (_M_id == Id::__double)
            return (long long &)(_M_representation._M_doubles);
        else
            throw ajson::critical_error("The type of data don't coinciden to type of allocator, type allocator: " + std::string(value::_M_id_representation[static_cast<int>(_M_id)]), 0);
    }
    template <>
    double &value::get<double>()
    {
        if (_M_id == Id::__integer)
            return (double &)(_M_representation._M_integer);
        else if (_M_id == Id::__longer)
            return (double &)_M_representation._M_longer;
        else if (_M_id == Id::__double)
            return _M_representation._M_doubles;
        else
            throw ajson::critical_error("The type of data don't coinciden to type of allocator, type allocator: " + std::string(value::_M_id_representation[static_cast<int>(_M_id)]), 0);
    }
    template <>
    std::string &value::get<std::string>()
    {
        if (_M_id != Id::__string)
            throw ajson::critical_error("The type of data don't coinciden to type of allocator, type allocator: " + std::string(value::_M_id_representation[static_cast<int>(_M_id)]), 0);
        return *_M_representation._M_string.get();
    }
    template <>
    ajson::matrix &value::get<ajson::matrix>()
    {
        if (_M_id != Id::__matrix)
            throw ajson::critical_error("The type of data don't coinciden to type of allocator, type allocator: " + std::string(value::_M_id_representation[static_cast<int>(_M_id)]), 0);
        return *_M_representation._M_matrix.get();
    }
    template <>
    ajson::object &value::get<ajson::object>()
    {
        if (_M_id != Id::__object)
            throw ajson::critical_error("The type of data don't coinciden to type of allocator, type allocator: " + std::string(value::_M_id_representation[static_cast<int>(_M_id)]), 0);
        return *_M_representation._M_object.get();
    }
    template <>
    void value::set<bool>(const bool &_value)
    {
        if (_M_id == Id::__string)
            _M_representation._M_string.~basic_ptr();
        else if (_M_id == Id::__matrix)
            _M_representation._M_matrix.~basic_ptr();
        else if (_M_id == Id::__object)
            _M_representation._M_object.~basic_ptr();
        _M_representation._M_boolean = _value;
        _M_id = Id::__boolean;
    }
    template <>
    void value::set<int>(const int &_value)
    {
        if (_M_id == Id::__string)
            _M_representation._M_string.~basic_ptr();
        else if (_M_id == Id::__matrix)
            _M_representation._M_matrix.~basic_ptr();
        else if (_M_id == Id::__object)
            _M_representation._M_object.~basic_ptr();
        _M_representation._M_integer = _value;
        _M_id = Id::__integer;
    }
    template <>
    void value::set<long long>(const long long &_value)
    {
        if (_M_id == Id::__string)
            _M_representation._M_string.~basic_ptr();
        else if (_M_id == Id::__matrix)
            _M_representation._M_matrix.~basic_ptr();
        else if (_M_id == Id::__object)
            _M_representation._M_object.~basic_ptr();
        _M_representation._M_longer = _value;
        _M_id = Id::__longer;
    }
    template <>
    void value::set<double>(const double &_value)
    {
        if (_M_id == Id::__string)
            _M_representation._M_string.~basic_ptr();
        else if (_M_id == Id::__matrix)
            _M_representation._M_matrix.~basic_ptr();
        else if (_M_id == Id::__object)
            _M_representation._M_object.~basic_ptr();
        _M_representation._M_doubles = _value;
        _M_id = Id::__double;
    }
    template <>
    void value::set<std::string>(const std::string &_value)
    {
        if (_M_id == Id::__string)
            _M_representation._M_string.~basic_ptr();
        else if (_M_id == Id::__matrix)
            _M_representation._M_matrix.~basic_ptr();
        else if (_M_id == Id::__object)
            _M_representation._M_object.~basic_ptr();
        _M_representation._M_string = _value;
        _M_id = Id::__string;
    }
    template <>
    void value::set<ajson::matrix>(const ajson::matrix &_value)
    {
        if (_M_id == Id::__string)
            _M_representation._M_string.~basic_ptr();
        else if (_M_id == Id::__matrix)
            _M_representation._M_matrix.~basic_ptr();
        else if (_M_id == Id::__object)
            _M_representation._M_object.~basic_ptr();
        _M_representation._M_matrix = std::basic_ptr<ajson::matrix>(_value);
        _M_id = Id::__matrix;
    }
    template <>
    void value::set<ajson::object>(const ajson::object &_value)
    {
        if (_M_id == Id::__string)
            _M_representation._M_string.~basic_ptr();
        else if (_M_id == Id::__matrix)
            _M_representation._M_matrix.~basic_ptr();
        else if (_M_id == Id::__object)
            _M_representation._M_object.~basic_ptr();
        _M_representation._M_object = std::basic_ptr<ajson::object>(_value);
        _M_id = Id::__object;
    }
    value::~value()
    {
        if (_M_id == Id::__string)
            _M_representation._M_string.~basic_ptr();
        else if (_M_id == Id::__matrix)
            _M_representation._M_matrix.~basic_ptr();
        else if (_M_id == Id::__object)
            _M_representation._M_object.~basic_ptr();
    }
} // namespace ajson

#endif