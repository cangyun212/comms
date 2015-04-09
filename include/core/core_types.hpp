#ifndef __CORE_TYPES_HPP__
#define __CORE_TYPES_HPP__

#ifndef CORE_CXX11_CORE_NULLPTR_SUPPORT
const class nullptr_t
{
public:
    template <typename T>
    operator T*() const
    {
        return reinterpret_cast<T*>(0);
    }

    template <typename C, typename T>
    operator T C::*() const
    {
        return reinterpret_cast<T C::*>(0);
    }

private:
    void operator&() const;
} nullptr = {};

#endif


#ifdef CORE_CXX11_CORE_STATIC_ASSERT_SUPPORT
    #define CORE_STATIC_ASSERT(x) static_assert(x, #x)
#else
    #include <boost/static_assert.hpp>
    #define CORE_STATIC_ASSERT(x) BOOST_STATIC_ASSERT(x)
#endif

#ifdef CORE_CXX11_CORE_DECLTYPE_SUPPORT
    #define CORE_AUTO(var, expr) auto var = expr
    #define CORE_DECLTYPE(expr) sg::remove_reference<decltype(expr)>::type
#else
    #include <boost/typeof/typeof.hpp>
    #define CORE_AUTO(var, expr) BOOST_AUTO(var, expr)
    #define CORE_DECLTYPE(expr) BOOST_TYPEOF(expr)
#endif

#ifdef CORE_CXX11_CORE_FOREACH_SUPPORT
    #define CORE_FOREACH(var, col) for (var : col)
#else
    #include <boost/foreach.hpp>
    #define CORE_FOREACH(var, col) BOOST_FOREACH(var, col)
#endif

#ifdef CORE_CXX11_CORE_OVERRIDE_SUPPORT
    #define CORE_OVERRIDE override
    #define CORE_FINAL final
#else
    #define CORE_OVERRIDE
    #define CORE_FINAL
#endif

#ifdef CORE_CXX11_CORE_RVALUE_REFERENCES_SUPPORT
    #include <utility>
    namespace sg
    {
        using std::move;
    }
#else
    #include <boost/move/move.hpp>
    namespace sg
    {
        using boost::move;
    }
#endif

#ifdef CORE_CXX11_LIBRARY_ARRAY_SUPPORT
    #include <array>
    namespace sg
    {
        using std::array;
    }
#else
    #include <boost/array.hpp>
    namespace sg
    {
        using boost::array;
    }
#endif

#ifdef CORE_CXX11_LIBRARY_CHRONO_SUPPORT
    #include <chrono>
    namespace sg
    {
        namespace chrono = std::chrono;
        using std::nano;
        using std::micro;
        using std::milli;
    }
#else
    #include <boost/chrono.hpp>
    namespace sg
    {
        namespace chrono = boost::chrono;
        using boost::nano;
        using boost::micro;
        using boost::milli;
    }
#endif


#ifdef CORE_CXX11_LIBRARY_CSTDINT_SUPPORT
    #include <cstdint>
    namespace sg
    {
        using std::uint64_t;
        using std::uint32_t;
        using std::uint16_t;
        using std::uint8_t;
        using std::int64_t;
        using std::int32_t;
        using std::int16_t;
        using std::int8_t;
    }
#else
    #include <boost/cstdint.hpp>
    namespace sg
    {
        using boost::uint64_t;
        using boost::uint32_t;
        using boost::uint16_t;
        using boost::uint8_t;
        using boost::int64_t;
        using boost::int32_t;
        using boost::int16_t;
        using boost::int8_t;
    }
#endif

#ifdef CORE_CXX11_LIBRARY_SMART_PTR_SUPPORT
    #include <functional>
    #include <memory>
    namespace sg
    {
        using std::bind;
        using std::function;
        namespace placeholders
        {
            using std::placeholders::_1;
            using std::placeholders::_2;
            using std::placeholders::_3;
            using std::placeholders::_4;
            using std::placeholders::_5;
            using std::placeholders::_6;
            using std::placeholders::_7;
            using std::placeholders::_8;
            using std::placeholders::_9;
        }

        using std::shared_ptr;
        using std::weak_ptr;
        using std::enable_shared_from_this;
        using std::static_pointer_cast;
        using std::dynamic_pointer_cast;
        using std::ref;
        using std::cref;
    }

#ifdef CORE_CXX11_LIBRARY_MEM_FN_SUPPORT
    namespace sg
    {
        using std::mem_fn;
    }
#else
    #include <boost/mem_fn.hpp>
    namespace sg
    {
        using boost::mem_fn;
    }
#endif
#else
    #include <boost/mem_fn.hpp>
    #include <boost/bind.hpp>
    #include <boost/function.hpp>
    #include <boost/ref.hpp>
    #include <boost/smart_ptr.hpp>
    namespace sg
    {
        using boost::bind;
        using boost::mem_fn;
        using boost::function;
        namespace placeholders
        {
            static boost::arg<1> _1;
            static boost::arg<2> _2;
            static boost::arg<3> _3;
            static boost::arg<4> _4;
            static boost::arg<5> _5;
            static boost::arg<6> _6;
            static boost::arg<7> _7;
            static boost::arg<8> _8;
            static boost::arg<9> _9;
        }

        using boost::shared_ptr;
        using boost::weak_ptr;
        using boost::enable_shared_from_this;
        using boost::static_pointer_cast;
        using boost::dynamic_pointer_cast;
        using boost::ref;
        using boost::cref;
    }
#endif

#ifdef CORE_CXX11_LIBRARY_TYPE_TRAITS_SUPPORT
    #include <type_traits>
    namespace sg
    {
        using std::is_same;
        using std::remove_reference;
        using std::is_pod;
    }
#else
    #include <boost/type_traits.hpp>
    namespace sg
    {
        using boost::is_same;
        using boost::remove_reference;
        using boost::is_pod;
    }
#endif


#ifdef CORE_CXX11_LIBRARY_UNORDERED_SUPPORT
    #include <unordered_map>
    #include <unordered_set>
    namespace sg
    {
        using std::unordered_map;
        using std::unordered_multimap;
        using std::unordered_set;
        using std::unordered_multiset;
    }
#else
    #include <boost/unordered_map.hpp>
    #include <boost/unordered_set.hpp>
    namespace sg
    {
        using boost::unordered_map;
        using boost::unordered_multimap;
        using boost::unordered_set;
        using boost::unordered_multiset;
    }
#endif

#ifdef CORE_CXX11_LIBRARY_ATOMIC_SUPPORT
    #include <atomic>
    namespace sg
    {
        using std::atomic;

        using std::atomic_thread_fence;
        using std::atomic_signal_fence;

        using std::memory_order_relaxed;
        using std::memory_order_release;
        using std::memory_order_acquire;
        using std::memory_order_consume;
        using std::memory_order_acq_rel;
        using std::memory_order_seq_cst;
    }
#else
    #include <boost/atomic.hpp>
    namespace sg
    {
        using boost::atomic;

        using boost::atomic_thread_fence;
        using boost::atomic_signal_fence;

        using boost::memory_order_relaxed;
        using boost::memory_order_release;
        using boost::memory_order_acquire;
        using boost::memory_order_consume;
        using boost::memory_order_acq_rel;
        using boost::memory_order_seq_cst;
    }
#endif


#include <boost/assert.hpp>

#endif
