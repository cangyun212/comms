#include "Core.hpp"

#include "Utils.hpp"
#include "Comms.hpp"
#include "CmdCompletion.hpp"
#include "LineReader.hpp"

namespace sg
{

    void LineReader::Init(ConsoleWindowPtr &input_wnd, std::string const& prompt)
    {

        BOOST_ASSERT(input_wnd);

        m_input_wnd = input_wnd;
        m_prompt = prompt;

        uint t, b, l, r;
        m_input_wnd->GetMargin(t, b, l, r);

        this->NextLine(m_input_wnd->Row(), t, b, l);

        // TODO
        m_buf_len = m_input_wnd->Width() - l - r - static_cast<uint>(m_prompt.size());
        m_buffer = MakeArraryPtr(char, m_buf_len);

        m_input_wnd->CharEvent().connect(
            std::bind(&LineReader::OnChar, this, std::placeholders::_1, std::placeholders::_2));
        m_input_wnd->KeyEvent().connect(
            std::bind(&LineReader::OnKey, this, std::placeholders::_1, std::placeholders::_2));

    }

    std::string LineReader::ReadLine()
    {

        std::memset(m_buffer.get(), '\0', m_buf_len);

        m_finish = false;
        m_pos = m_len = 0;
        m_input_wnd->CurCoord(m_row, m_col);
        m_curr_hist = m_hists.rbegin();

        while (!m_finish)
        {
            m_buffer.get()[m_len] = ' ';
            m_input_wnd->AddStrTo(m_row, m_col, m_buffer.get(), m_len + 1);
            m_input_wnd->MoveTo(m_row, m_col + m_pos);
            m_input_wnd->Render();

            ConsoleEvent event;
            NextEvent(*m_input_wnd, event);

            m_input_wnd->MsgProc(event);
        }

        this->NextLine();

        m_buffer.get()[m_len] = '\0';

        std::string line(m_buffer.get());

        if (!line.empty())
        {
            if (!m_hists.empty())
            {
                if (m_hists.back() != line)
                {
                    m_hists.push_back(line);
                }
            }
            else
            {
                m_hists.push_back(line);
            }
        }

        m_curr_hist = m_hists.rbegin();

        return line;

    }

    void LineReader::OnChar(const ConsoleWindow &wnd, int ch)
    {
        SG_UNREF_PARAM(wnd);

        if (m_pos < m_buf_len - 1)
        {
            memmove(m_buffer.get() + m_pos + 1, m_buffer.get() + m_pos, m_len - m_pos);
            m_buffer.get()[m_pos++] = static_cast<char>(ch);
            ++m_len;
        }
        else
        {
            m_input_wnd->Beep();
        }
    }

    void LineReader::OnKey(const ConsoleWindow &wnd, int key)
    {
        SG_UNREF_PARAM(wnd);

        switch(key)
        {
        case SG_CONSOLE_KEY_ENTER:
        case '\n':
        case '\r':
            m_finish = true;
            break;
        case SG_CONSOLE_KEY_LEFT:
            if (m_pos > 0) --m_pos;
            break;
        case SG_CONSOLE_KEY_RIGHT:
            if (m_pos < m_len) ++m_pos;
            break;
        case '\b':
            if (m_pos > 0)
            {
                memmove(m_buffer.get() + m_pos - 1, m_buffer.get() + m_pos, m_len - m_pos);
                --m_pos;
                --m_len;
            }
            break;
        case SG_CONSOLE_KEY_DC:
            if (m_pos < m_len)
            {
                memmove(m_buffer.get() + m_pos, m_buffer.get() + m_pos + 1, m_len - m_pos - 1);
                --m_len;
            }
            break;
        case SG_CONSOLE_KEY_UP:
            if (m_curr_hist != m_hists.rend())
            {
                m_input_wnd->ClearHLine(m_row, m_col, m_len);
                memmove(m_buffer.get(), m_curr_hist->c_str(), m_curr_hist->size());
                m_pos = m_len = static_cast<uint>(m_curr_hist->size());
                ++m_curr_hist;
                if (m_curr_hist == m_hists.rend())
                    --m_curr_hist;
            }
            break;
        case SG_CONSOLE_KEY_DOWN:
            if (!m_hists.empty())
            {
                if (m_curr_hist != m_hists.rbegin())
                {
                    --m_curr_hist;
                    m_input_wnd->ClearHLine(m_row, m_col, m_len);
                    memmove(m_buffer.get(), m_curr_hist->c_str(), m_curr_hist->size());
                    m_pos = m_len = static_cast<uint>(m_curr_hist->size());
                    if (m_curr_hist == m_hists.rbegin())
                        ++m_curr_hist;
                }
            }
            break;
        case '\t':
        {
            std::string cmdStr(m_buffer.get(), m_len);

            std::vector<std::string> matchStrings;

            std::string::size_type pos = CmdCompletion::Instance().doCmdCompletion(cmdStr, matchStrings);

            if(matchStrings.size() > 1)
            {
                std::string res;

                for(auto const &tmpStr : matchStrings)
                {
                    res.append("  ").append(tmpStr);
                }

                NextLine();
                m_input_wnd->CurCoord(m_row, m_col);
                m_input_wnd->ClearHLine(m_row, 0, 5);
                m_input_wnd->AddStrTo(m_row, 0, res.c_str(), static_cast<int>(res.size()));
                NextLine();
                m_input_wnd->CurCoord(m_row, m_col);
            }
            else if(matchStrings.size() == 1)
            {
                std::string res;

                res = cmdStr;

                res.append(matchStrings[0], pos, matchStrings[0].size());

                if(res.size() > m_len)
                {
                    memcpy(m_buffer.get(), res.c_str(), res.size());
                    m_len = static_cast<uint>(res.size());
                    m_pos = static_cast<uint>(res.size());
                }
            }
        }
            break;

        default:
            break;
        }
    }

    void LineReader::NextLine()
    {
        int row = m_input_wnd->Row();

        uint t, b, l, r;
        m_input_wnd->GetMargin(t, b, l, r);

        this->NextLine(row, t, b, l);
    }

    void LineReader::NextLine(int row, uint t, uint b, uint l)
    {
        uint height = m_input_wnd->Height();

        uint to_row = row + 1;
        uint to_col = l + static_cast<uint>(m_prompt.size());

        if (to_row >= (height - b)) // reach to bottom
        {
            // make the window scroll
            //m_input_wnd->AddStrTo(row, l + m_prompt.size() + m_len, "\n", 1);
            m_input_wnd->Scroll(1);
            to_row = row;
        }
        else if (to_row < t) // above the top margin
        {
            to_row = t;
        }

        if (!m_prompt.empty())
        {
            m_input_wnd->AddStrTo(static_cast<int>(to_row), l, m_prompt);
        }

        m_input_wnd->MoveTo(to_row, to_col);
        m_input_wnd->Render();
    }

    std::string LineReader::ReadPreLine()
    {
        if (!m_hists.empty())
        {
            return *m_curr_hist;
        }

        return std::string();
    }
}





