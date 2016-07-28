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

#define _dev    ((CommsSerialPort*)m_dev)

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

        LPOVERLAPPED Get() { return &m_ol;  }

    private:
        OVERLAPPED  m_ol;
    };
#endif

    class CommsSerialPort
    {
    public:
        enum EParity
        {
            None,
            Odd,
            Even,
            Mark,
            Space
        };

    public:
        CommsSerialPort(bool multidrop = true)
#ifdef SG_PLATFORM_LINUX
            : m_fd(-1)
#else
            : m_fd(INVALID_HANDLE_VALUE)
#endif // SG_PLATFORM_LINUX
            , m_multidrop(multidrop)

        {
        }

        ~CommsSerialPort()
        {
            this->Close();
        }

    public:
        bool Open(std::string const& port, unsigned int baud, EParity parity)
        {
            if (this->Close())
            {
#ifdef SG_PLATFORM_LINUX
                int fd = -1;
                fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

                if (fd < 0)
                {
                    COMMS_LOG(boost::format("Failed open device: %||\n") % strerror(errno), CLL_Error);
                    return false;
                }
                else
                {
                    struct termios oldtio, newtio;
                    tcgetattr(fd, &oldtio);
                    memset(&newtio, 0, sizeof(newtio));

                    newtio.c_cflag |= this->Baud(baud); // baud rate
                    newtio.c_cflag |= CS8; // character size
                    newtio.c_iflag |= IGNPAR; // ignore framing errors and parity errors
                    newtio.c_cflag |= this->Parity(parity); // enable parity generation on output and parity checking for input/use "stick" parity [not in POSIX]
                    newtio.c_iflag |= IGNBRK;// ignore break condition on input
                    newtio.c_oflag = 0;
                    newtio.c_lflag = 0;
                    newtio.c_cflag |= CLOCAL | CREAD; // ignore modem control lines/enable receiver

                    tcflush(fd, TCIFLUSH);
                    tcsetattr(fd, TCSANOW, &newtio);

                    COMMS_LOG(boost::format("Open device %1%\n") % port, CLL_Info);
                    m_fd = fd;
                }
#else
                HANDLE h = ::CreateFileA(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);

                if (h == INVALID_HANDLE_VALUE)
                {
                    COMMS_LOG(boost::format("Failed open device: %||\n") % ::GetLastError(), CLL_Error);
                    return false;
                }
                else
                {
#define _SG_SER_BUFF_SIZE   4096
                    if (::SetupComm(h, _SG_SER_BUFF_SIZE, _SG_SER_BUFF_SIZE) == TRUE)
                    {
                        DCB dcb = { 0 };
                        dcb.DCBlength = sizeof(DCB);

                        if (::GetCommState(h, &dcb) != TRUE)
                        {
                            COMMS_LOG(boost::format("Failed get serial state: %||\n") % ::GetLastError(), CLL_Error);
                            return false;
                        }

                        dcb.BaudRate = this->Baud(baud);
                        dcb.fBinary = 1;
                        dcb.fParity = 0;
                        dcb.Parity = (BYTE)this->Parity(parity);
                        dcb.StopBits = ONESTOPBIT;
                        dcb.ByteSize = 8;
                        dcb.fDtrControl = DTR_CONTROL_DISABLE;
                        dcb.fRtsControl = RTS_CONTROL_DISABLE;
                        dcb.fOutxCtsFlow = 0;
                        dcb.fOutxDsrFlow = 0;
                        dcb.fDsrSensitivity = 0;
                        dcb.fTXContinueOnXoff = 0;
                        dcb.fOutX = 0;
                        dcb.fInX = 0;
                        dcb.fErrorChar = 0;
                        dcb.fNull = 0;
                        dcb.fAbortOnError = 0;

                        if (::SetCommState(h, &dcb) != TRUE)
                        {
                            COMMS_LOG(boost::format("Failed set serial state: %||\n") % ::GetLastError(), CLL_Error);
                            return false;
                        }

                        COMMTIMEOUTS to;
                        to.ReadIntervalTimeout = MAXDWORD;
                        to.ReadTotalTimeoutMultiplier = 0;
                        to.ReadTotalTimeoutConstant = 0;
                        to.WriteTotalTimeoutConstant = SG_COMM_OP_TIMEOUT;
                        to.WriteTotalTimeoutMultiplier = 0;

                        if (::SetCommTimeouts(h, &to) != TRUE)
                        {
                            COMMS_LOG(boost::format("Failed set serial timeout: %||\n") % ::GetLastError(), CLL_Error);
                            return false;
                        }

                        if (::SetCommMask(h, EV_RXCHAR) != TRUE)
                        {
                            COMMS_LOG(boost::format("Failed clear serial mask: %||\n") % ::GetLastError(), CLL_Error);
                            return false;
                        }

                        if (::PurgeComm(h, PURGE_TXCLEAR | PURGE_RXCLEAR) != TRUE)
                        {
                            COMMS_LOG(boost::format("Failed discard all data in input/output serial buffer: %||\n") % ::GetLastError(),
                                CLL_Error);
                            return false;
                        }

                        if (!m_ol.Init())
                        {
                            COMMS_LOG("Failed to initialize Comm Event\n", CLL_Error);
                            return false;
                        }

                        COMMS_LOG(boost::format("Open device %||\n") % port, CLL_Info);
                        m_fd = h;
                    }
                    else
                    {
                        COMMS_LOG(boost::format("Failed setup serial port buffer: %||\n") % ::GetLastError(), CLL_Error);
                        return false;
                    }
                }
#endif
                return true;
            }

            return false;
        }

        bool Close()
        {
#ifdef SG_PLATFORM_LINUX
            if (m_fd >= 0)
            {
                if (close(m_fd) == 0)
                {
                    m_fd = -1;
                    return true;
                }
                else
                {
                    COMMS_LOG(boost::format("Failed close device: %||\n") % strerror(errno), CLL_Error);
                    return false;
                }
            }

#else
            if (m_fd != INVALID_HANDLE_VALUE)
            {
                if (::CloseHandle(m_fd))
                {
                    m_fd = INVALID_HANDLE_VALUE;
                    if (!m_ol.Close())
                    {
                        COMMS_LOG("Failed to close Comm Event\n", CLL_Error);
                        return false;
                    }
                    return true;
                }
                else
                {
                    COMMS_LOG(boost::format("Failed close device: %||\n") % ::GetLastError(), CLL_Error);
                    return false;
                }
            }

#endif // SG_PLATFORM_LINUX

            return true;
        }

        bool WaitForRead()
        {
#ifdef SG_PLATFORM_LINUX
            if (m_fd >= 0)
            {
                fd_set read_fds;

                FD_ZERO(&read_fds);
                FD_SET(m_fd, &read_fds);

                struct timeval  timeout;

                timeout.tv_sec = SG_COMM_OP_TIMEOUT;
                timeout.tv_usec = 0;

                int ret = select(m_fd + 1, &read_fds, nullptr, nullptr, &timeout);

                if (ret > 0)
                {
                    if (FD_ISSET(m_fd, &read_fds))
                        return true;
                }

                return false;
            }
#else
            if (m_fd != INVALID_HANDLE_VALUE)
            {
                static bool waiting = false;

                DWORD commev;
                if (!waiting)
                {
                    if (!::WaitCommEvent(m_fd, &commev, m_ol.Get()))
                    {
                        if (::GetLastError() == ERROR_IO_PENDING)
                            waiting = true;
                        else
                            return false;
                    }
                    else
                    {
                        return true;
                    }
                }

                if (waiting)
                {
                    DWORD res = ::WaitForSingleObject(m_ol.Get()->hEvent, SG_COMM_OP_TIMEOUT);
                    switch (res)
                    {
                    case WAIT_OBJECT_0:
                        m_ol.Reset();
                        waiting = false;
                        return true;
                    case WAIT_TIMEOUT:
                        return false;
                    default:
                        if (m_ol.Close())
                        {
                            if (m_ol.Init())
                                return false;
                        }
                        COMMS_LOG("Error happened when waiting for reading buffer\n", CLL_Error);
                        return false;
                    }
                }
            }
#endif
            return false;
        }

        unsigned int Read(uint8_t *buffer, unsigned int length)
        {
#ifdef SG_PLATFORM_LINUX
            if (m_fd >= 0)
            {
                int ret = read(m_fd, buffer, length);

                if (ret > 0)
                    return (unsigned int)(ret);
            }

            return 0;
#else
            DWORD ret = 0;
            if (m_fd != INVALID_HANDLE_VALUE)
            {
                ::ReadFile(m_fd, buffer, length, &ret, m_ol.Get());
            }

            return (unsigned int)(ret);
#endif
        }

        bool Write(uint8_t *buffer, unsigned int length)
        {
#ifdef SG_PLATFORM_LINUX
            if (m_fd >= 0)
            {
#else
            if (m_fd != INVALID_HANDLE_VALUE)
            {
#endif
                if (m_multidrop)
                    return (this->WriteMultidrop(buffer, length) == length);
                else
                    return (this->WriteNormal(buffer, length) == length);
            }

            return false;
        }

    private:
        unsigned int WriteMultidrop(uint8_t *buffer, unsigned int length)
        {
#ifdef SG_PLATFORM_LINUX
            termios tio;
            tcgetattr(m_fd, &tio);

            tio.c_cflag |= PARENB | CMSPAR | PARODD;
            tcsetattr(m_fd, TCSADRAIN, &tio);

            write(m_fd, &buffer[0], 1);

            tio.c_cflag &= ~PARODD;
            tcsetattr(m_fd, TCSADRAIN, &tio);
            write(m_fd, &buffer[1], length - 1);

            tcdrain(m_fd);

            return length;
#else
            DCB dcb = { 0 };
            dcb.DCBlength = sizeof(DCB);
            ::GetCommState(m_fd, &dcb);

            dcb.Parity = MARKPARITY;
            ::SetCommState(m_fd, &dcb);

            if (this->WriteNormal(&buffer[0], 1) == 1)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));

                ::FlushFileBuffers(m_fd);

                dcb.Parity = SPACEPARITY;
                ::SetCommState(m_fd, &dcb);

                return this->WriteNormal(&buffer[1], length - 1) + 1;
            }
#endif // SG_PLATFORM_LINUX

            return 0;
        }

        unsigned int WriteNormal(uint8_t *buffer, unsigned int length)
        {
#ifdef SG_PLATFORM_LINUX
            write(m_fd, buffer, length);
            tcdrain(m_fd);
            return length;
#else
            CommsOverlapped ol;
            if (!ol.Init())
                return 0;

            DWORD ret = 0, send = length;
            unsigned int retry = 0;
            bool quit = false;

            while (!quit)
            {
                if (!::WriteFile(m_fd, buffer, send, &ret, ol.Get()))
                {
                    if (::GetLastError() == ERROR_IO_PENDING)
                    {
                        if (::GetOverlappedResult(m_fd, ol.Get(), &ret, TRUE));
                        else
                            return 0;
                    }
                    else
                        return 0;
                }

                if (ret == send || (++retry) > length)
                    quit = true;
                else
                {
                    buffer += ret;
                    send -= ret;
                    ::FlushFileBuffers(m_fd);
                }
            }

            return (length - (unsigned int)(send - ret));
#endif
        }

        unsigned int Baud(unsigned int baud)
        {
#ifdef SG_PLATFORM_LINUX
            switch (baud)
            {
            case 19200:
                baud = B19200;
                break;
            default:
                BOOST_ASSERT(false);
                baud = B19200;
                break;
            }
#endif
            return baud;
        }

        unsigned int Parity(unsigned int parity)
        {
#ifdef SG_PLATFORM_LINUX
            switch (parity)
            {
            case None:
                parity = 0;
                break;
            case Odd:
                parity = PARENB | PARODD;
                break;
            case Even:
                parity = PARENB;
                break;
            case Mark:
                parity = PARENB | CMSPAR | PARODD;
                break;
            case Space:
                parity = PARENB | CMSPAR;
                break;
            default:
                BOOST_ASSERT(false);
                parity = 0;
                break;
            }
#else
            switch (parity)
            {
            case None:
                parity = NOPARITY;
                break;
            case Odd:
                parity = ODDPARITY;
                break;
            case Even:
                parity = EVENPARITY;
                break;
            case Mark:
                parity = MARKPARITY;
                break;
            case Space:
                parity = SPACEPARITY;
                break;
            default:
                BOOST_ASSERT(false);
                parity = NOPARITY;
                break;
            }
#endif
            return parity;
        }

    private:
#ifdef SG_PLATFORM_LINUX
        int         m_fd;
#else
        HANDLE      m_fd;
        CommsOverlapped m_ol;
#endif
        bool        m_multidrop;
    };

    Comms::Comms(const std::string &dev, CommsType type)
        : m_dev_name(dev)
        , m_init(false)
        , m_start(false)
        , m_type(type)
        , m_dev(nullptr)
        , m_resp_timeout(false)
    {
        switch (m_type)
        {
        case sg::Comms::CT_QCOM:
            m_dev = new CommsSerialPort();
            break;
        default:
            BOOST_ASSERT(false);
            m_dev = new CommsSerialPort();
            break;
        }
    }

    Comms::~Comms()
    {
        this->Quit();

        CommsSerialPort *dev = _dev;
        SG_SAFE_DELETE(dev);
        m_dev = nullptr;
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
            if (!_dev->Close())
                return;

            m_init = false;
        }

    }

    bool Comms::ChangeDev(std::string const& dev)
    {
        this->Quit();

        m_dev_name = dev;
        if (!this->Init())
        {
            COMMS_LOG(boost::format("Change device to %1% failed\n") % m_dev_name, CLL_Error);
            return false;
        }

        this->Start();
        return true;
    }

    bool Comms::Init()
    {
        if (!m_init)
        {
            unsigned int baud = 19200U;
            CommsSerialPort::EParity parity = CommsSerialPort::Space;

            if (_dev->Open(m_dev_name, baud, parity))
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
        while (m_start)
        {
            if (_dev->WaitForRead())
            {
                this->Read();
            }
        }
    }

    void Comms::Read()
    {
        static uint8_t  *ptr;
        static uint8_t  msg_buf[SG_COMM_BUFF_SIZE];
        static unsigned int length;
        static bool first_time = true;

        if (first_time)
        {
            length = 0;
            ptr = msg_buf;
            std::memset(msg_buf, 0, sizeof(msg_buf));
            first_time = false;
        }

        //bool firstbyte = true;
        //bool trt = false;
        unsigned int ret = 0;
        bool timeout = false;

        do
        {
//            if (firstbyte)
//            {
//                ret = _dev->Read(ptr, 1U);
//                if (!ret && !trt && (m_resp_timer.Elapsed() > SG_TRT_TIMEOUT))
//                {
//                    COMMS_LOG("Trt reach\n", CLL_Warning);
//                    trt = true;
//                }
//
//                if (ret)
//                {
//                    ++ptr;
//                    ++length;
//                    firstbyte = false;
//                }
//            }
//            else
//            {
                ret = _dev->Read(ptr, sizeof(msg_buf) - length);
                ptr += ret;
                length += ret;
//            }

            if (ret && this->IsPacketComplete(msg_buf, length))
            {
                if (this->IsCRCValid(msg_buf, length))
                {
                    this->HandlePacket(msg_buf, length);
                }

                ptr = msg_buf;
                length = 0;
                std::memset(msg_buf, 0, SG_COMM_BUFF_SIZE);
                break;
            }

            if ((m_resp_time != 0) && ((this->Now() - m_resp_time) > SG_RESPONSE_TIMEOUT))
                timeout = true;

        } while (!timeout);

        std::unique_lock<std::mutex> lock(m_response);

        if (timeout)
            m_resp_timeout = true;
        else
            m_resp_timeout = false;

        lock.unlock();

        m_response_cond.notify_one();

        if (timeout && length)
        {
            ptr = msg_buf;
            length = 0;
            std::memset(msg_buf, 0, SG_COMM_BUFF_SIZE);
        }
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
        if (!_dev->Write(buf, length))
        {
            COMMS_LOG("Failed to send packet\n", CLL_Error);
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

    bool CommsPacketHandler::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(buf); 
        SG_UNREF_PARAM(length); 
        BOOST_ASSERT(false);
        return false;
    }

}


