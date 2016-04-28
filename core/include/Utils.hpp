#ifndef __CORE_UTILS_HPP__
#define __CORE_UTILS_HPP__

#include "Core.hpp"

#define SG_UNREF_PARAM(x) (void)(x)

#define SG_SAFE_DELETE(p) if (p) { delete p; p = nullptr; }
#define SG_SAFE_DELETE_ARRAY(p) if (p) { delete[] p; p = nullptr; }
#define SG_SAFE_RELEASE(p) if(p) { (p)->release(); (p) = nullptr; }

#define SG_ARRY_SIZE(x)     (sizeof((x)) / sizeof((x)[0]))

#define MakeArraryPtr(T, N) std::shared_ptr<T>(new T[N], [](T *p){ SG_SAFE_DELETE_ARRAY(p); })

namespace sg
{
    template <typename T>
    T clamp(T const& x, T const& lower_bound, T const& upper_bound)
    {
        if (x < lower_bound) return lower_bound;
        if (x > upper_bound) return upper_bound;

        return x;
    }

    template <typename T>
    inline bool equal(T const& lhs, T const& rhs)
    {
        return (lhs == rhs);
    }

    template <>
    inline bool equal<float>(float const& lhs, float const& rhs)
    {
        return (std::abs<float>(lhs - rhs) <= std::numeric_limits<float>::epsilon());
    }

    template <>
    inline bool equal<double>(double const& lhs, double const& rhs)
    {
        return (std::abs<double>(lhs - rhs) <= std::numeric_limits<double>::epsilon());
    }

    template<typename T, typename... Args>
    inline std::shared_ptr<T> MakeSharedPtr(Args&& ... args)
    {
        return std::shared_ptr<T>(new T(std::forward<Args>(args)...), std::default_delete<T>());
    }

    inline void str2vec(std::string const& str, std::vector<char> &vec)
    {
        vec.reserve(str.size() + 1);
        vec.assign(str.begin(), str.end());
        vec.push_back('\0');
    }
}

#endif
