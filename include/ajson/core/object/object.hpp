#ifndef AJSONOBJECT
#define AJSONOBJECT

#include <map>
#include <string>
#include <variant>
#include "value.hpp"
#include <stdexcept>

namespace ajson
{
    class object
    {
    public:
        std::map<std::string, ajson::value> _M_values;

    public:
        object(const std::initializer_list<std::pair<std::string, ajson::value>> &list)
        {
            for (auto &&iterator : list)
                _M_values[iterator.first] = iterator.second;
        }
        template <typename T>
        void insert(const std::string &position, const T &_value) { _M_values.emplace(position, ajson::value(_value)); }
        template <typename T>
        void set(const std::string &position, const T &_value) { return _M_values[position].set<T>(_value); }
        template <typename T>
        T &get(const std::string &position) { return _M_values[position].get<T>(); }
        inline std::map<std::string, ajson::value>::iterator begin() noexcept { return _M_values.begin(); }
        inline std::map<std::string, ajson::value>::iterator end() noexcept { return _M_values.end(); }
        inline size_t size() const noexcept { return _M_values.size(); }
        inline bool empty() const noexcept { return _M_values.empty(); }
        void remove(const std::string &position) { _M_values.erase(position); }
        ~object() {}
    };
} // namespace ajson

#endif