#include "core/core.hpp"
#include "core/console/core_console_stream.hpp"
#include "core/console/core_console_table.hpp"

#include <ostream>

#include "boost/any.hpp"
#include "boost/array.hpp"


namespace sg {

    std::ostream& operator<<(std::ostream & s, ConsoleTable const& t)
    {
        return t.Print(s);
    }

    ConsoleStream& operator << (ConsoleStream & s, ConsoleTable const& t)
    {
        return t.Print(s);
    }


    CTableStyle::StyleMapPtr CTableStyle::s_styles = nullptr;

    void CTableStyle::SetStyleDefinition(const std::string &name, CTableStylePtr style)
    {
        BOOST_ASSERT(style);

        initStyles();

        (*s_styles)[name] = style;
    }

    CTableStylePtr CTableStyle::GetStyleDefinition(const std::string &name)
    {
        initStyles();

        StyleMap::iterator it = s_styles->find(name);
        if (it != s_styles->end())
        {
            return it->second;
        }
        else
        {
            return nullptr;
        }
    }

    void CTableStyle::initStyles()
    {
        if (!s_styles)
        {
            //======== ============ ==========
            // XX       YY           ZZ
            //======== ============ ==========
            // aa       bb           cc
            // dd       ee           ff
            //======== ============ ==========

            CTableStylePtr borderless = MakeSharedPtr<CTableStyle>();
            borderless->SetHorizontalBorderChar('=')
                    .SetVerticalBorderChar(' ')
                    .SetCrossingChar(' ');

            // XX       YY          ZZ
            // aa       bb          cc
            // dd       ee          ff

            CTableStylePtr compact = MakeSharedPtr<CTableStyle>();
            compact->SetHorizontalBorderChar('\0')
                    .SetVerticalBorderChar('\0')
                    .SetCrossingChar('\0');

            //+---------------------------------+
            //| XX  |   YY          |   ZZ      |
            //+---------------------------------+
            //| aa  |   bb          |   cc      |
            //| dd  |   ee          |   ff      |
            //+-----|---------------+-----------+

            CTableStylePtr def = MakeSharedPtr<CTableStyle>();

            s_styles = MakeSharedPtr<StyleMap>();
            s_styles->insert(std::make_pair("default", def));
            s_styles->insert(std::make_pair("borderless", borderless));
            s_styles->insert(std::make_pair("compact", compact));
        }
    }

    CTableItem::CTableItem(const CTableItem &it)
        : m_stream(it.m_stream ? it.m_stream->Clone() : nullptr)
        , m_val(it.m_val)
    {

    }

    CTableItem::~CTableItem()
    {
        CORE_SAFE_DELETE(m_stream);
    }

    void CTableItem::Swap(CTableItem &r)
    {
        std::swap(m_stream, r.m_stream);
        std::swap(m_val, r.m_val);
    }

    std::ostream & operator << (std::ostream & o, CTableItem const& it)
    {
        if (it.m_stream)
            return it.m_stream->Print(o, it.m_val);
        return o;
    }

    CTableRow::~CTableRow()
    {
        CORE_SAFE_DELETE_ARRAY(cells);
        CORE_SAFE_DELETE(fmt);
    }

    ConsoleTable::ConsoleTable()
        : m_header(nullptr)
    {
        m_style = CTableStyle::GetStyleDefinition("default");
    }

    ConsoleTable::~ConsoleTable()
    {
        CORE_SAFE_DELETE(m_header);
        for(size_t i = 0; i < m_rows.size(); ++i)
        {
            CORE_SAFE_DELETE(m_rows[i]);
        }

    }

    void ConsoleTable::SetStyle(const std::string &name)
    {
        m_style = CTableStyle::GetStyleDefinition(name);
        BOOST_ASSERT(m_style);
    }

    void ConsoleTable::SetStyle(CTableStylePtr style)
    {
        BOOST_ASSERT(style);
        m_style = style;
    }

    ConsoleTable& ConsoleTable::AddRow(const CTableSeparator &)
    {
        CTableSeparator *s = new CTableSeparator();
        m_rows.push_back(s);

        return *this;
    }

    void ConsoleTable::PreparePrint() const
    {
        m_col_width.clear();

        if (m_header)
        {
            for (size_t i = 0; i < m_header->col; ++i)
            {
                PrepareCell(m_header, i, m_style->GetHeaderFormat());
                m_col_width.push_back(m_header->cells[i].str.size());
            }
        }

        for (size_t i = 0; i < m_rows.size(); ++i)
        {
            if (!m_rows[i]->IsSeparator())
            {
                CTableRow *r = (CTableRow*)m_rows[i];
                for (size_t j = 0; j < r->col; ++j)
                {
                    PrepareCell(r, j, m_style->GetCellFormat());
                    if (j < m_col_width.size())
                    {
                        m_col_width[j] =
                                m_col_width[j] < r->cells[j].str.size() ?
                                r->cells[j].str.size() :
                                m_col_width[j];
                    }
                    else
                    {
                        m_col_width.push_back(r->cells[j].str.size());
                    }
                }
            }
        }
    }

    void ConsoleTable::PrepareCell(CTableRow *r, size_t col, CTableFormat const*def_fmt) const
    {
        CTableCell &c = r->cells[col];
        CTableFormat const*fmt = GetCellFormat(r, col, def_fmt);
        c.str = (boost::format(fmt->spec) % c.val).str();
    }

    CTableFormat const* ConsoleTable::GetCellFormat(CTableRow *r, size_t col, CTableFormat const *def_fmt) const
    {
        if (col < r->col)
        {
            if (r->cells[col].fmt)
                return r->cells[col].fmt;
        }

        if (col < m_header->col)
        {
            if (m_header->cells[col].fmt)
                return m_header->cells[col].fmt;
        }

        if (r->fmt)
            return r->fmt;

        return def_fmt;
    }

    void ConsoleTable::SetCellFormat(size_t row, size_t col, const CTableFormat &fmt)
    {
        if (row < m_rows.size())
        {
            if (!m_rows[row]->IsSeparator())
            {
                CTableRow *r = (CTableRow*)m_rows[row];
                if (col < r->col)
                {
                    CORE_SAFE_DELETE(r->cells[col].fmt);
                    r->cells[col].fmt = new CTableFormat(fmt);
                }
            }
        }
    }

    void ConsoleTable::SetRowFormat(size_t row, const CTableFormat &fmt)
    {
        if (row < m_rows.size())
        {
            if (!m_rows[row]->IsSeparator())
            {
                CTableRow *r = (CTableRow*)m_rows[row];
                CORE_SAFE_DELETE(r->fmt);
                r->fmt = new CTableFormat(fmt);
            }
        }
    }

    void ConsoleTable::SetHeaderCellFormat(size_t col, const CTableFormat &fmt)
    {
        if (m_header)
        {
            if (col < m_header->col)
            {
                CORE_SAFE_DELETE(m_header->cells[col].fmt);
                m_header->cells[col].fmt = new CTableFormat(fmt);
            }
        }
    }

}



