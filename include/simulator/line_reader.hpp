#ifndef __LINE_READER_HPP__
#define __LINE_READER_HPP__

#include "core/core.hpp"
#include "core/core_utils.hpp"
#include "core/core_singleton.hpp"

#include <string>

#include "boost/circular_buffer.hpp"

#define SG_CMD_HISTORY_LEN  100

namespace sg
{
    class ConsoleWindow;

    class LineReader : public Singleton<LineReader>
    {
    public:
        LineReader()
            : m_input_wnd(nullptr)
            , m_buffer(nullptr)
            , m_buf_len(0)
            , m_finish(false)
            , m_pos(0)
            , m_len(0)
            , m_row(0)
            , m_col(0)
            , m_hists(SG_CMD_HISTORY_LEN)
        {
        }

       ~LineReader()
        {
            CORE_SAFE_DELETE_ARRAY(m_buffer);
        }

    public:
        void Init(ConsoleWindow *input_wnd, std::string const& prompt = "(Sim) ");
        std::string const& GetPrompt() const { return m_prompt; }

    public:

        std::string ReadLine();
        std::string ReadPreLine();

    public:
        void OnChar(ConsoleWindow const& wnd, int ch);
        void OnKey(ConsoleWindow const& wnd, int key);

    private:
        void NextLine();
        void NextLine(int row, uint32_t t, uint32_t b, uint32_t l);

    private:

        std::string             m_prompt;
        ConsoleWindow          *m_input_wnd;
        char*                   m_buffer;
        uint32_t                m_buf_len;
        bool                    m_finish;
        uint32_t                m_pos;
        uint32_t                m_len;
        int                     m_row;
        int                     m_col;
        boost::circular_buffer<std::string> m_hists;
        boost::circular_buffer<std::string>::reverse_iterator m_curr_hist;
    };

}




#endif

