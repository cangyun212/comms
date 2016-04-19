#ifndef __CORE_UTILS_HPP__
#define __CORE_UTILS_HPP__

#include "core/core.hpp"

#include <cstring>

#include "boost/checked_delete.hpp"

#define CORE_UNREF_PARAM(x) (void)(x)

#define CORE_SAFE_DELETE(p) if (p) { delete p; p = nullptr; }
#define CORE_SAFE_DELETE_ARRAY(p) if (p) { delete[] p; p = nullptr; }

namespace sg
{
    CORE_API uint32_t LastError();


	template <typename T>
	inline shared_ptr<T> MakeSharedPtr()
	{
		return shared_ptr<T>(new T, boost::checked_deleter<T>());
	}

	template <typename T, typename A1>
	inline shared_ptr<T> MakeSharedPtr(A1 const & a1)
	{
		return shared_ptr<T>(new T(a1), boost::checked_deleter<T>());
	}

	template <typename T, typename A1>
	inline shared_ptr<T> MakeSharedPtr(A1& a1)
	{
		return shared_ptr<T>(new T(a1), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2>
	inline shared_ptr<T> MakeSharedPtr(A1 const & a1, A2 const & a2)
	{
		return shared_ptr<T>(new T(a1, a2), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2>
	inline shared_ptr<T> MakeSharedPtr(A1& a1, A2& a2)
	{
		return shared_ptr<T>(new T(a1, a2), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2, typename A3>
	inline shared_ptr<T> MakeSharedPtr(A1 const & a1, A2 const & a2, A3 const & a3)
	{
		return shared_ptr<T>(new T(a1, a2, a3), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2, typename A3>
	inline shared_ptr<T> MakeSharedPtr(A1& a1, A2& a2, A3& a3)
	{
		return shared_ptr<T>(new T(a1, a2, a3), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2, typename A3, typename A4>
	inline shared_ptr<T> MakeSharedPtr(A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4)
	{
		return shared_ptr<T>(new T(a1, a2, a3, a4), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2, typename A3, typename A4>
	inline shared_ptr<T> MakeSharedPtr(A1& a1, A2& a2, A3& a3, A4& a4)
	{
		return shared_ptr<T>(new T(a1, a2, a3, a4), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
	inline shared_ptr<T> MakeSharedPtr(A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4,
		A5 const & a5)
	{
		return shared_ptr<T>(new T(a1, a2, a3, a4, a5), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5>
	inline shared_ptr<T> MakeSharedPtr(A1& a1, A2& a2, A3& a3, A4& a4, A5& a5)
	{
		return shared_ptr<T>(new T(a1, a2, a3, a4, a5), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	inline shared_ptr<T> MakeSharedPtr(A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4,
		A5 const & a5, A6 const & a6)
	{
		return shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	inline shared_ptr<T> MakeSharedPtr(A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6)
	{
		return shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
		typename A7>
	inline shared_ptr<T> MakeSharedPtr(A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4,
		A5 const & a5, A6 const & a6, A7 const & a7)
	{
		return shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6, a7), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
		typename A7>
	inline shared_ptr<T> MakeSharedPtr(A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7)
	{
		return shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6, a7), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
		typename A7, typename A8>
	inline shared_ptr<T> MakeSharedPtr(A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4,
		A5 const & a5, A6 const & a6, A7 const & a7, A8 const & a8)
	{
		return shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6, a7, a8), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
		typename A7, typename A8>
	inline shared_ptr<T> MakeSharedPtr(A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7,
		A8& a8)
	{
		return shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6, a7, a8), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
		typename A7, typename A8, typename A9>
	inline shared_ptr<T> MakeSharedPtr(A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4,
		A5 const & a5, A6 const & a6, A7 const & a7, A8 const & a8, A9 const & a9)
	{
		return shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6, a7, a8, a9), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
		typename A7, typename A8, typename A9>
	inline shared_ptr<T> MakeSharedPtr(A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7,
		A8& a8, A9& a9)
	{
		return shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6, a7, a8, a9), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
		typename A7, typename A8, typename A9, typename A10>
	inline shared_ptr<T> MakeSharedPtr(A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4,
		A5 const & a5, A6 const & a6, A7 const & a7, A8 const & a8, A9 const & a9, A10 const & a10)
	{
		return shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10), boost::checked_deleter<T>());
	}

	template <typename T, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6,
		typename A7, typename A8, typename A9, typename A10>
	inline shared_ptr<T> MakeSharedPtr(A1& a1, A2& a2, A3& a3, A4& a4, A5& a5, A6& a6, A7& a7,
		A8& a8, A9& a9, A10& a10)
	{
		return shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10), boost::checked_deleter<T>());
	}


}

#endif
