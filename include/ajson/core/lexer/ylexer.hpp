#ifndef YOURSLEXER
#define YOURSLEXER
#include <iostream>
#include <corecrt.h>
#include <fstream>
#include <type_traits>
#include <stdexcept>
#include <string>
#include <vector>
#include <stdexcept>
#include <math.h>
#include <memory>
#include <filesystem>
#include <queue>

#ifndef MYREGEXSELECTOR
#define MYREGEXSELECTOR
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

namespace slexer
{
#ifndef BASICBUFFER
#define BASICBUFFER
    template <typename charT>
    class basic_buffer
    {
    private:
        charT *_M_allocator;
        uint32_t _M_capacity;
        bool _M_is_eof;
        uint32_t _M_queue;
        uint32_t _M_head;
        uint32_t _M_start;
        uint32_t _M_count;

        void _M_realloc();

    public:
        basic_buffer() : _M_allocator(nullptr), _M_capacity(0), _M_is_eof(false), _M_queue(0), _M_head(0), _M_count(0), _M_start(0) {}
        basic_buffer(uint32_t size) : _M_allocator(new charT[size]), _M_capacity(size), _M_is_eof(false), _M_queue(0), _M_head(0), _M_count(0), _M_start(size) {}
        basic_buffer(const std::basic_string<charT> &str) : _M_allocator(new charT[str.size()]), _M_capacity(str.size()), _M_is_eof(true), _M_queue(0), _M_head(0), _M_count(0), _M_start(0) { str.copy(_M_allocator, _M_capacity); }

        basic_buffer(const basic_buffer<charT> &other);
        basic_buffer(basic_buffer<charT> &&other) noexcept;
        basic_buffer<charT> &operator=(const basic_buffer<charT> &other);
        basic_buffer<charT> &operator=(basic_buffer<charT> &&other) noexcept;

        void push(charT c);
        charT peak();
        charT pop();
        void back(uint32_t n);
        friend std::basic_ifstream<charT> &operator>>(std::basic_ifstream<charT> &in, basic_buffer &buff)
        {
            if (buff._M_count == 0 && buff._M_queue == buff._M_start)
                buff._M_realloc();

            if (buff._M_start == buff._M_capacity)
            {
                in.read(buff._M_allocator, buff._M_capacity);
                buff._M_start = 0;
                buff._M_count = in.gcount();
            }
            else
            {
                if (buff._M_queue > buff._M_start)
                {
                    in.read(buff._M_allocator + buff._M_queue, buff._M_capacity - buff._M_queue);
                    buff._M_count += in.gcount();
                    if (!(buff._M_is_eof = in.eof()) && buff._M_start > 0)
                    {
                        in.read(buff._M_allocator, buff._M_start);
                        buff._M_count += in.gcount();
                    }
                }
                else
                {
                    in.read(buff._M_allocator + buff._M_queue, buff._M_start - buff._M_queue);
                    buff._M_count += in.gcount();
                }
            }
            buff._M_queue = buff._M_start;
            buff._M_is_eof = in.eof();
            in.clear();
            return in;
        }
        friend std::basic_ostream<charT> &operator<<(std::basic_ostream<charT> &out, const basic_buffer<charT> &_buffer)
        {
            out.write(_buffer._M_allocator, _buffer.capacity());
            return out;
        }
        inline void update() { _M_start = _M_head; }
        inline void copy(std::basic_string<charT> &, uint32_t, uint32_t);
        inline bool eof() const { return _M_is_eof; }
        inline bool end() const { return _M_is_eof && _M_count == 0; }
        inline bool empty() const { return _M_count == 0; }
        inline uint32_t size() const { return _M_count; }
        inline uint32_t capacity() const { return _M_capacity; }
        inline bool full() const { return _M_count == _M_capacity; }
        inline std::string_view to_string() const { return std::string_view(_M_allocator, _M_capacity); }
        ~basic_buffer();
    };

    template <typename charT>
    basic_buffer<charT>::basic_buffer(const basic_buffer<charT> &other) : _M_allocator(nullptr),
                                                                          _M_capacity(other._M_capacity),
                                                                          _M_is_eof(other._M_is_eof),
                                                                          _M_queue(other._M_queue),
                                                                          _M_head(other._M_head),
                                                                          _M_count(other._M_count),
                                                                          _M_start(other._M_start)
    {
        this->_M_allocator = new charT[other._M_capacity];
        for (size_t i = 0; i < other._M_capacity; i++)
            this->_M_allocator[i] = other._M_allocator[i];
    }

    template <typename charT>
    basic_buffer<charT>::basic_buffer(basic_buffer<charT> &&other) noexcept : _M_allocator(std::move(other._M_allocator)),
                                                                              _M_capacity(other._M_capacity),
                                                                              _M_is_eof(other._M_is_eof),
                                                                              _M_queue(other._M_queue),
                                                                              _M_head(other._M_head),
                                                                              _M_count(other._M_count),
                                                                              _M_start(other._M_start)
    {
        other._M_allocator = nullptr;
    }

    template <typename charT>
    basic_buffer<charT> &basic_buffer<charT>::operator=(const basic_buffer<charT> &other)
    {
        if (this != &other)
        {
            _M_capacity = other._M_capacity;
            _M_is_eof = other._M_is_eof;
            _M_queue = other._M_queue;
            _M_head = other._M_head;
            _M_count = other._M_count;
            _M_start = other._M_start;
            this->_M_allocator = new charT[other._M_capacity];
            for (size_t i = 0; i < other._M_capacity; i++)
                this->_M_allocator[i] = other._M_allocator[i];
        }
        return *this;
    }
    template <typename charT>
    basic_buffer<charT> &basic_buffer<charT>::operator=(basic_buffer<charT> &&other) noexcept
    {
        if (this != &other)
        {
            _M_capacity = other._M_capacity;
            _M_is_eof = other._M_is_eof;
            _M_queue = other._M_queue;
            _M_head = other._M_head;
            _M_count = other._M_count;
            _M_start = other._M_start;
            this->_M_allocator = other._M_allocator;
            other._M_allocator = nullptr;
        }
        return *this;
    }

    template <typename charT>
    void basic_buffer<charT>::push(charT c)
    {
        if (_M_count == _M_capacity)
            _M_realloc();
        _M_allocator[_M_queue] = c;
        _M_queue = (_M_queue + 1) % _M_capacity;
        _M_count++;
    }

    template <typename charT>
    void basic_buffer<charT>::_M_realloc()
    {
        uint32_t _new_capacity = _M_capacity * 2;
        charT *_new_allocator = new charT[_new_capacity];
        for (uint32_t i = 0; i < _M_capacity; i++)
        {
            _new_allocator[i] = _M_allocator[_M_queue];
            _M_queue = (_M_queue + 1) % _M_capacity;
        }
        _M_head = _M_queue = _M_capacity;
        _M_start = 0;
        _M_capacity = _new_capacity;
        delete _M_allocator;
        _M_allocator = _new_allocator;
    }

    template <typename charT>
    inline void basic_buffer<charT>::copy(std::basic_string<charT> &str, uint32_t start, uint32_t end)
    {
        while (start != end)
        {
            str.push_back(_M_allocator[start]);
            start = (start + 1) % _M_capacity;
        }
    }

    template <typename charT>
    charT basic_buffer<charT>::pop()
    {
        if (_M_count == 0)
            return -1;
        charT c = _M_allocator[_M_head];
        _M_head = (_M_head + 1) % _M_capacity;
        _M_count--;
        return c;
    }

    template <typename charT>
    void basic_buffer<charT>::back(uint32_t n)
    {
        if (_M_count + n > _M_capacity)
            n = _M_capacity - _M_count;
        _M_count += n;
        //////////////////////////////////////////
        if ((_M_head - n) > _M_capacity)
            _M_head = _M_capacity - (n - _M_head);
        else
            _M_head -= n;
    }

    template <typename charT>
    charT basic_buffer<charT>::peak()
    {
        if (_M_count == 0)
            return -1;
        return _M_allocator[_M_head];
    }

    template <typename charT>
    basic_buffer<charT>::~basic_buffer()
    {
        if (_M_allocator != nullptr)
        {
            delete[] _M_allocator;
            _M_allocator = nullptr;
        }
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef BASICSTACKBUFFER
#define BASICSTACKBUFFER
    template <typename charT>
    class basic_stackbuffer
    {
    private:
        struct _S_node
        {
            slexer::basic_buffer<charT> _M_buffer;
            std::shared_ptr<_S_node> _M_back;
            _S_node(size_t size, std::shared_ptr<_S_node> &&back) : _M_buffer(size), _M_back(std::move(back)) {}
            _S_node(const std::basic_string<charT> &str, std::shared_ptr<_S_node> &&back) : _M_buffer(str), _M_back(std::move(back)) {}
        };
        std::shared_ptr<_S_node> _M_iterator;
        size_t _M_size;

    public:
        basic_stackbuffer() : _M_iterator(), _M_size(0) {}
        void push(size_t size)
        {
            if (_M_iterator.get() == nullptr)
                _M_iterator = std::make_shared<_S_node>(_S_node(size, nullptr));
            else
                _M_iterator = std::make_shared<_S_node>(size, std::move(_M_iterator));
            _M_size++;
        }
        void push(const std::basic_string<charT> &str)
        {
            if (_M_iterator.get() == nullptr)
                _M_iterator = std::make_shared<_S_node>(_S_node(str, nullptr));
            else
                _M_iterator = std::make_shared<_S_node>(str, std::move(_M_iterator));
            _M_size++;
        }

        inline slexer::basic_buffer<charT> &top()
        {
            if (_M_size == 0)
                throw std::runtime_error("in: basic_stackbuffer::top(); this stack is empty.");
            return _M_iterator.get()->_M_buffer;
        }
        inline const slexer::basic_buffer<charT> &top() const
        {
            if (_M_size == 0)
                throw std::runtime_error("in: basic_stackbuffer::top(); this stack is empty.");
            return _M_iterator.get()->_M_buffer;
        }
        void pop()
        {
            if (_M_iterator.get() == nullptr)
                throw std::runtime_error("in: basic_stackbuffer::pop(); this stack is empty.");
            std::shared_ptr provitional = std::move(_M_iterator->_M_back);
            _M_iterator->_M_buffer.~basic_buffer();
            _M_iterator.reset();
            _M_iterator = std::move(provitional);
            _M_size--;
        }
        inline size_t size() const { return _M_size; }
        ~basic_stackbuffer()
        {
        }
    };
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
#ifndef BASICSLEXERTOKEN
#define BASICSLEXERTOKEN
    template <typename charT, typename idT>
    class basic_lexer;

    template <typename charT, typename idT>
    class basic_token
    {
    private:
        static_assert(std::is_unsigned_v<idT> | std::is_enum_v<idT>, "in basic_token: do you no have template with value is not a unsigned integer");
        std::basic_string<charT> _M_text;
        idT _M_id;
        size_t _M_line, _M_position, _M_column;
        inline void _M_set_localete(size_t _line, size_t _column, size_t _position) { _M_line =_line; _M_position = _position; _M_column = _column; }

    public:
        basic_token() : _M_text(), _M_id(), _M_line(0), _M_position(0), _M_column(0) {}
        basic_token(const std::basic_string<charT>& text, idT _id, size_t _position, size_t _line, size_t _column) : 
        _M_text(text), _M_id(_id), _M_line(_line), _M_position(_position), _M_column(_column) {}
        basic_token(std::basic_string<charT>&& text, size_t _id, size_t _position, size_t _line, size_t _column) : 
        _M_text(text), _M_id(_id), _M_line(_line), _M_position(_position), _M_column(_column) {}
        inline std::basic_string<charT>& text() { return _M_text; }
        inline const std::basic_string<charT>& text() const { return _M_text; }
        inline idT id() const { return _M_id; }
        inline idT& id() { return _M_id; }
        ///////////////////////////////////////////////
        inline size_t position() const { return _M_position; }
        inline size_t line() const { return _M_line; }
        inline size_t column() const { return _M_column; }
        inline size_t& position() { return _M_position; }
        inline size_t& line() { return _M_line; }
        inline size_t& column() { return _M_column; }
        void view() const { std::selector<charT>::stream() << charT('{') << std::endl << charT('\t') << "str: "<< _M_text << std::endl << charT('\t') << "id: "<< _M_id << std::endl << charT('\t') << "pos: "<< _M_position << std::endl << charT('\t') << "line: "<< _M_line << std::endl << charT('\t') << "column: "<< _M_column << std::endl << charT('}') << std::endl; }
        ~basic_token() {}
        friend basic_lexer<charT, idT>;
    };

    template <typename idT>
    using token = basic_token<char, idT>;
    template <typename idT>
    using wtoken = basic_token<wchar_t, idT>;
#endif
} // namespace slexer
#ifndef BASICSSTRINGRANGE
#define BASICSSTRINGRANGE
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
        const typename std::__cxx11::basic_string<CharT>::const_iterator &it,
        size_t offset = 0) const { return (it + offset) - __begin; }
    ~basic_string_range() {}
};

using stringrange = basic_string_range<char>;
/////////////////////////////////////////////////////////////////////
using wstringrange = basic_string_range<wchar_t>;
#endif
namespace slexer
{
#ifndef BASICSLEXERERROR
#define BASICSLEXERERROR
    class critical_error : public std::logic_error
    {
    private:
        unsigned int _M_code;

    public:
        critical_error(uint32_t __code, const std::string &__message)
            : std::logic_error("critical error in lexer: " +
                               __message + " code: " +
                               std::to_string(__code)),
              _M_code(__code) {}
        inline unsigned char code() const { return _M_code; }
        ~critical_error() {}
    };

    template <typename charT, typename idT>
    class basic_lexical_error : public std::exception
    {
    public:
        static std::basic_string<charT> _S_syntax_error_viewer(const basic_string_range<charT> &, size_t, size_t);
        typedef std::basic_string<charT> (*handle)(uint16_t, const std::basic_string<char> &, std::basic_istream<charT> &, const slexer::basic_token<charT, idT> &);

    private:
        uint16_t _M_code;
        std::basic_string<char> _M_message;
        slexer::basic_token<charT, idT> _M_capture;
        handle _M_handle;

    public:
        basic_lexical_error(uint16_t, const std::basic_string<char> &, const slexer::basic_token<charT, idT> &);
        basic_lexical_error(uint16_t, const std::basic_string<char> &, const slexer::basic_token<charT, idT> &, basic_lexical_error<charT, idT>::handle);
        inline uint16_t code() const { return _M_code; }
        inline const std::string &message() const _GLIBCXX_NOTHROW { return _M_message; }
        inline std::basic_string<charT> especification(std::basic_istream<charT> &in) const _GLIBCXX_NOTHROW { return _M_handle(_M_code, _M_message, in, _M_capture); }
        inline const slexer::basic_token<charT, idT> &capture() const { return _M_capture; }
        inline const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override { return _M_message.c_str(); }
        ~basic_lexical_error() {}
    };
    template <typename charT, typename idT>
    basic_lexical_error<charT, idT>::basic_lexical_error(uint16_t _code, const std::basic_string<char> &__message, const slexer::basic_token<charT, idT> &__capture) : std::exception(), _M_code(_code), _M_message(__message), _M_capture(__capture), _M_handle(nullptr) {}
    template <typename charT, typename idT>
    basic_lexical_error<charT, idT>::basic_lexical_error(uint16_t _code, const std::basic_string<char> &__message, const slexer::basic_token<charT, idT> &__capture, basic_lexical_error<charT, idT>::handle __handle) : std::exception(), _M_code(_code), _M_message(__message), _M_capture(__capture), _M_handle(__handle) {}
    /// @brief Funcion de captura por defecto
    template <typename charT, typename idT>
    inline constexpr typename basic_lexical_error<charT, idT>::handle default_hanlde_excpetion = [](uint16_t _code, const std::basic_string<char> &_message, std::basic_istream<charT> &in, const slexer::basic_token<charT, idT> &tokn) -> std::basic_string<charT>
    {
        in.seekg(tokn.position() - tokn.column());
        std::basic_string<charT> line;
        std::getline(in, line);
        basic_string_range<charT> range{line};
        std::basic_string<charT> especification = "in [line: " + std::to_string(tokn.line()) + ", column: " + std::to_string(tokn.column()) + ", position: " + std::to_string(tokn.position()) + "]: " + tokn.text() + "\n\t" + line + "\n\t" + basic_lexical_error<charT, idT>::_S_syntax_error_viewer(range, tokn.column(), tokn.column() + (tokn.text().size() - 1));
        return especification;
    };

    template <typename charT, typename idT>
    std::basic_string<charT> basic_lexical_error<charT, idT>::_S_syntax_error_viewer(const basic_string_range<charT> &expresion, size_t index_a, size_t index_b)
    {
        std::basic_string<charT> generate_spaces_and_mark = {};
        typename basic_string_range<charT>::iterator minrange;
        typename basic_string_range<charT>::iterator maxrange;

        if (expresion.begin() == expresion.end())
            return {};

        minrange = expresion.begin() + index_a;
        maxrange = expresion.begin() + index_b;

        for (typename basic_string_range<charT>::iterator i = expresion.begin(); i != expresion.end(); i++)
        {
            if (*i == charT('\n'))
                generate_spaces_and_mark.push_back(charT('\n'));
            else if (minrange <= i && i <= maxrange)
                generate_spaces_and_mark.push_back(charT('~'));
            else
                generate_spaces_and_mark.push_back(charT(' '));
        }
        if constexpr (std::is_same_v<charT, char>)
            return generate_spaces_and_mark;
        else
            return generate_spaces_and_mark;
    }
#endif
} // namespace slexer

namespace slexer
{
#ifndef BASICSLEXERTOKENSTREAM
#define BASICSLEXERTOKENSTREAM
    template <typename charT, typename idT>
    class basic_tokenstream
    {
    private:
        static_assert(std::is_unsigned_v<idT> | std::is_enum_v<idT>, "in basic_tokenstream: do you no have template with value is not a unsigned integer");
        std::vector<slexer::basic_token<charT, idT>> _M_tokens;
        typename std::vector<slexer::basic_token<charT, idT>>::const_iterator _M_iterator;

    public:
        using const_iterator = typename std::vector<slexer::basic_token<charT, idT>>::const_iterator;

    public:
        basic_tokenstream() : _M_tokens(), _M_iterator(nullptr) {}
        inline void push(const slexer::basic_token<charT, idT> &token) { _M_tokens.push_back(token); }
        inline const slexer::basic_token<charT, idT> &back() const { return _M_tokens.back(); }
        inline basic_tokenstream::const_iterator begin() const { return _M_tokens.begin(); }
        inline basic_tokenstream::const_iterator end() const { return _M_tokens.end(); }

        inline void reset() { _M_iterator = _M_tokens.begin(); }
        inline basic_tokenstream::const_iterator peak() const { return _M_iterator; }
        inline basic_tokenstream::const_iterator next() { return _M_iterator++; }
        inline basic_tokenstream::const_iterator undo() { return _M_iterator--; }
        inline void localete(size_t position) { _M_iterator = _M_tokens.begin() + position; }
        bool isend() const { return _M_iterator.base() == nullptr || _M_iterator >= _M_tokens.end(); }

        inline const slexer::basic_token<charT, idT> &operator[](size_t pos) const { return _M_tokens[pos]; }
        inline size_t size() const { return _M_tokens.size(); }
        inline bool empty() const { return _M_tokens.empty(); }
        ~basic_tokenstream() {}
    };

    template <typename idT>
    using tokenstream = basic_tokenstream<char, idT>;
    template <typename idT>
    using wtokenstream = basic_tokenstream<wchar_t, idT>;
#endif
} // namespace slexer

namespace std
{
#ifndef _GLIBCXX_MEMORY_ME
#define _GLIBCXX_MEMORY_ME
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
            if (other._M_ptr)
                this->_M_ptr = new type(*other._M_ptr);
        }
        basic_ptr(std::basic_ptr<type> &&other) noexcept : _M_ptr(other._M_ptr) { other._M_ptr = nullptr; }
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
                if (this->_M_ptr)
                    delete this->_M_ptr;
                if (other._M_ptr)
                    this->_M_ptr = new type(*other._M_ptr);
                else
                    this->_M_ptr = nullptr;
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
        explicit basic_allocator(size_t size) : _M_size(size), _M_ptr(new type[size]) {}
        basic_allocator(const std::basic_allocator<type> &other) : _M_ptr(nullptr), _M_size(0)
        {
            if (other._M_ptr && other._M_size > 0)
            {
                _M_ptr = new type[other._M_size];
                std::copy(other._M_ptr, other._M_ptr + other._M_size, _M_ptr);
                _M_size = other._M_size;
            }
        }

        basic_allocator(std::basic_allocator<type> &&other) noexcept
            : _M_ptr(other._M_ptr), _M_size(other._M_size)
        {
            other._M_ptr = nullptr;
            other._M_size = 0;
        }

        std::basic_allocator<type> &operator=(const std::basic_allocator<type> &other)
        {
            if (this != &other)
            {
                if (_M_ptr)
                {
                    delete[] _M_ptr;
                    _M_ptr = nullptr;
                    _M_size = 0;
                }

                if (other._M_ptr && other._M_size > 0)
                {
                    _M_ptr = new type[other._M_size];
                    std::copy(other._M_ptr, other._M_ptr + other._M_size, _M_ptr);
                    _M_size = other._M_size;
                }
            }
            return *this;
        }
        std::basic_allocator<type> &operator=(std::basic_allocator<type> &&other)
        {
            if (this != &other)
            {
                if (_M_ptr)
                {
                    delete[] _M_ptr;
                    _M_ptr = nullptr;
                    _M_size = 0;
                }
                _M_ptr = other._M_ptr;
                _M_size = other._M_size;
                other._M_ptr = nullptr;
                other._M_size = 0;
            }
            return *this;
        }
        type &operator[](size_t index) { return _M_ptr[index]; }
        const type &operator[](size_t index) const { return _M_ptr[index]; }
        type &at(size_t index)
        {
            if (index >= _M_size)
                throw std::out_of_range("index out of range: " + std::to_string(index));
            return _M_ptr[index];
        }

        const type &at(size_t index) const
        {
            if (index >= _M_size)
                throw std::out_of_range("index out of range: " + std::to_string(index));
            return _M_ptr[index];
        }

        type *begin() { return _M_ptr; }
        const type *begin() const { return _M_ptr; }
        type *end() { return _M_ptr + _M_size; }
        const type *end() const { return _M_ptr + _M_size; }

        // Capacity
        size_t size() const { return _M_size; }
        bool empty() const { return _M_size == 0; }

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
        if (list.size())
        {
            _M_ptr = new type[list.size()];
            std::copy(list.begin(), list.end(), _M_ptr);
            _M_size = list.size();
        }
    }
    template <typename type>
    basic_allocator<type>::basic_allocator(const std::vector<type> &list)
        : _M_ptr(nullptr), _M_size(0)
    {
        if (!list.empty())
        {
            _M_ptr = new type[list.size()];
            std::copy(list.begin(), list.end(), _M_ptr);
            _M_size = list.size();
        }
    }
#endif
} // namespace std

namespace myregex
{
#ifndef BASICSLEXERSTATEREGEX
#define BASICSLEXERSTATEREGEX
    template <typename idT>
    class state
    {
    private:
        bool _M_valid;
        idT _M_id;

    public:
        state() : _M_valid(false), _M_id() {}
        state(idT id) : _M_valid(true), _M_id(id) {}
        inline const idT &get() const { return _M_id; }
        inline bool valid() const { return _M_valid; }
    };
#endif
} // namespace myregex

#define DEBUG false

namespace myregex
{
#ifndef BASICSLEXERTABLE
#define BASICSLEXERTABLE
    template <typename charT, typename idT>
    class basic_builder;

    template <typename charT, typename idT>
    class basic_table
    {
        static_assert(std::is_same_v<charT, char> || std::is_same_v<charT, wchar_t>, "Error: no se permiten tipos de datos que no sean de caracteres(solo char o wchar_t)");

    public:
        using Qtable = std::vector<myregex::state<idT>>;
        using Transitions = std::basic_allocator<size_t>;

    private:
        Qtable Q_table;
        Transitions Q_transitions;

    public:
        basic_table() : Q_transitions(), Q_table() {}
        basic_table(const Qtable &_states, const Transitions &_transitions) : Q_transitions(_transitions), Q_table(_states) {}
        basic_table(Qtable &&_states, Transitions &&_transitions) : Q_transitions(std::move(_transitions)), Q_table(std::move(_states)) {}
        basic_table(const std::vector<myregex::state<idT>> &status) : Q_transitions(status.size() * myregex::basic_table<charT, idT>::dictionary), Q_table(status) {}
        basic_table(std::vector<myregex::state<idT>> &&status) : Q_transitions(status.size() * myregex::basic_table<charT, idT>::dictionary), Q_table(std::move(status)) {}
        basic_table(const basic_table &other) : Q_transitions(other.Q_transitions), Q_table(other.Q_table) {}
        basic_table(basic_table &&other) : Q_transitions(std::move(other.Q_transitions)), Q_table(std::move(other.Q_table)) {}
        basic_table &operator=(const basic_table &);
        basic_table &operator=(basic_table &&);
        inline const Qtable &status() const { return Q_table; }
        inline const Transitions &transitions() const { return Q_transitions; }
        inline static constexpr size_t dictionary = std::pow(256ULL, sizeof(charT));
        inline size_t size() const { return Q_table.size() * myregex::basic_table<charT, idT>::dictionary * sizeof(size_t) + sizeof(Transitions); }
        friend std::basic_ostream<charT> &operator<<(std::basic_ostream<charT> &out, basic_table<charT, idT> other)
        {
            out << charT('{');
            for (size_t state = 0; state < other.Q_table.size(); state++)
            {
                if (other.Q_table[state].valid())
                {
                    if constexpr (std::is_enum_v<idT>)
                        out << static_cast<size_t>(other.Q_table[state].get());
                    else
                        out << other.Q_table[state].get();
                }
                else
                    out << charT('{') << charT('}');
                out << charT((state >= other.Q_table.size() - 1ULL) ? '}' : ',');
            }
            out << charT(',') << std::endl
                << charT('{');
            for (size_t state = 0; state < other.Q_table.size(); state++)
            {
                for (size_t letter = 0; letter < myregex::basic_table<charT, idT>::dictionary; letter++)
                {
                    if constexpr (std::is_same_v<charT, char>)
                        out << (long long)(other.Q_transitions[(state * myregex::basic_table<charT, idT>::dictionary) + letter]) << "ULL" << charT(',');
                    else
                        out << (long long)(other.Q_transitions[(state * myregex::basic_table<charT, idT>::dictionary) + letter]) << L"ULL" << charT(',');
                }
                out << std::endl;
            }
            out << charT('}');
            return out;
        }
        ~basic_table();
        friend basic_builder<charT, idT>;
    };
    template <typename idT>
    using CompatibleTable = basic_table<char, idT>;
    template <typename idT>
    using table = basic_table<char, idT>;
    /////////////////////////////////////////////////////////////////////
    template <typename idT>
    using UnicodeTable = basic_table<wchar_t, idT>;
    template <typename idT>
    using wtable = basic_table<wchar_t, idT>;

    template <typename charT, typename idT>
    myregex::basic_table<charT, idT> &myregex::basic_table<charT, idT>::operator=(const basic_table &other)
    {
        if (&other != this)
        {
            Q_transitions = other.Q_transitions;
            Q_table = other.Q_table;
        }
        return *this;
    }
    template <typename charT, typename idT>
    myregex::basic_table<charT, idT> &myregex::basic_table<charT, idT>::operator=(basic_table &&other)
    {
        if (&other != this)
        {
            Q_transitions = std::move(other.Q_transitions);
            Q_table = std::move(other.Q_table);
        }
        return *this;
    }
    template <typename charT, typename idT>
    myregex::basic_table<charT, idT>::~basic_table() {}

#endif
} // namespace myregex

namespace slexer
{
    #ifndef BASICSLEXER
    #define BASICSLEXER
    enum class Flags : unsigned int
    {
        _S_emit_error = 0x0001,
        _S_emit_token = 0x0002,
        _S_push_capture = 0x0004,
    };
    _GLIBCXX_NODISCARD _GLIBCXX_CONSTEXPR inline Flags
    operator&(Flags __a, Flags __b) _GLIBCXX_NOTHROW { return Flags(static_cast<unsigned int>(__a) & static_cast<unsigned int>(__b)); }
    _GLIBCXX_NODISCARD _GLIBCXX_CONSTEXPR inline Flags
    operator|(Flags __a, Flags __b) _GLIBCXX_NOTHROW { return Flags(static_cast<unsigned int>(__a) | static_cast<unsigned int>(__b)); }
    _GLIBCXX_NODISCARD _GLIBCXX_CONSTEXPR inline Flags
    operator^(Flags __a, Flags __b) _GLIBCXX_NOTHROW { return Flags(static_cast<unsigned int>(__a) ^ static_cast<unsigned int>(__b)); }
    _GLIBCXX14_CONSTEXPR
    inline const Flags &
    operator&=(Flags &__a, Flags __b) _GLIBCXX_NOTHROW { return __a = __a & __b; }
    _GLIBCXX14_CONSTEXPR
    inline const Flags &
    operator|=(Flags &__a, Flags __b) _GLIBCXX_NOTHROW { return __a = __a | __b; }
    _GLIBCXX14_CONSTEXPR
    inline const Flags &
    operator^=(Flags &__a, Flags __b) _GLIBCXX_NOTHROW { return __a = __a ^ __b; }

    template <typename charT, typename idT>
    class basic_lexer
    {
    public:
        static_assert(std::is_unsigned_v<idT> | std::is_enum_v<idT>, "in basic_lexer: do you no have template with value is not a unsigned integer");
        class master
        {
        private:
            /// @brief Contador de posicion de archivo
            inline void _M_manager_position(charT);

        private:
            /// @brief Inicio de tabla
            size_t _M_begin;
            /// @brief Tamano del grupo de tablas
            size_t _M_gsize;
            /// @brief Captura
            slexer::basic_token<charT, idT> _M_capture;
            /// @brief Queue
            std::queue<slexer::basic_token<charT, idT>> _M_queue;
            /// @brief Cadena de tokens
            slexer::basic_tokenstream<charT, idT> &_M_tokenstream;
            /// @brief Captura de posicion
            size_t _M_capture_position;
            /// @brief Captura de linea
            size_t _M_capture_line;
            /// @brief Captura de columna
            size_t _M_capture_column;

        public:
            /// @brief Pila de Buffers
            slexer::basic_stackbuffer<charT> stackbuffer;

        public:
            /// @brief Constructor por defecto
            /// @param _gsize Tamano del grupo de tablas
            /// @param _tokenstream Cadena de tokens
            /// @param _buffer Buffer
            master(size_t _gsize, slexer::basic_tokenstream<charT, idT> &_tokenstream, size_t size)
                : _M_gsize(_gsize), _M_begin(0), _M_tokenstream(_tokenstream), stackbuffer(), _M_capture_position(0), _M_capture_line(0), _M_capture_column(0)
            {
                _M_capture._M_text.reserve(size);
            }
            /// @brief Restuarua el inicio a la tabla principal
            inline void begin() { _M_begin = 0; }
            /// @brief Cambia el inicio a la posicion de la tabla deseada
            /// @param _begin Posicion de inicio
            inline void begin(size_t _begin)
            {
                if (_begin >= _M_gsize)
                    throw slexer::critical_error(1, "no access to grupo over-size to table");
                _M_begin = _begin;
            }
            /// @brief Emite el token a la cadena de tokens
            inline void emit() { _M_tokenstream.push(_M_capture); }
            /// @brief Emite el token con un numero menor de caracteres a la cadena de tokens
            /// @param _less Numero de reduccion de caractres
            inline void emit(uint32_t _less)
            {
                if (_less > _M_capture.text().size())
                    _less = _M_capture.text().size();
                _M_capture.text().resize(_M_capture.text().size() - _less);
                stackbuffer.top().back(_less);
                _M_tokenstream.push(_M_capture);
            }
            /// @brief Cambia el token con una cadena modificada
            /// @param _str Cadena a insertar
            inline void change(const std::basic_string<char> &_str)
            {
                _M_capture._M_text = _str;
                _M_tokenstream.push(_M_capture);
            }
            /// @brief Cambia el token con una cadena e id modificada
            /// @param _str Cadena a insertar
            /// @param _id Id deseado
            inline void change(const std::basic_string<char> &_str, idT _id)
            {
                _M_capture._M_text = _str;
                _M_capture._M_id = _id;
                _M_tokenstream.push(_M_capture);
            }
            /// @brief Vacia la cola de capturas pendientes
            inline void pull()
            {
                while (!_M_queue.empty())
                {
                    _M_tokenstream.push(std::move(_M_queue.front()));
                    _M_queue.pop();
                }
            }
            /// @brief Vacia un numero especifico de capturas de la cola de pendientes
            /// @param _elements Cantidad de elementos a vaciar
            inline void pull(size_t _elements)
            {
                while (!_M_queue.empty() && _elements < _M_queue.size())
                {
                    _M_tokenstream.push(std::move(_M_queue.front()));
                    _M_queue.pop();
                }
            }
            /// @brief Obtiene la cola
            /// @return Cola de elementos de capturas
            inline std::queue<slexer::basic_token<charT, idT>> &queue() { return _M_queue; }
            /// @brief Obtiene la captura constante
            /// @return captura del lexer
            inline const slexer::basic_token<charT, idT> &capture() const { return _M_capture; }
            friend basic_lexer;
        };

        /// @brief Patron de la funcion de captura
        typedef void (*handle)(slexer::basic_lexer<charT, idT>::master &);

/// @brief Funcion de captura por defecto
#define defaultf(_charT, _idT) [](slexer::basic_lexer<_charT, _idT>::master &m) -> void { m.emit(); }
/// @brief Hanlde de definicion rapida
#define slexerf(_charT, _idT, _nmaster) [](slexer::basic_lexer<_charT, _idT>::master &_nmaster)->void
    public:
        class _I_idT
        {
        private:
            idT _M_id;
            handle _M_handle;

        public:
            _I_idT() : _M_id(), _M_handle(nullptr) {}
            _I_idT(idT _id, handle _handle) : _M_id(_id), _M_handle(_handle) {}
            inline idT id() const { return _M_id; }
            inline handle funt() const { return _M_handle; }
            friend bool operator<(const _I_idT &lhs, const _I_idT &rhs) { return lhs._M_id < rhs._M_id; }
            friend std::ostream &operator<<(std::ostream &out, const _I_idT &element) { return out; }
            ~_I_idT() {}
        };

    private:
        /// @brief Tablas de expreseiones
        std::basic_allocator<myregex::basic_table<charT, _I_idT>> _M_group_tables;

    private: /// @brief Funciones privadass
        /// @brief Captura de token por fichero
        template <typename Funtion>
        bool _M_caption(master &, basic_lexer::handle &, Funtion);
        constexpr inline static size_t _S_value(charT caracter)
        {
            size_t value;
            if constexpr (std::is_same_v<charT, char>)
                return (uint8_t)(caracter);
            else
            {
                if constexpr (sizeof(wchar_t) == 2)
                    return (uint16_t)(caracter);
                else
                    return (uint32_t)(caracter);
            }
        }

    public:
        /// @brief Constructor base del lexer
        /// @param group Grupo de tablas de expresiones regulares
        /// @param size_buffer tamano del buffer
        basic_lexer(std::basic_allocator<myregex::basic_table<charT, _I_idT>> &&group)
            : _M_group_tables(std::move(group)) {}
        basic_lexer(const basic_lexer<charT, idT> &) = delete;

        /// @brief Constructor de movimiento base
        /// @param other otro objecto basic_lexer
        basic_lexer(basic_lexer<charT, idT> &&other)
            : _M_group_tables(std::move(other._M_group_tables)) {}

        basic_lexer<charT, idT> &operator=(const basic_lexer<charT, idT> &) = delete;
        basic_lexer<charT, idT> &operator=(basic_lexer<charT, idT> &&);
        slexer::basic_tokenstream<charT, idT> tokenize(std::basic_ifstream<charT> &_M_stream, size_t buffersize = 4096);
        slexer::basic_tokenstream<charT, idT> tokenize(const std::basic_string<charT> &str);
        size_t size() const
        {
            size_t size_ = 0;
            for (size_t i = 0; i < _M_group_tables.size(); i++)
                size_ = _M_group_tables[i].size();
            return size_;
        }

        void view()
        {
            for (size_t i = 0; i < _M_group_tables.size(); i++)
                std::selector<charT>::stream() << _M_group_tables[i];
        }

        void view(size_t i) { std::selector<charT>::stream() << _M_group_tables[i]; }
    };

    template <typename charT, typename idT>
    basic_lexer<charT, idT> &basic_lexer<charT, idT>::operator=(basic_lexer<charT, idT> &&other)
    {
        if (this != &other)
            _M_group_tables = std::move(other._M_group_tables);
        return *this;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename charT, typename idT>
    void basic_lexer<charT, idT>::master::_M_manager_position(charT letter)
    {
        this->_M_capture_column++;
        if (letter == charT('\n'))
        {
            this->_M_capture_column = 0;
            this->_M_capture_line++;
        }
        this->_M_capture_position++;
    }

    template <typename charT, typename idT>
    template <typename Funtion>
    bool basic_lexer<charT, idT>::_M_caption(master &_M_master, basic_lexer<charT, idT>::handle &_M_handle, Funtion excecute)
    {
        size_t status = 0ULL;
        charT letter;
        std::basic_string<charT> str{};
        size_t max = 0ULL;
        size_t count = 0ULL;
        size_t accept_status = -1ULL;
        _I_idT reference;
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        if (_M_group_tables[_M_master._M_begin].status()[status].valid())
        {
            accept_status = status;
            reference = _M_group_tables[_M_master._M_begin].status()[status].get();
        }
        _M_master._M_capture._M_set_localete(
            _M_master._M_capture_line,
            _M_master._M_capture_column,
            _M_master._M_capture_position);
        ///////////////////////////////////////////////////////////////////////////////////////////////////
        while (!_M_master.stackbuffer.top().end())
        {
            letter = _M_master.stackbuffer.top().peak();
            size_t next = _M_group_tables[_M_master._M_begin].transitions()[status * myregex::basic_table<charT, _I_idT>::dictionary + _S_value(letter)];
            ///////////////////////////////////////////////////////////////////////////////////////////////////
            if (next == -1ULL)
                break; // No hay transición, rechazar
            status = next;
            ///////////////////////////////////////////////////////////////////////////////////////////////////
            _M_master._M_manager_position(letter);
            ///////////////////////////////////////////////////////////////////////////////////////////////////
            str.push_back(_M_master.stackbuffer.top().pop());
            count++;
            ///////////////////////////////////////////////////////////////////////////////////////////////////
            if (_M_group_tables[_M_master._M_begin].status()[status].valid())
            {
                accept_status = status;
                reference = _M_group_tables[_M_master._M_begin].status()[status].get();
                max = str.size();
                count = 0ULL;
            }
            excecute();
        }
        if (count != 0)
            _M_master.stackbuffer.top().back(count);
        if (accept_status != -1ULL)
        {
            _M_master._M_capture._M_text = str.substr(0, max);
            _M_master._M_capture._M_id = reference.id();
            _M_handle = reference.funt();
        }
        return accept_status != -1ULL;
    }
    template <typename charT, typename idT>
    slexer::basic_tokenstream<charT, idT> basic_lexer<charT, idT>::tokenize(std::basic_ifstream<charT> &_M_stream, size_t buffersize)
    {
        slexer::basic_tokenstream<charT, idT> _M_basic_tokenstream;
        slexer::basic_lexer<charT, idT>::master _M_master = slexer::basic_lexer<charT, idT>::master(
            _M_group_tables.size(),
            _M_basic_tokenstream,
            buffersize);
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        _M_master.stackbuffer.push(buffersize);
        _M_stream >> _M_master.stackbuffer.top();
        auto v = []() -> void {};
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        while (!_M_master.stackbuffer.top().end())
        {
            handle _M_handle = nullptr;
            try
            {
                if (!basic_lexer::_M_caption(_M_master, _M_handle, [&_M_stream, &_M_master]() -> void
                                             {
            ///////////////////////////////////////////////////////////////////////////////////////////////////
                                                if (_M_master.stackbuffer.top().empty() && !_M_master.stackbuffer.top().eof())
                                                    _M_stream >> _M_master.stackbuffer.top(); }))
                    throw slexer::basic_lexical_error<charT, idT>(0, "unesxpect token", _M_master._M_capture, default_hanlde_excpetion<charT, idT>);
                if (_M_handle == nullptr)
                    continue;
                _M_handle(_M_master);
            }
            catch (const slexer::basic_lexical_error<charT, idT> &e)
            {
                size_t before = _M_stream.tellg();
                std::cerr << e.what() << std::endl;
                std::selector<charT>::stream() << e.especification(_M_stream) << std::endl;
                _M_master.stackbuffer.top().pop();
                _M_stream.seekg(before);
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        _M_master.stackbuffer.pop();
        return _M_basic_tokenstream;
    }

    template <typename charT, typename idT>
    slexer::basic_tokenstream<charT, idT> basic_lexer<charT, idT>::tokenize(const std::basic_string<charT> &str)
    {
        slexer::basic_tokenstream<charT, idT> _M_basic_tokenstream;
        slexer::basic_lexer<charT, idT>::master _M_master = slexer::basic_lexer<charT, idT>::master(
            _M_group_tables.size(),
            _M_basic_tokenstream,
            str.size());
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        _M_master.stackbuffer.push(str);
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        while (!_M_master.stackbuffer.top().end())
        {
            handle _M_handle = nullptr;
            try
            {
                if (!basic_lexer::_M_caption(_M_master, _M_handle, []() -> void {}))
                    throw slexer::basic_lexical_error<charT, idT>(0, "unesxpect token", _M_master._M_capture, default_hanlde_excpetion<charT, idT>);
                if (_M_handle == nullptr)
                    continue;
                _M_handle(_M_master);
            }
            catch (const slexer::basic_lexical_error<charT, idT> &e)
            {
                // std::cerr << e.what() << std::endl;
                // std::selector<charT>::stream() << e.especification(_M_stream) << std::endl;
                // _M_sbuffer_input.top().pop();
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        _M_master.stackbuffer.pop();
        return _M_basic_tokenstream;
    }
    template <typename idT>
    using lexer = basic_lexer<char, idT>;
    template <typename idT>
    using wlexer = basic_lexer<wchar_t, idT>;
    #endif
} // namespace slexer#pragma yourcode
//your code
enum class lexerid : unsigned char
{
    _S_none,
    _S_string,
    _S_number,
    _S_boolean,
    _S_null,
    _S_openarray,
    _S_openobject,
    _S_closearray,
    _S_closeobject,
    _S_separator,
    _S_separator_object
};
#pragma yourcode
//your code
extern slexer::basic_lexer<char,lexerid> ylexer;
#endif
