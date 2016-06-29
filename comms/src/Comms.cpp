#include "Core.hpp"

#include <functional>

#ifdef SG_PLATFORM_LINUX
//    #ifndef _XOPEN_SOURCE
//        #define _XOPEN_SOURCE 600
//    #endif
//    #include <stdio.h>
//    #include <sys/types.h>
    #include <stdlib.h>
    #include <string.h>
    #include <fcntl.h>
//    #include <sys/select.h>
//    #include <sys/time.h>
    #include <termios.h>
    #include <unistd.h>
    #include <errno.h>
#else
    #include <Windows.h>
    #ifdef AddJob
        #undef AddJob
    #endif
#endif

#include "Utils.hpp"

#include "Comms.hpp"


namespace sg 
{

#ifdef SG_PLATFORM_WINDOWS
    class CommsOverlapped
    {
    public:
        CommsOverlapped()
            : m_ol({ 0 })
        {
        }

       ~CommsOverlapped()
        {
            this->Close();
        }

    public:
        bool Init()
        {
            if (!m_ol.hEvent)
            {
                m_ol.hEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
                if (!m_ol.hEvent)
                    return false;
            }

            return true;
        }

        bool Close()
        {
            if (m_ol.hEvent)
            {
                if (::CloseHandle(m_ol.hEvent))
                {
                    m_ol.hEvent = nullptr;
                    return true;
                }
                return false;
            }

            return true;
        }

        bool Reset()
        {
            return ::ResetEvent(m_ol.hEvent) == TRUE;
        }

        LPOVERLAPPED Get() { return &m_ol; }

    private:
        OVERLAPPED  m_ol;
    };

    namespace
    {
        CommsOverlapped s_rol;
    }
#endif

    Comms::Comms(const std::string &dev, CommsType type)
#ifdef SG_PLATFORM_LINUX
        : m_fd(-1)
#else
        : m_fd(INVALID_HANDLE_VALUE)
#endif
        , m_dev(dev)
        , m_init(false)
        , m_start(false)
        , m_type(type)
        //, m_resp_received(false)
        //, m_resp_finish(false)
        //, m_resp_timeout(true)
    {
    }

    Comms::~Comms()
    {
        Quit();
    }

    void Comms::Quit()
    {
        if (m_start)
        {
            COMMS_LOG("Comms is stopping...\n", CLL_Info);
            this->Stop();
            this->DoQuit();
        }

        if (m_init)
        {
            COMMS_LOG("Comms is shutting down...\n", CLL_Info);
#ifdef SG_PLATFORM_LINUX
            if (m_fd >= 0)
                close(m_fd);
#else
            if (m_fd != INVALID_HANDLE_VALUE)
                ::CloseHandle(m_fd);
#endif
            m_init = false;
        }

    }

    bool Comms::ChangeDev(std::string dev)
    {
        this->Quit();
        m_dev = dev;
        if (!this->Init())
        {
            COMMS_LOG(boost::format("Change device to %1% failed\n") % m_dev, CLL_Error);
            return false;
        }

        this->Start();
        return true;
    }

    bool Comms::OpenDevFile() 
    {
#ifdef SG_PLATFORM_LINUX
        int fd = -1;
        // Ref https://en.wikibooks.org/wiki/Serial_Programming/termios
        fd = open(m_dev.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

        if (fd < 0)
        {
            COMMS_LOG(boost::format("%d\n") % strerror(errno), CLL_Error);
            return false;
        }
        else
        {
            m_fd = fd;

            COMMS_LOG(boost::format("Comms open device %1%\n") % m_dev, CLL_Info);
            struct termios oldtio, newtio;
            tcgetattr(fd, &oldtio);
            memset(&newtio, 0, sizeof(newtio));

            newtio.c_cflag |= B19200; // baud rate
            newtio.c_cflag |= CS8; // character size
            newtio.c_iflag |= IGNPAR; // ignore framing errors and parity errors
            newtio.c_cflag |= PARENB | CMSPAR; // enable parity generation on output and parity checking for input/use "stick" parity [not in POSIX]
            newtio.c_iflag |= IGNBRK;// ignore break condition on input
            newtio.c_oflag = 0;
            newtio.c_lflag = 0;
            newtio.c_cflag |= CLOCAL | CREAD; // ignore modem control lines/enable receiver

            tcflush(fd, TCIFLUSH);
            tcsetattr(fd, TCSANOW, &newtio);
        }

        return true;
#else
        HANDLE h = ::CreateFileA(m_dev.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);

        if (h == INVALID_HANDLE_VALUE)
        {
            COMMS_LOG(boost::format("%||") % ::GetLastError(), CLL_Error);
            return false;
        }
        else
        {
            COMMS_LOG(boost::format("Comms open device %1%\n") % m_dev, CLL_Info);

            DCB dcb = { 0 };
            dcb.DCBlength = sizeof(DCB);

            if (!::GetCommState(h, &dcb))
            {
                COMMS_LOG(boost::format("Failed to get Comm State: %||\n") % ::GetLastError(), CLL_Error);
                return false;
            }

            dcb.BaudRate = CBR_19200;
            //dcb.fParity = TRUE;
            dcb.Parity = EVENPARITY;
            dcb.StopBits = ONESTOPBIT;
            //dcb.ByteSize = DATABITS_8;
            dcb.ByteSize = 8;
            dcb.fDtrControl = DTR_CONTROL_DISABLE;
            dcb.fRtsControl = RTS_CONTROL_DISABLE;

            if (!::SetCommState(h, &dcb))
            {
                COMMS_LOG(boost::format("Failed to set Comm State: %||\n") % ::GetLastError(), CLL_Error);
                return false;
            }

            COMMTIMEOUTS to;
            to.ReadIntervalTimeout = MAXWORD;
            to.ReadTotalTimeoutMultiplier = 0;
            to.ReadTotalTimeoutConstant = 0;
            to.WriteTotalTimeoutConstant = 0;
            to.WriteTotalTimeoutMultiplier = 0;

            if (!::SetCommTimeouts(h, &to))
            {
                COMMS_LOG(boost::format("Failed to set Comm timeout: %||\n") % ::GetLastError(), CLL_Error);
                return false;
            }

            m_fd = h;
        }

        return true;
#endif
    }

    bool Comms::Init()
    {
        if (!m_init)
        {
            if (this->OpenDevFile())
            {
                m_init = true;
                this->DoInit();
                return true;
            }
        }

        return m_init;
    }

    void Comms::Start()
    {
        if (!m_start)
        {
            COMMS_LOG("Comms start\n", CLL_Info);
            m_start = true;
            this->StartResponseThread();
            this->StartCheckTimeoutThread();
            this->DoStart();
        }
    }

    void Comms::StartResponseThread()
    {
        m_reader = std::thread(std::bind(&Comms::ReceiveResponse, this));
    }

    void Comms::StartCheckTimeoutThread()
    {
        m_checker = std::thread(std::bind(&Comms::CheckCommsTimeout, this));
    }

    void Comms::Stop()
    {
        if (m_start)
        {
            m_start = false;
            this->StopResponseThread();
            this->StopCheckTimeoutThread();
            this->DoStop();
        }
    }

    void Comms::StopResponseThread()
    {
        // In c++ 11, if the thread is joinable when destory,
        // then std::terminate is called, while boost thread
        // will call detach automatically.
        //m_reader.detach();
        if (m_reader.joinable())
            m_reader.join();
    }

    void Comms::StopCheckTimeoutThread()
    {
        if (m_checker.joinable())
            m_checker.join();
    }

    void Comms::CheckCommsTimeout()
    {
        this->DoCheckCommsTimeout();
    }

    void Comms::ReceiveResponse()
    {
#ifdef SG_PLATFORM_LINUX
        fd_set  read_fds;
        int ret;
        struct timeval  timeout;

        while(m_start)
        {
            FD_ZERO(&read_fds);
            FD_SET(m_fd, &read_fds);

            timeout.tv_sec = 0;
            timeout.tv_usec = TIMEOUT_USEC;

            ret = select(m_fd + 1, &read_fds, nullptr, nullptr, &timeout);
            if (ret <= 0) // TODO : error or no device ready
            {
                continue;
            }

            if (FD_ISSET(m_fd, &read_fds))
            {
                Read();
            }
        }
#else
        if (!::SetCommMask(m_fd, EV_RXCHAR))
        {
            COMMS_LOG("Failed to set Comm Mask, response receiver stop working\n", CLL_Error);
            return;
        }

        if (!s_rol.Init())
        {
            COMMS_LOG("Failed to initialize Comm Event, response receiver stop working\n", CLL_Error);
            return;
        }

        DWORD commev;
        DWORD wres;
        bool waiting = false;

        while (m_start)
        {
            if (!waiting)
            {
                if (!::WaitCommEvent(m_fd, &commev, s_rol.Get()))
                {
                    if (::GetLastError() == ERROR_IO_PENDING)
                        waiting = true;
                    else
                        continue;
                }
                else
                {
                    this->Read();
                }
            }

            if (waiting)
            {
                wres = ::WaitForSingleObject(s_rol.Get()->hEvent, TIMEOUT_USEC);
                switch (wres)
                {
                case WAIT_OBJECT_0:
                    this->Read();
                    s_rol.Reset();
                    waiting = false;
                    break;
                case WAIT_TIMEOUT:
                    break;
                default:
                    if (s_rol.Close())
                    {
                        if (s_rol.Init())
                            break;
                    }
                    COMMS_LOG("Comm Event Error, response receiver stop working\n", CLL_Error);
                    return;
                }
            }
        }
#endif
    }

    void Comms::Read()
    {
        static uint8_t  *ptr;
        static uint8_t  msg_buf[BUFF_SIZE];
        static bool first_time = true;
        static int length;

        if (first_time)
        {
            length = 0;
            ptr = msg_buf;
            std::memset(msg_buf, 0, sizeof(msg_buf));
            first_time = false;
        }

#ifdef SG_PLATFORM_LINUX
        int ret;

        ret = read(m_fd, ptr, 1);
        ptr += ret;
        length += ret;

        //if (length && !m_resp_received)
        //{
            //std::unique_lock<std::mutex> lock(m_response);
            //m_resp_received = true;
            //if (!m_resp_timeout)
            //{
            //    m_response_cond.notify_one();
            //}
        //}

        if (length && this->IsPacketComplete(msg_buf, length))
        {
            //{
            //    std::unique_lock<std::mutex> lock(m_response);
            //    m_resp_finish = true;
            //    m_response_cond.notify_one();
            //}
            //unique_lock<mutex> lock(m_response);
            //if (!m_resp_timeout)
            //{
                if (this->IsCRCValid(msg_buf, length))
                {
                    this->HandlePacket(msg_buf, length);
                }
            //}

            ptr = msg_buf;
            length = 0;
            std::memset(msg_buf, 0, BUFF_SIZE);

           //m_resp_received = false;
        }
#else
        DWORD rd = 0;

        do
        {
            if (::ReadFile(m_fd, ptr, sizeof(msg_buf) - length, &rd, s_rol.Get()))
            {
                COMMS_START_PRINT_BLOCK();

                for (DWORD i = 0; i < rd; ++i)
                {
                    ++ptr;
                    ++length;

                    COMMS_PRINT_BLOCK(boost::format(" %|02X| ") % (unsigned int)(*(ptr -1)));

                    if (this->IsPacketComplete(msg_buf, length))
                    {
                        if (this->IsCRCValid(msg_buf, length))
                        {
                            this->HandlePacket(msg_buf, length);
                        }

                        length = rd - i - 1;
                        if (length)
                        {
                            std::memcpy(msg_buf, ptr, length);
                        }

                        ptr = msg_buf + length;
                        std::memset(ptr, 0, sizeof(msg_buf) - length);
                    }
                }

                COMMS_END_PRINT_BLOCK();
            }
        } while (rd);
#endif
    }

    bool Comms::IsPacketComplete(uint8_t [], int /*length*/)
    {
        return true;
    }

    bool Comms::IsCRCValid(uint8_t [], int /*length*/)
    {
        return true;
    }

    void Comms::HandlePacket(uint8_t [], int /*length*/)
    {

    }

    void Comms::SendPacket(uint8_t buf[], int length)
    {
#ifdef SG_PLATFORM_LINUX
        if (m_fd >= 0)
        {
            termios tio;
            tcgetattr(m_fd, &tio);

            tio.c_cflag |= PARENB | CMSPAR | PARODD;
            tcsetattr(m_fd, TCSADRAIN, &tio);

            write(m_fd, &buf[0], 1);

            tio.c_cflag &= ~PARODD;
            tcsetattr(m_fd, TCSADRAIN, &tio);
            write(m_fd, &buf[1], length - 1);

            tcdrain(m_fd);
        }
#else
        if (m_fd != INVALID_HANDLE_VALUE)
        {
//            COMMS_START_PRINT_BLOCK();
//            COMMS_PRINT_BLOCK("Sending Package: ");
//            for (int i = 0; i < length; ++i)
//            {
//                COMMS_PRINT_BLOCK(boost::format("%|02X| ") % (unsigned int)(buf[i]));
//            }
//            COMMS_PRINT_BLOCK("\n");
//            COMMS_END_PRINT_BLOCK();

            CommsOverlapped wol;
            if (!wol.Init())
            {
                COMMS_LOG("Failed to send package, cannot create Comm Event", CLL_Error);
                return;
            }

            DCB dcb = { 0 };
            dcb.DCBlength= sizeof(DCB);

            if (!::GetCommState(m_fd, &dcb))
            {
                COMMS_LOG("Failed to send package, cannot get Comm State\n", CLL_Error);
                return;
            }

            dcb.Parity = MARKPARITY;

            // TODO: this is ugly but necessary for windows, I don't know why now.
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            if (!::SetCommState(m_fd, &dcb))
            {
                COMMS_LOG("Failed to send package, cannot change Comm Parity to Mark\n", CLL_Error);
                return;
            }

            bool ok = false;
            DWORD written;
            if (!::WriteFile(m_fd, buf, 1, &written, wol.Get()))
            {
                if (::GetLastError() != ERROR_IO_PENDING)
                {
                    COMMS_LOG("Failed to send Mark Byte\n", CLL_Error);
                }
                else
                {
                    if (!::GetOverlappedResult(m_fd, wol.Get(), &written, TRUE))
                    {
                        COMMS_LOG(boost::format("Error happened when send package: %||\n") % ::GetLastError(), CLL_Error);
                    }
                    else
                    {
                        ok = true;
                    }
                }
            }
            else
            {
                ok = true;
            }

            if (ok)
            {
                wol.Reset();

                // TODO: this is ugly but necessary for windows, I don't know why now.
                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                dcb.Parity = SPACEPARITY;
                if (::SetCommState(m_fd, &dcb))
                {
                    if (!::WriteFile(m_fd, &buf[1], length - 1, &written, wol.Get()))
                    {
                        if (::GetLastError() == ERROR_IO_PENDING)
                        {
                            if (!::GetOverlappedResult(m_fd, wol.Get(), &written, TRUE))
                            {
                                COMMS_LOG(boost::format("Error happened when send package: %||\n") % ::GetLastError(), CLL_Error);
                            }
                            else
                            {
                                if (written != (DWORD)(length - 1))
                                {
                                    COMMS_LOG("Write operation time out when send package\n", CLL_Error);
                                }
                            }
                        }
                        else
                        {
                            COMMS_LOG("Failed to send Space Bytes\n", CLL_Error);
                        }
                    }
                    else
                    {
                        if (written != (DWORD)(length - 1))
                        {
                            COMMS_LOG("Write operation time out when send package\n", CLL_Error);
                        }
                    }
                }
                else
                {
                    COMMS_LOG("Failed to send package, cannot change Comms Parity to Space\n", CLL_Error);
                }
            }
        }
#endif
        else
        {
            COMMS_LOG("Comms send package failed, invalid device\n", CLL_Error);
        }
    }

    void CommsPacketHandler::RunSubStage()
    {
        while (1) 
        {
            {
                std::unique_lock<std::mutex> lock(m_sub);
                while(!m_job_num) // TODO : can we use lambda ?
                {
                    m_sub_cond.wait(lock);
                }

                if (m_job_num)
                {
                    --m_job_num;
                }
            }

            this->DoSubStage();
        }
    }

    void CommsPacketHandler::StartSubStageThread()
    {
        if (m_has_sub_stage)
        {
            m_job = std::thread(std::bind(&CommsPacketHandler::RunSubStage, this));
        }
    }

    void CommsPacketHandler::StopSubStageThread()
    {
        if (m_has_sub_stage)
        {
            m_job.detach();
        }
    }

    void CommsPacketHandler::AddJob()
    {
        if (m_has_sub_stage)
        {
            std::unique_lock<std::mutex> lock(m_sub);

            ++m_job_num;

            m_sub_cond.notify_one();
        }
    }

}


