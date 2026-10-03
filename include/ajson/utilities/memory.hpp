#ifndef _GLIBCXX_MEMORY_ME
#define _GLIBCXX_MEMORY_ME

#include <iostream>
#include <stdexcept>
#include <vector>
#include <memory>

namespace std
{
    template <typename type>
    class basic_ptr
    {
    private:
        type *_M_ptr;

    public:
        basic_ptr() : _M_ptr(nullptr) {}
        basic_ptr(const type &value) : _M_ptr(new type(value)) {}
        basic_ptr(const std::basic_ptr<type> &other) : _M_ptr(nullptr)
        {
            if (!other._M_ptr)
                throw std::runtime_error("error: empty basic_ptr in copy contructor: \
                    basic_ptr(const std::basic_ptr& other)");
            this->_M_ptr = new type(*other._M_ptr);
        }
        basic_ptr(std::basic_ptr<type> &&other) noexcept : _M_ptr(other._M_ptr)
        {
            other._M_ptr = nullptr;
        }
        std::basic_ptr<type> &operator=(std::nullptr_t)
        {
            this->reset();
            return *this;
        }
        std::basic_ptr<type> &operator=(const type &other)
        {
            if (this->_M_ptr)
                delete this->_M_ptr;
            this->_M_ptr = new type(other);
            return *this;
        }
        std::basic_ptr<type> &operator=(const std::basic_ptr<type> &other)
        {
            if (&other != this)
            {
                if (!other._M_ptr)
                    throw std::runtime_error("error: empty basic_ptr in copy contructor: \
                        basic_ptr(const std::basic_ptr& other)");
                if (this->_M_ptr)
                    delete this->_M_ptr;
                this->_M_ptr = new type(*other._M_ptr);
            }
            return *this;
        }
        std::basic_ptr<type> &operator=(std::basic_ptr<type> &&other) noexcept
        {
            if (&other != this)
            {
                if (this->_M_ptr)
                    delete this->_M_ptr;
                this->_M_ptr = other._M_ptr;
                other._M_ptr = nullptr;
            }
            return *this;
        }
        _GLIBCXX23_CONSTEXPR type operator*(int)
            const noexcept(noexcept(*std::declval<type>()))
        {
            return *get();
        }

        _GLIBCXX23_CONSTEXPR type *operator->()
            const noexcept { return get(); }

        _GLIBCXX23_CONSTEXPR type *get() const noexcept
        {
            return _M_ptr;
        }

        void reset()
        {
            if (_M_ptr)
                delete _M_ptr;
            _M_ptr = nullptr;
        }
        ~basic_ptr()
        {
            if (_M_ptr)
                delete _M_ptr;
            _M_ptr = nullptr;
        }
    };
    template <typename type>
    class basic_allocator
    {
    public:
    private:
        type *_M_ptr;
        size_t _M_size;

    public:
        basic_allocator() : _M_ptr(nullptr), _M_size(0) {}
        basic_allocator(const std::initializer_list<type> &list);
        basic_allocator(const std::vector<type> &list);
        basic_allocator(size_t _size) : _M_size(_size), _M_ptr(new type[_size]) {}
        basic_allocator(const std::basic_allocator<type> &other) : _M_ptr(nullptr)
        {
            if (!other._M_ptr)
                throw std::runtime_error("error: empty basic_allocator in copy \
                    contructor: basic_allocator(const std::basic_allocator& other)");
            _M_ptr = new type[other._M_size];
            for (size_t i = 0; i < other._M_size; i++)
                _M_ptr[i] = other._M_ptr[i];
            _M_size = other._M_size;
        }

        basic_allocator(std::basic_allocator<type> &&other) noexcept : _M_ptr(other._M_ptr), _M_size(other._M_size)
        {
            other._M_ptr = nullptr;
        }

        std::basic_allocator<type> &operator=(const std::basic_allocator<type> &other)
        {
            if (&other != this)
            {
                if (!other._M_ptr)
                    throw std::runtime_error("error: empty basic_allocator in \
                        copy contructor: basic_allocator(const std::basic_allocator& other)");
                if (this->_M_ptr)
                    delete[] this->_M_ptr;
                this->_M_ptr = new type[other._M_size];
                for (size_t i = 0; i < other._M_size; i++)
                    this->_M_ptr[i] = other._M_ptr[i];
                this->_M_size = other._M_size;
            }
            return *this;
        }
        const type &operator[](size_t _p) const { return _M_ptr[_p]; }
        type &operator[](size_t _p) { return _M_ptr[_p]; }
        const type &at(size_t _p) const
        {
            if (_p >= _M_size)
                throw std::runtime_error("error: acceso too large position in: \
                    const type& std::basic_allocator<type>::at(size_t _p) const \
                    ; max: " + std::to_string(_M_size));
            return _M_ptr[_p];
        }
        type &at(size_t _p)
        {
            if (_p >= _M_size)
                throw std::runtime_error("error: acceso too large position in: \
                    const type& std::basic_allocator<type>::at(size_t _p) const \
                    ; max: " + std::to_string(_M_size));
            return _M_ptr[_p];
        }

        inline size_t
        size() const { return _M_size; }

        std::iterator_traits<type> begin() const { return std::iterator_traits<type>(_M_ptr); }
        std::iterator_traits<type> end() const { return std::iterator_traits<type>(_M_ptr + _M_size); }

        std::basic_allocator<type> &operator=(std::basic_allocator<type> &&other) noexcept
        {
            if (&other != this)
            {
                if (this->_M_ptr)
                    delete this->_M_ptr;
                this->_M_ptr = other._M_ptr;
                other._M_ptr = nullptr;
                _M_size = other._M_size;
            }
            return *this;
        }
        ~basic_allocator()
        {
            if (_M_ptr)
                delete[] _M_ptr;
            _M_ptr = nullptr;
            _M_size = 0;
        }
    };
    template <typename type>
    basic_allocator<type>::basic_allocator(const std::initializer_list<type> &list)
        : _M_ptr(nullptr), _M_size(0)
    {
        if (list.size() == 0)
            throw std::runtime_error("error: empty basic_allocator in contructor: \
                template <typename type> basic_allocator<type>::basic_allocator\
                (const std::initializer_list<type> &list)");
        _M_ptr = new type[list.size()];
        for (size_t i = 0; i < list.size(); i++)
            _M_ptr[i] = *(list.begin() + i);
        _M_size = list.size();
    }
    template <typename type>
    basic_allocator<type>::basic_allocator(const std::vector<type> &list)
        : _M_ptr(nullptr), _M_size(0)
    {
        if (list.size() == 0)
            throw std::runtime_error("error: empty basic_allocator in contructor: \
                template <typename type> basic_allocator<type>::basic_allocator\
                (const std::initializer_list<type> &list)");
        _M_ptr = new type[list.size()];
        for (size_t i = 0; i < list.size(); i++)
            _M_ptr[i] = list[i];
        _M_size = list.size();
    }
} // namespace std

#endif