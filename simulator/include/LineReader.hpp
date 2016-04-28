#ifndef __SG_LINE_READER_HPP__
#define __SG_LINE_READER_HPP__

#include "Core.hpp"

#include <string>

#include "boost/circular_buffer.hpp"

#include "Utils.hpp"
#include "Singleton.hpp"

#define SG_CMD_HISTORY_LEN  100

namespace sg
{
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

    public:
        void Init(ConsoleWindowPtr & input_wnd, std::string const& prompt = "(Sim) ");
        std::string const& GetPrompt() const { return m_prompt; }

    public:

        std::string ReadLine();
        std::string ReadPreLine();

    public:
        void OnChar(ConsoleWindow const& wnd, int ch);
        void OnKey(ConsoleWindow const& wnd, int key);

    private:
        void NextLine();
        void NextLine(int row, uint t, uint b, uint l);

    private:

        typedef std::shared_ptr<char>   BufferType;

        std::string             m_prompt;
        ConsoleWindowPtr        m_input_wnd;
        BufferType              m_buffer;
        uint                    m_buf_len;
        bool                    m_finish;
        uint                    m_pos;
        uint                    m_len;
        int                     m_row;
        int                     m_col;
        boost::circular_buffer<std::string> m_hists;
        boost::circular_buffer<std::string>::reverse_iterator m_curr_hist;
    };

}




#endif

