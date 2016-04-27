#include "Core.hpp"

#include <ostream>

#include "boost/any.hpp"
#include "boost/array.hpp"

#include "Console/ConsoleStream.hpp"
#include "Console/ConsoleTable.hpp"

namespace sg 
{

    std::ostream& operator<<(std::ostream & s, ConsoleTable const& t)
    {
        return t.Print(s);
    }

    ConsoleWindowStream& operator << (ConsoleWindowStream & s, ConsoleTable const& t)
    {
        return t.Print(s);
    }

    ConsoleStdColorStream& operator << (ConsoleStdColorStream & s, ConsoleTable const& t)
    {
        return t.Print(s);
    }


    ConsoleTableStyle::StyleMapPtr ConsoleTableStyle::s_styles = nullptr;

    void ConsoleTableStyle::SetStyleDefinition(const std::string &name, ConsoleTableStylePtr style)
    {
        BOOST_ASSERT(style);

        initStyles();

        (*s_styles)[name] = style;
    }

    ConsoleTableStylePtr ConsoleTableStyle::GetStyleDefinition(const std::string &name)
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

    void ConsoleTableStyle::initStyles()
    {
        if (!s_styles)
        {
            //======== ============ ==========
            // XX       YY           ZZ
            //======== ============ ==========
            // aa       bb           cc
            // dd       ee           ff
            //======== ============ ==========

            ConsoleTableStylePtr borderless = MakeSharedPtr<ConsoleTableStyle>();
            borderless->SetHorizontalBorderChar('=')
                    .SetVerticalBorderChar(' ')
                    .SetCrossingChar(' ');

            // XX       YY          ZZ
            // aa       bb          cc
            // dd       ee          ff

            ConsoleTableStylePtr compact = MakeSharedPtr<ConsoleTableStyle>();
            compact->SetHorizontalBorderChar('\0')
                    .SetVerticalBorderChar('\0')
                    .SetCrossingChar('\0');

            //+---------------------------------+
            //| XX  |   YY          |   ZZ      |
            //+---------------------------------+
            //| aa  |   bb          |   cc      |
            //| dd  |   ee          |   ff      |
            //+-----|---------------+-----------+

            ConsoleTableStylePtr def = MakeSharedPtr<ConsoleTableStyle>();

            s_styles = MakeSharedPtr<StyleMap>();
            s_styles->insert(std::make_pair("default", def));
            s_styles->insert(std::make_pair("borderless", borderless));
            s_styles->insert(std::make_pair("compact", compact));
        }
    }

    void ConsoleTableItem::Swap(ConsoleTableItem &r)
    {
        std::swap(m_stream, r.m_stream);
        std::swap(m_val, r.m_val);
    }

    std::ostream & operator << (std::ostream & o, ConsoleTableItem const& it)
    {
        if (it.m_stream)
            return it.m_stream->Print(o, it.m_val);
        return o;
    }

    ConsoleTableCell & ConsoleTableRow::GetCell(size_t c)
    {
        BOOST_ASSERT(c < col);

        return (cells.get())[c];
    }

    ConsoleTable::ConsoleTable()
        : m_header(nullptr)
    {
        m_style = ConsoleTableStyle::GetStyleDefinition("default");
    }

    void ConsoleTable::SetStyle(const std::string &name)
    {
        m_style = ConsoleTableStyle::GetStyleDefinition(name);
        BOOST_ASSERT(m_style);
    }

    void ConsoleTable::SetStyle(ConsoleTableStylePtr style)
    {
        BOOST_ASSERT(style);
        m_style = style;
    }

    ConsoleTable& ConsoleTable::AddRow(const ConsoleTableSeparator &)
    {
        m_rows.push_back(MakeSharedPtr<ConsoleTableSeparator>());

        return *this;
    }

    void ConsoleTable::PreparePrint() const
    {
        m_col_width.clear();

        if (m_header)
        {
            for (size_t i = 0; i < m_header->col; ++i)
            {
                this->PrepareCell(m_header, i, m_style->GetHeaderFormat());
                m_col_width.push_back(m_header->GetCell(i).str.size());
            }
        }

        for (size_t i = 0; i < m_rows.size(); ++i)
        {
            if (!m_rows[i]->IsSeparator())
            {
                ConsoleTableRowPtr r = std::static_pointer_cast<ConsoleTableRow>(m_rows[i]);
                for (size_t j = 0; j < r->col; ++j)
                {
                   this->PrepareCell(r, j, m_style->GetCellFormat());

                   size_t width = r->GetCell(j).str.size();
                   if (j < m_col_width.size())
                   {
                       m_col_width[j] = 
                               m_col_width[j] < width ?
                               width :
                               m_col_width[j];
                   }
                   else
                   {
                       m_col_width.push_back(width);
                   }
                }
            }
        }
    }

    void ConsoleTable::PrepareCell(ConsoleTableRowPtr const& r, size_t col, ConsoleTableFormatConstPtr const& def_fmt) const
    {
        ConsoleTableCell &c = r->GetCell(col);
        ConsoleTableFormatConstPtr fmt = this->GetCellFormat(r, col, def_fmt);
        c.str = (boost::format(fmt->spec) % c.val).str();
    }

    ConsoleTableFormatConstPtr ConsoleTable::GetCellFormat(ConsoleTableRowPtr const&r, size_t col, ConsoleTableFormatConstPtr const& def_fmt) const
    {
        ConsoleTableFormatPtr fmt = nullptr;

        if (col < r->col)
        {
            if (r->GetCell(col).fmt)
                return r->GetCell(col).fmt;
        }

        if (col < m_header->col)
        {
            if (m_header->GetCell(col).fmt)
                return m_header->GetCell(col).fmt;
        }

        if (r->fmt)
            return r->fmt;

        return def_fmt;
    }

    void ConsoleTable::SetCellFormat(size_t row, size_t col, const ConsoleTableFormat &fmt)
    {
        if (row < m_rows.size())
        {
            if (!m_rows[row]->IsSeparator())
            {
                ConsoleTableRowPtr r = std::static_pointer_cast<ConsoleTableRow>(m_rows[row]);

                if (col < r->col)
                {
                    *(r->GetCell(col).fmt) = fmt;
                }
            }
        }
    }

    void ConsoleTable::SetRowFormat(size_t row, const ConsoleTableFormat &fmt)
    {
        if (row < m_rows.size())
        {
            if (!m_rows[row]->IsSeparator())
            {
                ConsoleTableRowPtr r = std::static_pointer_cast<ConsoleTableRow>(m_rows[row]);
                r->fmt = MakeSharedPtr<ConsoleTableFormat>(fmt);
            }
        }
    }

    void ConsoleTable::SetHeaderCellFormat(size_t col, const ConsoleTableFormat &fmt)
    {
        if (m_header)
        {
            if (col < m_header->col)
            {
                m_header->GetCell(col).fmt = MakeSharedPtr<ConsoleTableFormat>(fmt);
            }
        }
    }



}

