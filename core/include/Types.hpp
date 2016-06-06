#ifndef __CORE_TYPES_HPP__
#define __CORE_TYPES_HPP__

#include <limits>
#include <complex>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include "boost/assert.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/numeric/conversion/cast.hpp"

namespace sg
{
    typedef unsigned int                            uint;
    typedef unsigned long                           ulong;

#ifdef SG_COMPILER_MSVC
    typedef ulong                                   cwctype;
#elif defined(SG_COMPILER_GCC)
#ifdef SG_CPU_X64
    typedef uint                                    cwctype;
#else
    typedef ulong                                   cwctype;
#endif // SG_CPU_X64
#endif

    struct uint8
    {
        uint8_t value;

        uint8(): value() {}
        uint8(uint8_t const& v) : value(v) {}
        uint8(uint8 const& arg) : value(arg.value) {}

        operator uint8_t() const { return value; }

        friend std::istream& operator>> (std::istream & in, uint8 & value)
        {
            std::string _v;
            in >> _v;

            // boost::numeric_cast won't raise exception for negative number, so we use
            // signed value first to cover this
            int _t;
            if (boost::conversion::try_lexical_convert(_v, _t))
            {
                value = boost::numeric_cast<uint8_t>(_t); // exception if _t is negative value or out of range
            }
            else
            {
                unsigned int _ut = boost::lexical_cast<unsigned int>(_v); // exception if failed
                value = boost::numeric_cast<uint8_t>(_ut); // exception if out of range
            }

            return in;
        }

        friend std::ostream& operator<< (std::ostream & out, uint8 const& value)
        {
            out << static_cast<unsigned int>(value.value); // static_cast is enough
            return out;
        }

    };

    struct int8
    {
        int8_t value;

        int8(): value() {}
        int8(int8_t const& v) : value(v) {}
        int8(int8 const& arg) : value(arg.value) {}

        operator int8_t() const { return value; }

        friend std::istream& operator>> (std::istream & in, int8 & value)
        {
            std::string _v;
            in >> _v;

            int _t = boost::lexical_cast<int>(_v);
            value = boost::numeric_cast<int8_t>(_t);

            return in;
        }

        friend std::ostream& operator<< (std::ostream & out, int8 const& value)
        {
            out << static_cast<int>(value.value);
            return out;
        }
    };

    struct uint16
    {
        uint16_t value;

        uint16() : value() {}
        uint16(uint16_t const& v) : value(v) {}
        uint16(uint16 const& arg) : value(arg.value) {}

        operator uint16_t() const { return value; }

        friend std::istream& operator>> (std::istream & in, uint16 & value)
        {
            std::string _v;
            in >> _v;

            int _t;
            if (boost::conversion::try_lexical_convert(_v, _t))
            {
                value = boost::numeric_cast<uint16_t>(_t);
            }
            else
            {
                unsigned int _ut = boost::lexical_cast<unsigned int>(_v);
                value = boost::numeric_cast<uint16_t>(_ut);
            }

            return in;
        }

        friend std::ostream& operator<< (std::ostream & out, uint16 const& value)
        {
            out << static_cast<unsigned int>(value.value);
            return out;
        }
    };

    struct int16
    {
        int16_t value;

        int16(): value() {}
        int16(int16_t const& v) : value(v) {}
        int16(int16 const& arg) : value(arg.value) {}

        operator int16_t() const { return value; }

        friend std::istream& operator>> (std::istream & in, int16 & value)
        {
            std::string _v;
            in >> _v;

            int _t = boost::lexical_cast<int>(_v);
            value = boost::numeric_cast<int16_t>(_t);

            return in;
        }

        friend std::ostream& operator<< (std::ostream & out, int16 const& value)
        {
            out << static_cast<int>(value.value);
            return out;
        }
    };
    
    struct uint32
    {
        uint32_t value;

        uint32() : value() {}
        uint32(uint32_t const& v) : value(v) {}
        uint32(uint32 const& arg) : value(arg.value) {}

        operator uint32_t() const { return value; }

        friend std::istream& operator>> (std::istream & in, uint32 & value)
        {
            std::string _v;
            in >> _v;

            int _t;
            if (boost::conversion::try_lexical_convert(_v, _t))
            {
                value = boost::numeric_cast<uint32_t>(_t);
            }
            else
            {
                unsigned int _ut = boost::lexical_cast<unsigned int>(_v);
                value = boost::numeric_cast<uint32_t>(_ut);
            }

            return in;
        }

        friend std::ostream& operator<< (std::ostream & out, uint32 const& value)
        {
            out << static_cast<unsigned int>(value.value);
            return out;
        }
    };

    struct int32
    {
        int32_t value;

        int32(): value() {}
        int32(int32_t const& v) : value(v) {}
        int32(int32 const& arg) : value(arg.value) {}

        operator int32_t() const { return value; }

        friend std::istream& operator>> (std::istream & in, int32 & value)
        {
            std::string _v;
            in >> _v;

            int _t = boost::lexical_cast<int>(_v);
            value = boost::numeric_cast<int32_t>(_t);

            return in;
        }

        friend std::ostream& operator<< (std::ostream & out, int32 const& value)
        {
            out << static_cast<int>(value.value);
            return out;
        }
    };


    class Window;
    class ConsoleWindow;
    class ConsoleWindowStream;
    class ConsoleStdColorStream;


    typedef std::shared_ptr<Window>                 WindowPtr;
    typedef std::shared_ptr<ConsoleWindow>          ConsoleWindowPtr;
    typedef std::shared_ptr<ConsoleWindowStream>    ConsoleWindowStreamPtr;
    typedef std::shared_ptr<ConsoleStdColorStream>  ConsoleStdColorStreamPtr;
}

#endif
