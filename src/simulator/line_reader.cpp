#include "core/core.hpp"
#include "core/core_console.hpp"

#ifdef CORE_DEBUG
    #include <readline/readline.h>
    #include <readline/history.h>
#endif

#include "simulator/line_reader.hpp"

namespace sg
{

    void LineReader::Init(ConsoleWindow *input_wnd, std::string const& prompt)
    {
#ifdef CORE_DEBUG
        if (input_wnd)
        {
#endif
        BOOST_ASSERT(input_wnd);
        m_input_wnd = input_wnd;
        m_prompt = prompt;

        uint32_t t, b, l, r;
        m_input_wnd->GetMargin(t, b, l, r);

        NextLine(m_input_wnd->Row(), t, b, l);

        // TODO
        m_buf_len = m_input_wnd->Width() - l - r - m_prompt.size();
        m_buffer = new char[m_buf_len];

        m_input_wnd->CharEvent().connect(
                    bind(&LineReader::OnChar, this, placeholders::_1, placeholders::_2));
        m_input_wnd->KeyEvent().connect(
                    bind(&LineReader::OnKey, this, placeholders::_1, placeholders::_2));
#ifdef CORE_DEBUG
        }
        else
        {
            m_prompt = prompt;
        }
#endif
    }

    std::string LineReader::ReadLine()
    {
#ifdef CORE_DEBUG
        if (m_input_wnd)
        {
#endif
        memset(m_buffer, '\0', m_buf_len);
        m_finish = false;
        m_pos = m_len = 0;
        m_input_wnd->CurCoord(m_row, m_col);
        m_curr_hist = m_hists.rbegin();

        while (!m_finish)
        {
            m_buffer[m_len] = ' ';
            m_input_wnd->AddStrTo(m_row, m_col, m_buffer, m_len + 1);
            m_input_wnd->MoveTo(m_row, m_col + m_pos);
            m_input_wnd->Render();

            ConsoleEvent event;
            NextEvent(*m_input_wnd, event);

            m_input_wnd->MsgProc(event);
        }

        NextLine();

        m_buffer[m_len] = '\0';

        std::string line(m_buffer);

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
#ifdef CORE_DEBUG
        }
        else
        {
            std::string line;
            char *p = readline(m_prompt.c_str());
            if (p)
            {
                line = p;
                free(p);
                add_history(line.c_str());
            }
            return line;
        }
#endif
    }

    void LineReader::OnChar(const ConsoleWindow &wnd, int ch)
    {
        if (m_pos < m_buf_len - 1)
        {
            memmove(m_buffer + m_pos + 1, m_buffer + m_pos, m_len - m_pos);
            m_buffer[m_pos++] = ch;
            ++m_len;
        }
        else
        {
            m_input_wnd->Beep();
        }
    }

    void LineReader::OnKey(const ConsoleWindow &wnd, int key)
    {
        switch(key)
        {
        case CONSOLE_KEY_ENTER:
        case '\n':
        case '\r':
            m_finish = true;
            break;
        case CONSOLE_KEY_LEFT:
            if (m_pos > 0) --m_pos;
            break;
        case CONSOLE_KEY_RIGHT:
            if (m_pos < m_len) ++m_pos;
            break;
        case CONSOLE_KEY_BACKSPACE:
            if (m_pos > 0)
            {
                memmove(m_buffer + m_pos - 1, m_buffer + m_pos, m_len - m_pos);
                --m_pos;
                --m_len;
            }
            break;
        case CONSOLE_KEY_DC:
            if (m_pos < m_len)
            {
                memmove(m_buffer + m_pos, m_buffer + m_pos + 1, m_len - m_pos - 1);
                --m_len;
            }
            break;
        case CONSOLE_KEY_UP:
            if (m_curr_hist != m_hists.rend())
            {
                m_input_wnd->ClearHLine(m_row, m_col, m_len);
                memmove(m_buffer, m_curr_hist->c_str(), m_curr_hist->size());
                m_pos = m_len = m_curr_hist->size();
                ++m_curr_hist;
                if (m_curr_hist == m_hists.rend())
                    --m_curr_hist;
            }
            break;
        case CONSOLE_KEY_DOWN:
            if (!m_hists.empty())
            {
                if (m_curr_hist != m_hists.rbegin())
                {
                    --m_curr_hist;
                    m_input_wnd->ClearHLine(m_row, m_col, m_len);
                    memmove(m_buffer, m_curr_hist->c_str(), m_curr_hist->size());
                    m_pos = m_len = m_curr_hist->size();
                    if (m_curr_hist == m_hists.rbegin())
                        ++m_curr_hist;
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

        uint32_t t, b, l, r;
        m_input_wnd->GetMargin(t, b, l, r);

        this->NextLine(row, t, b, l);
    }

    void LineReader::NextLine(int row, uint32_t t, uint32_t b, uint32_t l)
    {
        uint32_t height = m_input_wnd->Height();

        int to_row = row + 1;
        int to_col = l + m_prompt.size();

        if ((uint32_t)to_row >= height - b) // reach to bottom
        {
            // make the window scroll
            //m_input_wnd->AddStrTo(row, l + m_prompt.size() + m_len, "\n", 1);
            m_input_wnd->Scroll(1);
            to_row = row;
        }
        else if ((uint32_t)to_row < t) // above the top margin
        {
            to_row = t;
        }

        if (!m_prompt.empty())
        {
            m_input_wnd->AddStrTo(to_row, l, m_prompt);
        }

        m_input_wnd->MoveTo(to_row, to_col);
        m_input_wnd->Render();
    }

    std::string LineReader::ReadPreLine()
    {
#ifdef CORE_DEBUG
        if (m_input_wnd)
        {
#endif
        if (!m_hists.empty())
        {
            return *m_curr_hist;
        }

        return std::string();
#ifdef CORE_DEBUG
        }
        else
        {
            HIST_ENTRY *hist = previous_history();
            if (hist)
            {
                if (hist->line)
                {
                    return std::string(hist->line);
                }
            }
            return std::string();
        }
#endif
    }
}





