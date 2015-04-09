#ifndef __SG_CORE_CONSOLE_TABLE_HPP__
#define __SG_CORE_CONSOLE_TABLE_HPP__

#include "core/core.hpp"
#include "core/core_utils.hpp"

#include <iomanip>
#include <vector>
#include <algorithm>

#include "boost/format.hpp"
#include "boost/any.hpp"
#include "boost/algorithm/string.hpp"

namespace sg {

    enum CTablePadType
    {
        TPT_AlignRight = 0,
        TPT_AlignLeft,
        TPT_AlignCentered
    };

    struct CTableFormat
    {
        CTableFormat(): pad(TPT_AlignRight) {}
        CTableFormat(std::string const& sp, CTablePadType p): spec(sp), pad(p) {}

        std::string     spec;
        CTablePadType    pad;
    };

    class CTableStyle;
    typedef shared_ptr<CTableStyle>      CTableStylePtr;

    class CTableStyle
    {
    public:
        static void SetStyleDefinition(std::string const& name, CTableStylePtr style);
        static CTableStylePtr GetStyleDefinition(std::string const& name);

    private:
         static void initStyles();

    private:
        typedef unordered_map<std::string, CTableStylePtr>   StyleMap;
        typedef shared_ptr<StyleMap>    StyleMapPtr;

        static StyleMapPtr      s_styles;

    public:
        CTableStyle()
            : m_pad(" ")
            , m_hb("-")
            , m_vb("|")
            , m_cc("+")
            , m_header(" %|s| ", TPT_AlignLeft)
            , m_cell(" %|s| ", TPT_AlignLeft)
        {

        }

    public:
        CTableStyle& SetPaddingChar(char ch) { m_pad = std::string(1, ch); return *this; }
        std::string GetPaddingChar() const { return m_pad; }

        CTableStyle& SetHorizontalBorderChar(char ch)
        {
            if (ch != '\0')
                m_hb = std::string(1, ch);
            else
                m_hb.clear();
            return *this;
        }
        std::string GetHorizontalBorderChar() const { return m_hb; }

        CTableStyle& SetVerticalBorderChar(char ch)
        {
            if (ch != '\0')
                m_vb = std::string(1, ch);
            else
                m_vb.clear();
            return *this;
        }
        std::string GetVerticalBorderChar() const { return m_vb; }

        CTableStyle& SetCrossingChar(char ch)
        {
            if (ch != '\0')
                m_cc = std::string(1, ch);
            else
                m_cc.clear();
            return *this;
        }
        std::string GetCrossingChar() const { return m_cc; }

        CTableStyle& SetHeaderFormat(CTableFormat const& fmt) { m_header = fmt; return *this; }
  const CTableFormat*GetHeaderFormat() const { return &m_header; }

        CTableStyle& SetCellFormat(CTableFormat const& fmt) { m_cell = fmt; return *this; }
  const CTableFormat*GetCellFormat() const { return &m_cell; }

    private:
        std::string         m_pad;
        std::string         m_hb;
        std::string         m_vb;
        std::string         m_cc;
        CTableFormat         m_header;
        CTableFormat         m_cell;
    };

    struct CTableItemStream
    {
        virtual std::ostream& Print(std::ostream & o, boost::any const& a) const = 0;
        virtual CTableItemStream* Clone() const = 0;
        virtual ~CTableItemStream() {}
    };

    template <typename T>
    struct CTableItemStream_Impl : public CTableItemStream
    {
        std::ostream& Print(std::ostream & o, boost::any const& a) const CORE_OVERRIDE
        {
            return o << *boost::any_cast<T>(&a);
        }


        CTableItemStream* Clone() const CORE_OVERRIDE
        {
            return new CTableItemStream_Impl<T>();
        }
    };

    class CTableItem
    {
    public:
        CTableItem(): m_stream(nullptr) {}

        template <typename T>
        CTableItem(T const& value)
            : m_stream(new CTableItemStream_Impl<T>())
            , m_val(value)
        {

        }

        CTableItem(CTableItem const& it);

       ~CTableItem();

        template <typename T>
        CTableItem& operator=(T const& r)
        {
            CTableItem(r).Swap(*this);
            return *this;
        }

        CTableItem& operator=(CTableItem const& r)
        {
            CTableItem(r).Swap(*this);
            return *this;
        }

    public:
        friend std::ostream& operator << (std::ostream & o, CTableItem const& it);

    private:
        void Swap(CTableItem & r);

    private:
        CTableItemStream        *m_stream;
        boost::any              m_val;
    };

    struct CTableCell
    {
        CTableCell(): fmt(nullptr) {}
        CTableCell(CTableCell const& c)
            : val(c.val)
            , str(c.str)
            , fmt(nullptr)
        {
            if (c.fmt)
            {
                fmt = new CTableFormat();
                *fmt = *c.fmt;
            }
        }

        ~CTableCell()
        {
            CORE_SAFE_DELETE(fmt);
        }

        CTableCell& operator=(CTableCell const& r)
        {
            CTableCell(r).Swap(*this);
            return *this;
        }

        CTableCell& operator=(CTableItem const& r)
        {
            val = r;
            str = std::string();
            CORE_SAFE_DELETE(fmt);
            return *this;
        }

        void Swap(CTableCell & r)
        {
            std::swap(val, r.val);
            std::swap(str, r.str);
            std::swap(fmt, r.fmt);
        }

        CTableItem       val;
        std::string     str;
        CTableFormat    *fmt;
    };

    struct CTableSeparator
    {
        virtual ~CTableSeparator() {}
        virtual bool    IsSeparator() const { return true; }
    };

    struct CTableRow : public CTableSeparator
    {
        CTableRow(): cells(nullptr), col(0), fmt(nullptr) {}

       ~CTableRow();

        bool IsSeparator() const CORE_OVERRIDE { return false; }

        CTableRow* Clone();

        CTableCell      *cells;
        size_t          col;
        CTableFormat    *fmt;
    };

    class ConsoleTable
    {
    public:
        ConsoleTable();
       ~ConsoleTable();

    public:
        void            SetStyle(std::string const& name);
        void            SetStyle(CTableStylePtr style);
        CTableStylePtr  GetStyle() const { return m_style; }

        void            SetCellFormat(size_t row, size_t col, CTableFormat const& fmt);
        void            SetRowFormat(size_t row, CTableFormat const& fmt);
        void            SetHeaderCellFormat(size_t col, CTableFormat const& fmt);

        template <size_t N>
        void SetHeader(CTableItem (*it)[N])
        {
            BOOST_ASSERT(it);
            CORE_SAFE_DELETE(m_header);

            m_header = new CTableRow();

            m_header->col = N;
            m_header->cells = new CTableCell[N];
            std::copy(*it, *it + N, m_header->cells);
        }

        template <size_t N>
        ConsoleTable& AddRow(CTableItem (*it)[N])
        {
            BOOST_ASSERT(it);

            CTableRow *r = new CTableRow();
            m_rows.push_back(r);

            r->col = N;
            r->cells = new CTableCell[N];
            std::copy(*it, *it+N, r->cells);

            return *this;
        }

        ConsoleTable& AddRow(CTableSeparator const& sep);

        template <typename Stream>
        Stream& Print(Stream & s) const
        {
            PreparePrint();

            if (m_header)
            {
                PrintSeparator(s);
                PrintRow(s, m_header, m_style->GetHeaderFormat());
                PrintSeparator(s);
            }
            else
            {
                PrintSeparator(s);
            }

            typedef typename CORE_DECLTYPE(m_rows)   RowsType;
            CORE_FOREACH(typename RowsType::const_reference r, m_rows)
            {
                if (r->IsSeparator())
                {
                    PrintSeparator(s);
                }
                else
                {
                    PrintRow(s, (CTableRow*)r, m_style->GetCellFormat());
                }
            }

            PrintSeparator(s);

            return s;
        }

    private:
        void PreparePrint() const;
        void PrepareCell(CTableRow *r, size_t col, const CTableFormat *def_fmt) const;

        template <typename Stream>
        Stream& PrintSeparator(Stream & s) const
        {
            size_t col_num = m_col_width.size();
            if (0 == col_num)
                return s;

            if (m_style->GetHorizontalBorderChar().empty() && m_style->GetCrossingChar().empty())
                return s;

            std::string cross = m_style->GetCrossingChar();
            std::string separator(cross);
            for (size_t i = 0; i < col_num; ++i)
            {
                std::string hb(m_col_width[i], m_style->GetHorizontalBorderChar()[0]);
                separator += (hb + cross);
            }

            s << separator << "\n";
            return s;
        }

        template <typename Stream>
        Stream& PrintColumnSeparator(Stream & s) const
        {
            s << m_style->GetVerticalBorderChar();
            return s;
        }

        template <typename Stream>
        Stream& PrintRow(Stream & s, CTableRow *r, CTableFormat const* fmt) const
        {
            size_t col_num = m_col_width.size();
            if (0 == col_num)
                return s;

            BOOST_ASSERT(r->col <= col_num);

            PrintColumnSeparator(s);
            for (size_t i = 0; i < col_num; ++i)
            {
                PrintCell(s, r, i, fmt);
                PrintColumnSeparator(s);
            }

            s << "\n";
            return s;
        }

        template <typename Stream>
        Stream& PrintCell(Stream & s, CTableRow *row, size_t col, CTableFormat const* fmt) const
        {
            std::string cell;
            if (col < row->col)
                cell = row->cells[col].str;

            fmt = GetCellFormat(row, col, fmt);
            size_t width = m_col_width[col];
            std::string pad_fmt;
            if (TPT_AlignCentered == fmt->pad)
            {
                pad_fmt = (boost::format("%%|=%1%|") % width).str();
            }
            else if (TPT_AlignLeft == fmt->pad)
            {
                pad_fmt = (boost::format("%%|-%1%|") % width).str();
            }
            else
            {
                pad_fmt = (boost::format("%%|%1%|") % width).str();
            }

            s << boost::format(pad_fmt) % boost::io::group(std::setfill(m_style->GetPaddingChar()[0]), cell);

            return s;
        }

        CTableFormat const* GetCellFormat(CTableRow *r, size_t col, const CTableFormat *def_fmt) const;

    private:
        CTableStylePtr                   m_style;
        CTableRow                       *m_header;
        std::vector<CTableSeparator*>    m_rows;
        mutable std::vector<size_t>             m_col_width;
    };

    std::ostream& operator<<(std::ostream & s, ConsoleTable const& t);

    class ConsoleStream;
    ConsoleStream& operator<<(ConsoleStream & s, ConsoleTable const& t);
}

#endif



