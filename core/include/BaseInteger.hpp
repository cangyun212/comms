#ifndef __SG_CORE_BASE_INTEGER_HPP__
#define __SG_CORE_BASE_INTEGER_HPP__

#include "Core.hpp"

//#include <vector>
#include <cmath>
#include <unordered_map>
#include <string>
#include <memory>

#include "boost/range/adaptor/reversed.hpp"

#include "Utils.hpp"

#define SG_BASE_MAX_DIGIT_ITERATION     64

namespace sg
{
    typedef uint8_t BaseType; // support base from 2 to 255

    template <char... chars>
    struct BaseStringCodeSet
    {
        static std::string const& CodeSet()
        {
            static const std::string s_codeSet{ {chars...} };
            return s_codeSet;
        }
    };

    typedef BaseStringCodeSet<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'> BaseHexCodeSet;
    typedef BaseStringCodeSet<'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'> BaseDecimalCodeSet;
    typedef BaseStringCodeSet<'0', '1'> BaseBinaryCodeSet;

    typedef uint32_t BaseCodeType; // use 32bit type to enlarge code value range

    template <BaseType Base, typename T>
    class BaseCode
    {
    public:
        BaseCode(T const& sequence)
        {
            size_t n = 0;
            for (auto const& c : sequence)
            {
                auto r = m_codeMap.insert(std::make_pair(static_cast<BaseCodeType>(c), static_cast<BaseType>(n++)));
                SG_ASSERT(r.second);
            }

            SG_ASSERT(n < std::numeric_limits<BaseType>::max());
        }

    public:
        constexpr BaseType GetBase() const { return Base; }

    public:
        
        struct BaseTranslateCode
        {
            bool valid;
            BaseType num;
        };

        BaseTranslateCode Translate(BaseCodeType code) const
        {
            BaseTranslateCode res;
            res.valid = false;

            auto it = m_codeMap.find(code);
            if (it != m_codeMap.end())
            {
                res.valid = true;
                res.num = it->second;
            }

            return res;
        }

    private:
        typedef std::unordered_map<BaseCodeType, BaseType> BaseCodeMap;
        BaseCodeMap m_codeMap;
    };

    template <BaseType Base, char... chars>
    class BaseCode<Base, BaseStringCodeSet<chars...>>
    {
    public:
        BaseCode() 
        {
            if (s_codeMap.empty()) // for same BaseStringCodeSet type, the code map should be same
            {
                std::string const& codeSet = BaseStringCodeSet<chars...>::CodeSet();
                SG_ASSERT(codeSet.size() == Base);

                BaseType n = 0;
                for (auto const& c : codeSet)
                {
                    auto r = s_codeMap.insert(std::make_pair(c, n++));
                    SG_ASSERT(r.second);
                }
            }
        }

    public:
        constexpr BaseType GetBase() const { return Base; }

        struct BaseTranslateCode
        {
            bool valid;
            BaseType num;
        };

        BaseTranslateCode Translate(char code) const
        {
            BaseTranslateCode res;
            res.valid = false;

            auto it = s_codeMap.find(code);
            if (it != s_codeMap.end())
            {
                res.valid = true;
                res.num = it->second;
            }

            return res;
        }

    private:
        typedef std::unordered_map<char, BaseType> BaseCodeMap;
        static BaseCodeMap s_codeMap;
    };

    template <BaseType Base, char... chars>
    std::unordered_map<char, BaseType> BaseCode<Base, BaseStringCodeSet<chars...>>::s_codeMap;

    typedef BaseCode<16, BaseHexCodeSet>    BaseHex;
    typedef BaseCode<10, BaseDecimalCodeSet> BaseDecimal;
    typedef BaseCode<2, BaseBinaryCodeSet> BaseBinary;

    template <BaseType Base>
    class BaseInteger
    {
    public:
        //BaseInteger(BaseInteger const& rhs) : m_digits(rhs.m_digits) {}

        explicit BaseInteger(uint64_t u) 
            : m_counts(0) 
            , m_capacity(SG_BASE_MAX_DIGIT_ITERATION) // uint64_t has 64 digits max for base 2 which also max for any other base
            , m_digits(MakeArrayPtr(BaseType, m_capacity))
        {
            static_assert(Base >= 2 && Base <= std::numeric_limits<BaseType>::max(), "invalid base number");

            *this = u;
            //do
            //{
            //    this->PushBack(static_cast<BaseType>(u % Base));
            //    u /= Base;
            //} while (u >= Base);

            //if (u)
            //    this->PushBack(u);
        }

        template <typename T, typename U>
        BaseInteger(T const& sequence, BaseCode<Base, U> const& code) 
            : m_counts(0)
            , m_capacity(SG_BASE_MAX_DIGIT_ITERATION)
            , m_digits(MakeArrayPtr(BaseType, m_capacity))
        {
            static_assert(Base >= 2 && Base <= std::numeric_limits<BaseType>::max(), "invalid base number");

            for (auto const& c : boost::adaptors::reverse(sequence))
            {
                BaseCode<Base, U>::BaseTranslateCode res = code.Translate(c);
                SG_ASSERT(res.valid);

                this->PushBack(res.num);
            }

            if (!m_counts) // empty sequence end up 0
            {
                this->PushBack(0);
            }
        }

        template <size_t N, typename T>
        BaseInteger(const char(&str)[N], BaseCode<Base, T> const& code)
            : m_counts(0)
            , m_capacity(SG_BASE_MAX_DIGIT_ITERATION)
            , m_digits(MakeArrayPtr(BaseType, m_capacity))
        {
            static_assert(Base >= 2 && Base <= std::numeric_limits<BaseType>::max(), "invalid base number");

            for (size_t i = N - 1; i > 0; --i)
            {
                BaseCode<Base, T>::BaseTranslateCode res = code.Translate(str[i - 1]);
                SG_ASSERT(res.valid);

                this->PushBack(res.num);
            }

            if (!m_counts)
            {
                this->PushBack(0);
            }
        }

        template <typename T>
        BaseInteger(const char* pstrNum, size_t length, BaseCode<Base, T> const& code) 
            : m_counts(0)
            , m_capacity(SG_BASE_MAX_DIGIT_ITERATION)
            , m_digits(MakeArrayPtr(BaseType, m_capacity))
        {
            static_assert(Base >= 2 && Base <= std::numeric_limits<BaseType>::max(), "invalid base number");

            SG_ASSERT(length > 0);

            for (size_t i = length; i > 0; --i)
            {
                BaseCode<Base, T>::BaseTranslateCode res = code.Translate(pstrNum[i - 1]);
                SG_ASSERT(res.valid);

                this->PushBack(res.num);
            }
        }

        template <BaseType OtherBase>
        BaseInteger(BaseInteger<OtherBase> const& rhs) 
            : m_counts(0)
            , m_capacity(0)
            , m_digits(nullptr)
        {
            static_assert(Base >= 2 && Base <= std::numeric_limits<BaseType>::max(), "invalid base number");

            *this = rhs;
        }

        BaseInteger(BaseInteger const& rhs)
            : m_counts(0)
            , m_capacity(0)
            , m_digits(nullptr)
        {
            static_assert(Base >= 2 && Base <= std::numeric_limits<BaseType>::max(), "invalid base number");

            *this = rhs;
        }

    public:

        template <BaseType OtherBase>
        BaseInteger& operator= (BaseInteger<OtherBase> const& rhs)
        {
            if (m_counts)
                std::memset(m_digits.get(), 0, sizeof(BaseType) * m_counts); // clear all digits

            size_t counts = static_cast<size_t>(rhs.GetCounts() * (log(OtherBase) / log(Base))) + 1;
            if (m_counts < counts)
                this->IncreaseCount(counts - m_counts); // adjust the capacity

            m_counts = 0;

            // should be safe, capacity has enough space to hold all digits needed
            BaseType *pdigits = m_digits.get(); // C++17 support operator[]

            std::shared_ptr<BaseType> base2base(MakeArrayPtr(BaseType, counts)); // we need an array to represent the source base in new base
            BaseType *pbase2base = base2base.get(); // should be safe
            std::memset(pbase2base, 0, sizeof(BaseType) * counts);
            size_t bcount = 0;

            pbase2base[0] = 1;
            bcount = 1;

            for (size_t i = 0; i < rhs.GetCounts(); ++i)
            {
                // for each input digit, multi the new base representation and added into the output digit
                for (size_t j = 0; j < bcount; ++j)
                {
                    size_t sum = pbase2base[j] * rhs.GetDigit(i);

                    if (m_counts <= j)
                    {
                        this->PushBack(0, (j - m_counts + 1));
                        SG_ASSERT(pdigits == m_digits.get()); // no memory change due to capacity should be enough
                    }

                    this->AddNumAtPostion(sum, j);
                    SG_ASSERT(pdigits == m_digits.get());
                }

                // calculate next base representation
                if ((i + 1) < rhs.GetCounts())
                {
                    size_t rem = 0;
                    size_t level = 0;
                    size_t sum = pbase2base[level] * OtherBase;
                    do
                    {
                        rem = sum / Base;
                        pbase2base[level++] = static_cast<BaseType>(sum - rem * Base);

                        if (level >= bcount)
                        {
                            ++bcount;
                            SG_ASSERT(bcount <= counts);
                        }

                        sum = pbase2base[level] * OtherBase;
                        sum += rem;
                        if (sum < Base && (level + 1) == bcount)
                            pbase2base[level] = static_cast<BaseType>(sum);
                    } while (sum >= Base || (level + 1) < bcount);
                }
            }

            return *this;
        }

        BaseInteger& operator= (BaseInteger const& rhs)
        {
            if (this != &rhs)
            {
                if (m_counts < rhs.m_counts)
                    this->IncreaseCount(rhs.m_counts - m_counts);

                std::memcpy(m_digits.get(), rhs.m_digits.get(), sizeof(BaseType) * rhs.m_counts);
                m_counts = rhs.m_counts;
            }

            return *this;
        }

        BaseInteger& operator= (uint64_t u)
        {
            if (m_counts)
            {
                std::memset(m_digits.get(), 0, sizeof(BaseType) * m_counts);
                m_counts = 0;
            }

            do
            {
                this->PushBack(static_cast<BaseType>(u % Base));
                u /= Base;
            } while (u >= Base);

            if (u)
                this->PushBack(static_cast<BaseType>(u));

            return *this;
        }

        BaseInteger& operator+= (BaseInteger const& rhs)
        {
            if (this != &rhs)
            {
                if (m_counts < rhs.m_counts)
                    this->IncreaseCount(rhs.m_counts - m_counts);

                for (size_t i = 0; i < rhs.m_counts; ++i)
                {
                    this->AddNumAtPostion(rhs.m_digits.get()[i], i);
                }
            }
            else
            {
                *this *= 2U;
            }

            return *this;
        }

        BaseInteger& operator+= (uint64_t u)
        {
            BaseInteger<Base> num(u);

            return *this += num;
        }

        BaseInteger& operator*= (uint64_t u)
        {
            BaseInteger<Base> num(u);

            return *this *= num;
        }

        BaseInteger& operator*= (BaseInteger const& rhs)
        {
            if (this->IsZero())
                return *this;

            if (rhs.IsZero())
                return (*this = 0);

            BaseInteger const* top = this;
            BaseInteger const* bottom = &rhs;
            if (rhs.m_counts > m_counts)
            {
                top = &rhs;
                bottom = this;
            }

            BaseInteger sum(0);
            size_t counts = bottom->m_counts;
            for (size_t i = 0; i < counts; ++i)
            {
                sum += top->Mul(bottom->m_digits.get()[i], i);
            }

            return (*this = sum);
        }

        bool IsZero() const
        {
            return m_counts == 1 && m_digits.get()[0] == 0;
        }

        size_t GetCounts() const { return m_counts; }
        BaseType GetDigit(size_t index) const { SG_ASSERT(index < m_counts); return m_digits.get()[index]; }

    private:

        BaseInteger Mul(BaseType digit, size_t pad) const
        {
            BaseInteger n(0);

            n.IncreaseCount(m_counts + pad - n.m_counts); // n is 0000...0
            
            size_t i = 0;
            for (; i < pad; ++i); // skip pad digits

            uint64_t rem = 0;
            for (size_t j = 0; j < m_counts; ++j)
            {
                uint64_t mul = (uint64_t)(m_digits.get()[j]) * digit;

                mul += rem;
                rem = mul / Base;

                if (i >= n.m_counts)
                    n.IncreaseCount(i - n.m_counts + 1);

                n.m_digits.get()[i++] = static_cast<BaseType>(mul - rem * Base);
            }

            if (rem)
            {
                if (i >= n.m_counts)
                    n.IncreaseCount(1);

                n.AddNumAtPostion(rem, i);
            }

            return n;
        }

        void PushBack(BaseType digit, size_t counts = 1)
        {
            this->IncreaseCount(counts);

            for (size_t i = (m_counts - counts); i < m_counts; ++i)
            {
                m_digits.get()[i] = digit;
            }
        }

        void AddNumAtPostion(size_t num, size_t pos)
        {
            SG_ASSERT(pos < m_counts);

            size_t rem = 0;
            size_t sum = m_digits.get()[pos] + num;

            do
            {
                rem = sum / Base;
                m_digits.get()[pos++] = static_cast<BaseType>(sum - rem * Base);

                if (pos >= m_counts && rem)
                    this->IncreaseCount(1);

                if (pos < m_counts)
                    sum = m_digits.get()[pos] + rem;
                else
                    break;

                if (sum < Base)
                    m_digits.get()[pos] = (BaseType)sum;
            } while (sum >= Base);
            
        }

        void IncreaseCount(size_t counts)
        {
            m_counts += counts;
            if (m_counts > m_capacity)
            {
                if (!m_capacity) // for safe reasone
                    m_capacity = m_counts;

                while (m_counts > m_capacity)
                {
                    m_capacity *= 2; // double memory each time
                }

                std::shared_ptr<BaseType> p(MakeArrayPtr(BaseType, m_capacity));
                if (m_counts > counts)
                    std::memcpy(p.get(), m_digits.get(), sizeof(BaseType) * (m_counts - counts));

                m_digits = p;
            }

            std::memset(m_digits.get() + (m_counts - counts), 0, sizeof(BaseType) * counts);
        }


    private:
        size_t  m_counts;
        size_t  m_capacity;
        std::shared_ptr<BaseType>       m_digits;
    };

    typedef BaseInteger<16>     HexInteger;
    typedef BaseInteger<10>     DecimalInteger;
    typedef BaseInteger<2>      BinaryInteger;
}

#endif // !__SG_CORE_BASE_INTEGER_HPP__


