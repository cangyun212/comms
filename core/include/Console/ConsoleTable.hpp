#ifndef __SG_CORE_CONSOLE_TABLE_HPP__
#define __SG_CORE_CONSOLE_TABLE_HPP__

#include "Core.hpp"

#include <iomanip>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "boost/format.hpp"
#include "boost/any.hpp"

#include "Utils.hpp"

namespace sg {

    enum ConsoleTablePadType
    {
        CTPT_AlignRight = 0,
        CTPT_AlignLeft,
        CTPT_AlignCentered
    };

    struct ConsoleTableFormat
    {
        ConsoleTableFormat(): pad(CTPT_AlignRight) {}
        ConsoleTableFormat(std::string const& sp, ConsoleTablePadType p): spec(sp), pad(p) {}

        std::string             spec;
        ConsoleTablePadType     pad;
    };

    typedef std::shared_ptr<ConsoleTableFormat>         ConsoleTableFormatPtr;
    typedef std::shared_ptr<const ConsoleTableFormat>   ConsoleTableFormatConstPtr;

    class ConsoleTableStyle;
    typedef std::shared_ptr<ConsoleTableStyle>      ConsoleTableStylePtr;

    class ConsoleTableStyle
    {
    public:
        static void SetStyleDefinition(std::string const& name, ConsoleTableStylePtr style);
        static ConsoleTableStylePtr GetStyleDefinition(std::string const& name);

    private:
         static void initStyles();

    private:
        typedef std::unordered_map<std::string, ConsoleTableStylePtr>   StyleMap;
        typedef std::shared_ptr<StyleMap>    StyleMapPtr;

        static StyleMapPtr      s_styles;

    public:
        ConsoleTableStyle()
            : m_pad(" ")
            , m_hb("-")
            , m_vb("|")
            , m_cc("+")
            , m_header(MakeSharedPtr<ConsoleTableFormat>(" %|s| ", CTPT_AlignLeft))
            , m_cell(MakeSharedPtr<ConsoleTableFormat>(" %|s| ", CTPT_AlignLeft))
        {

        }

    public:
        ConsoleTableStyle& SetPaddingChar(char ch) { m_pad = std::string(1, ch); return *this; }
        std::string GetPaddingChar() const { return m_pad; }

        ConsoleTableStyle& SetHorizontalBorderChar(char ch)
        {
            if (ch != '\0')
                m_hb = std::string(1, ch);
            else
                m_hb.clear();
            return *this;
        }
        std::string GetHorizontalBorderChar() const { return m_hb; }

        ConsoleTableStyle& SetVerticalBorderChar(char ch)
        {
            if (ch != '\0')
                m_vb = std::string(1, ch);
            else
                m_vb.clear();
            return *this;
        }
        std::string GetVerticalBorderChar() const { return m_vb; }

        ConsoleTableStyle& SetCrossingChar(char ch)
        {
            if (ch != '\0')
                m_cc = std::string(1, ch);
            else
                m_cc.clear();
            return *this;
        }
        std::string GetCrossingChar() const { return m_cc; }

        ConsoleTableStyle& SetHeaderFormat(ConsoleTableFormat const& fmt) { *m_header = fmt; return *this; }
        ConsoleTableFormatConstPtr GetHeaderFormat() const { return m_header; }

        ConsoleTableStyle& SetCellFormat(ConsoleTableFormat const& fmt) { *m_cell = fmt; return *this; }
        ConsoleTableFormatConstPtr GetCellFormat() const { return m_cell; }

    private:
        std::string             m_pad;
        std::string             m_hb;
        std::string             m_vb;
        std::string             m_cc;
        ConsoleTableFormatPtr   m_header;
        ConsoleTableFormatPtr   m_cell;
    };

    struct ConsoleTableItemStream;
    typedef std::shared_ptr<ConsoleTableItemStream>   ConsoleTableItemStreamPtr;

    struct ConsoleTableItemStream
    {
        virtual std::ostream& Print(std::ostream & o, boost::any const& a) const = 0;
        virtual ~ConsoleTableItemStream() {}
    };

    template <typename T>
    struct ConsoleTableItemStream_Impl : public ConsoleTableItemStream
    {
        std::ostream& Print(std::ostream & o, boost::any const& a) const override
        {
            return o << *boost::any_cast<T>(&a);
        }
    };

    class ConsoleTableItem
    {
    public:
        ConsoleTableItem(): m_stream(nullptr) {}

        template <typename T>
        ConsoleTableItem(T const& value)
            : m_stream(MakeStream<T>())
            , m_val(value)
        {

        }

        template <typename T>
        ConsoleTableItem& operator=(T const& r)
        {
            ConsoleTableItem(r).Swap(*this);
            return *this;
        }

    public:
        friend std::ostream& operator << (std::ostream & o, ConsoleTableItem const& it);

    private:
        template <typename T>
        static ConsoleTableItemStreamPtr MakeStream()
        {
            static ConsoleTableItemStreamPtr s = MakeSharedPtr<ConsoleTableItemStream_Impl<T> >();

            return s;
        }

    private:
        void Swap(ConsoleTableItem & r);

    private:
        ConsoleTableItemStreamPtr   m_stream;
        boost::any                  m_val;
    };

    struct ConsoleTableCell
    {
        ConsoleTableCell(): fmt(nullptr) {}
        ConsoleTableCell(ConsoleTableCell const& c)
            : val(c.val)
            , str(c.str)
            , fmt(nullptr)
        {
            if (c.fmt)
            {
                fmt = MakeSharedPtr<ConsoleTableFormat>();
                *fmt = *c.fmt;
            }
        }

        ConsoleTableCell& operator=(ConsoleTableCell const& r)
        {
            ConsoleTableCell(r).Swap(*this);
            return *this;
        }

        ConsoleTableCell& operator=(ConsoleTableItem const& r)
        {
            val = r;
            str = std::string();
            fmt = nullptr;
            return *this;
        }

        void Swap(ConsoleTableCell & r)
        {
            std::swap(val, r.val);
            std::swap(str, r.str);
            std::swap(fmt, r.fmt);
        }

        ConsoleTableItem            val;
        std::string                 str;
        ConsoleTableFormatPtr       fmt;
    };

    typedef std::shared_ptr<ConsoleTableCell>   ConsoleTableCellPtr;

    struct ConsoleTableSeparator
    {
        virtual ~ConsoleTableSeparator() {}
        virtual bool    IsSeparator() const { return true; }
    };

    struct ConsoleTableRow : public ConsoleTableSeparator
    {
        ConsoleTableRow(): cells(nullptr), fmt(nullptr), col(0) {}

        bool IsSeparator() const override { return false; }

        ConsoleTableCell & GetCell(size_t c);

        ConsoleTableCellPtr     cells;
        ConsoleTableFormatPtr   fmt;
        size_t                  col;
    };

    typedef std::shared_ptr<ConsoleTableSeparator> ConsoleTableSeparatorPtr;
    typedef std::shared_ptr<ConsoleTableRow> ConsoleTableRowPtr;

    class CORE_API ConsoleTable
    {
    public:
        ConsoleTable();

    public:
        void                    SetStyle(std::string const& name);
        void                    SetStyle(ConsoleTableStylePtr style);
        ConsoleTableStylePtr    GetStyle() const { return m_style; }

        void    SetCellFormat(size_t row, size_t col, ConsoleTableFormat const& fmt);
        void    SetRowFormat(size_t row, ConsoleTableFormat const& fmt);
        void    SetHeaderCellFormat(size_t col, ConsoleTableFormat const& fmt);

        template <size_t N>
        void SetHeader(ConsoleTableItem (*it)[N])
        {
            BOOST_ASSERT(it);

            m_header = MakeSharedPtr<ConsoleTableRow>();

            m_header->col = N;
            m_header->cells = MakeArraryPtr(ConsoleTableCell, N);
            std::copy(*it, *it + N, m_header->cells.get());
        }

        template <size_t N>
        ConsoleTable& AddRow(ConsoleTableItem (*it)[N])
        {
            BOOST_ASSERT(it);

            ConsoleTableRowPtr r = MakeSharedPtr<ConsoleTableRow>();
            m_rows.push_back(r);

            r->col = N;
            r->cells = MakeArraryPtr(ConsoleTableCell, N);
            std::copy(*it, *it+N, r->cells.get());

            return *this;
        }

        ConsoleTable& AddRow(ConsoleTableSeparator const& sep);

        template <typename Stream>
        Stream& Print(Stream & s) const
        {
            this->PreparePrint();

            ConsoleTableFormatConstPtr defmt = m_style->GetHeaderFormat();

            if (m_header)
            {
                this->PrintSeparator(s);
                this->PrintRow(s, m_header, defmt);
                this->PrintSeparator(s);
            }
            else
            {
                this->PrintSeparator(s);
            }

            for (auto const& r : m_rows)
            {
                if (r->IsSeparator())
                    this->PrintSeparator(s);
                else
                    this->PrintRow(s, std::static_pointer_cast<ConsoleTableRow>(r), defmt);

            }

            PrintSeparator(s);

            return s;
        }

    private:
        void PreparePrint() const;
        void PrepareCell(ConsoleTableRowPtr const& r, size_t col, ConsoleTableFormatConstPtr const& def_fmt) const;

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
        Stream& PrintRow(Stream & s, ConsoleTableRowPtr const& r, ConsoleTableFormatConstPtr const& fmt) const
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
        Stream& PrintCell(Stream & s, ConsoleTableRowPtr const& row, size_t col, ConsoleTableFormatConstPtr const& fmt) const
        {
            std::string cell;
            if (col < row->col)
                cell = row->GetCell(col).str;

            ConsoleTableFormatConstPtr _fmt = this->GetCellFormat(row, col, fmt);
            size_t width = m_col_width[col];
            std::string pad_fmt;
            if (CTPT_AlignCentered == _fmt->pad)
            {
                pad_fmt = (boost::format("%%|=%1%|") % width).str();
            }
            else if (CTPT_AlignLeft == _fmt->pad)
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

       ConsoleTableFormatConstPtr GetCellFormat(ConsoleTableRowPtr const& r, size_t col, ConsoleTableFormatConstPtr const& def_fmt) const;

    private:
        ConsoleTableStylePtr                    m_style;
        ConsoleTableRowPtr                      m_header;
        std::vector<ConsoleTableSeparatorPtr>   m_rows;
        mutable std::vector<size_t>             m_col_width;
    };

    std::ostream& operator<<(std::ostream & s, ConsoleTable const& t);

    class ConsoleWindowStream;
    class ConsoleStdColorStream;
    CORE_API ConsoleWindowStream& operator<<(ConsoleWindowStream & s, ConsoleTable const& t);
    CORE_API ConsoleStdColorStream& operator<<(ConsoleStdColorStream & s, ConsoleTable const& t);
}

#endif



