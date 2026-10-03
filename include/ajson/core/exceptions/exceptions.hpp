#ifndef AJSONEXCEPTIONS
#define AJSONEXCEPTIONS

#include <stdexcept>

namespace ajson
{
    class critical_error : std::logic_error
    {
    private:
        unsigned char _M_code;
    public:
        critical_error(const std::string& __message, unsigned char __code) : std::logic_error(__message), _M_code(__code) {}
        unsigned char code() const {return _M_code;}
        ~critical_error() {}
    };
} // namespace ajson


#endif