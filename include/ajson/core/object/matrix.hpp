#ifndef AJSONMATRIX
#define AJSONMATRIX

#include <vector>
#include "value.hpp"

namespace ajson
{
    class matrix
    {
    public:
        std::vector<ajson::value> _M_values;
    public:
        matrix(const std::initializer_list<ajson::value>& list) { for (auto &&_value : list) _M_values.push_back(_value); }
        template <typename T>
        void insert(size_t position, const T& _value) { _M_values.insert(_M_values.begin() + position, ajson::value(_value)); }
        template <typename T>
        void set(size_t position, const T& _value) { return _M_values[position].set<T>(_value); }
        template <typename T>
        T& get(size_t position) { return _M_values[position].get<T>(); }
        inline std::vector<ajson::value>::const_iterator begin() const noexcept { return _M_values.begin(); }
        inline std::vector<ajson::value>::const_iterator end() const noexcept { return _M_values.end(); }
        inline size_t size() const noexcept { return _M_values.size(); }
        inline bool empty() const noexcept { return _M_values.empty(); }
        void remove(size_t position) { _M_values.erase(_M_values.begin() + position); }
        ~matrix() {}
    };
} // namespace ajson

#endif