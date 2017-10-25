#ifndef __SG_CORE_CONCATENATE_HPP__
#define __SG_CORE_CONCATENATE_HPP__

/*
* Implement this string utility function to help combine strings with/without integral/float value.
* By doing the benchmark, we found the bottleneck of the string concatenation is the malloc 
* operation when the string size is larger than the original capacity. So to improve string
* combination performance, we need to call reserve with the new string size first. The concatenate
* function will do this automatically for you, so you only need to pass the strings you want to 
* combine. This function will calculate new string size and then call reserve before combine
* operation. We don't implement this function with fluent api because this will dramatically slow
* down the performence when compile with MSVC on windows (I haven't been able to firgure out what's
* the reason for this issue. It seems because the fulent api prvent MSVC inlining the template 
* function and call them recursively. GCC doesn't have this issue).
*
* Besides strings combination, sometimes we also want to convert integral/float value to string and
* concatenate them with other strings. Without this function, we can use std::ostringstream or
* sprintf to achive this. But both of them have terrible performance. The following are the 
* benchmark results for three scenarios: combine strings/combine strings with integral value/
* combine strings with float value (Measure by running loop 100000 times).
* MSVC                                                  GCC
*       concatenate     ostringstream       snprintf    concatenate     ostringsteam    snprintf
* str   11.7s           109.69s             46.36s      39.08s          84.62s          56.02s
*                       x9                  x4                          x2              x1
* itoa  29.63s          339.64s             89.64s      62.86s          152.36s         66.15s
*                       x11                 x3                          x2              x1
* dtoa  62.62s          929.10s             672.50s     104.28s         420.84s         286.78s
*                       x15                 x11                         x4              x3
*
* By looking into this benchmark, we can see that concatenate improve the performance huge escipecilly
* when combine strings with integral/float values. This because we use some high performance itoa/dtoa
* versions which come from rapidjson library. You can get the detail benchmark info from
* https://github.com/miloyip/itoa-benchmark and https://github.com/miloyip/dtoa-benchmark
* 
*/

#include "Core.hpp"

#include <string>
#include <type_traits>
#include <cstdint>

#include "rapidjson/internal/itoa.h"
#include "rapidjson/internal/dtoa.h"

#include "Utils.hpp"

#define SG_MAX_FLOAT_BUF_SIZE   256

namespace sg
{
    int CORE_API GetConcatenateFloatPrecision();
    void CORE_API SetConcatenateFloatPrecision(int precision);

    namespace utils
    {
        inline char* itoa(uint32_t value, char *buffer)
        {
            return rapidjson::internal::u32toa(value, buffer);
        }

        inline char* itoa(int32_t value, char *buffer)
        {
            return rapidjson::internal::i32toa(value, buffer);
        }

        inline char* itoa(uint64_t value, char *buffer)
        {
            return rapidjson::internal::u64toa(value, buffer);
        }

        inline char* itoa(int64_t value, char *buffer)
        {
            return rapidjson::internal::i64toa(value, buffer);
        }

        template <typename T>
        struct string_size_impl;

        template <size_t N>
        struct string_size_impl<const char[N]>
        {
            static constexpr size_t size(const char(&)[N]) { return N - 1; }
        };

        template <size_t N>
        struct string_size_impl<char[N]>
        {
            static size_t size(char(&s)[N]) { return N ? std::strlen(s) : 0; }
        };

        template <>
        struct string_size_impl<const char*>
        {
            static size_t size(const char *s) { return s ? std::strlen(s) : 0; }
        };

        template <>
        struct string_size_impl<char*>
        {
            static size_t size(char *s) { return s ? std::strlen(s) : 0; }
        };

        template <>
        struct  string_size_impl<std::string>
        {
            static size_t size(std::string const& s) { return s.size(); }
        };

        template <typename String>
        size_t string_size(String&& s)
        {
            using noref_t = typename std::remove_reference<String>::type;
            using string_t = typename std::conditional<std::is_array<noref_t>::value,
                noref_t,
                typename std::remove_cv<noref_t>::type
            >::type;
            return string_size_impl<string_t>::size(s);
        }

        template <typename T, typename Placeholder = void>
        struct concatenate_traits
        {
            typedef T type;

            concatenate_traits(T&& t) : _ref(std::forward<T>(t)), _siz(string_size(_ref)) {}
            concatenate_traits(concatenate_traits<T> const&) = delete;

            concatenate_traits<T>& operator=(concatenate_traits<T> const&) = delete;
            concatenate_traits<T>& operator=(concatenate_traits<T> && t)
            {
                if (this != &t)
                {
                    _ref = std::forward<T>(t._ref);
                    _siz = t._siz;
                }
                return *this;
            }

            size_t size() const { return _siz; }
            std::string& concat(std::string & result) const { result.append(_ref); return result; }

            T&& _ref;
            size_t _siz;
        };

        template <typename T>
        struct concatenate_integer_traits
        {
            enum { kBufferSize = 12 };
        };

        template <>
        struct concatenate_integer_traits<uint64_t>
        {
            enum { kBufferSize = 21 };
        };

        template <>
        struct concatenate_integer_traits<int64_t>
        {
            enum { kBufferSize = 22 };
        };

        template <typename T>
        struct concatenate_traits<T, typename std::enable_if<std::is_integral< typename std::remove_reference<T>::type >::value>::type>
        {
            typedef T type;

            enum { kBufferSize = concatenate_integer_traits<T>::kBufferSize };

            concatenate_traits(T t) :_buf{ 0 }, _ref(t), _siz(0)
            {
                itoa(t, _buf);
                _siz = string_size(_buf);
            }

            concatenate_traits(concatenate_traits<T> const&) = delete;

            concatenate_traits<T>& operator=(concatenate_traits<T> const&) = delete;
            concatenate_traits<T>& operator=(concatenate_traits<T> && t)
            {
                if (this != &t)
                {
                    _ref = t._ref;
                    std::memcpy(_buf, t._ref, kBufferSize);
                    _siz = t._siz;
                }
                return *this;
            }

            size_t size() const { return _siz; }
            std::string& concat(std::string &result) const { result.append(_buf, _siz); return result; }

            char _buf[kBufferSize];
            T _ref;
            size_t _siz;
        };

        template <typename T>
        struct concatenate_traits<T, typename std::enable_if<std::is_floating_point< typename std::remove_reference<T>::type >::value>::type>
        {
            typedef T type;

            enum { kBufferSize = SG_MAX_FLOAT_BUF_SIZE };

            concatenate_traits(T t)
                : _buf{ 0 }, _ref(t), _siz(0)
            {
                SG_ASSERT(GetConcatenateFloatPrecision() <= (SG_MAX_FLOAT_BUF_SIZE - 3));
                char *buf = rapidjson::internal::dtoa(t, _buf, GetConcatenateFloatPrecision());
                *buf = 0;
                _siz = string_size(_buf);
            }
            concatenate_traits(concatenate_traits<T> const&) = delete;

            concatenate_traits<T>& operator=(concatenate_traits<T> const&) = delete;
            concatenate_traits<T>& operator=(concatenate_traits<T> && t)
            {
                if (this != &t)
                {
                    _ref = t._ref;
                    std::memcpy(_buf, t._ref, kBufferSize);
                }
                return *this;
            }

            size_t size() const { return _siz; }
            std::string& concat(std::string &result) const { result.append(_buf, _siz); return result; }

            char _buf[kBufferSize];
            T _ref;
            size_t _siz;
        };

        template <typename... T>
        struct concatenate_impl {};

        template <typename T>
        struct concatenate_impl<T>
        {
            static size_t size(T const& s) { return s.size(); }
            static void concat(std::string &result, T const& s) { s.concat(result); }
        };

        template <typename T, typename... Rest>
        struct concatenate_impl<T, Rest...>
        {
            static size_t size(T const& s, Rest const&... rest) { return s.size() + concatenate_impl<Rest...>::size(rest...); }
            static void concat(std::string &result, T const& s, Rest const&... rest) { s.concat(result); concatenate_impl<Rest...>::concat(result, rest...); }
        };

        template <typename... T>
        void concatenate_internal(std::string &result, T const&... s)
        {
            result.reserve(result.size() + concatenate_impl<T...>::size(s...));
            concatenate_impl<T...>::concat(result, s...);
        }
    }

    template <typename... T>
    void concatenate(std::string &result, T&&... t)
    {
        utils::concatenate_internal(result, utils::concatenate_traits<T>(std::forward<T>(t))...);
    }
}


#endif
