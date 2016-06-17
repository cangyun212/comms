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
#endif

#include "Utils.hpp"

#include "Comms.hpp"


namespace sg {

    Comms::Comms(const std::string &dev, CommsType type)
        : m_fd(-1)
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

#ifdef SG_PLATFORM_LINUX
        if (m_init)
        {
            COMMS_LOG("Comms is shutting down...\n", CLL_Info);
            if (m_fd >= 0)
                close(m_fd);
            m_init = false;
        }
#endif

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

    int Comms::OpenDevFile() 
    {
        int fd = -1;
#ifdef SG_PLATFORM_LINUX
        fd = open(m_dev.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

        if (fd < 0)
        {
            COMMS_LOG(boost::format("%d\n") % strerror(errno), CLL_Error);
        }
        else
        {
            COMMS_LOG(boost::format("Comms open device %1%\n") % m_dev, CLL_Info);
            struct termios oldtio, newtio;
            tcgetattr(fd, &oldtio);
            memset(&newtio, 0, sizeof(newtio));

            newtio.c_cflag |= B19200;
            newtio.c_cflag |= CS8;
            newtio.c_iflag |= IGNPAR;
            newtio.c_cflag |= PARENB | CMSPAR;
            newtio.c_iflag |= IGNBRK;
            newtio.c_oflag = 0;
            newtio.c_lflag = 0;
            newtio.c_cflag |= CLOCAL | CREAD;

            tcflush(fd, TCIFLUSH);
            tcsetattr(fd, TCSANOW, &newtio);
        }
#endif
        return fd;
    }

    bool Comms::Init()
    {
        if (!m_init)
        {
            m_fd = this->OpenDevFile();
            if (m_fd >= 0)
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
#endif
    }

    void Comms::Read()
    {
#ifdef SG_PLATFORM_LINUX
        static uint8_t  *ptr;
        static uint8_t  msg_buf[BUFF_SIZE];
        static int length;
        int ret;
        static bool first_time = true;

        if (first_time)
        {
            ptr = msg_buf;
            memset(msg_buf, 0, BUFF_SIZE);
            first_time = false;
        }

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
            memset(msg_buf, 0, BUFF_SIZE);

           //m_resp_received = false;
        }
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
        if (m_fd >= 0)
        {
            for (int i = 0; i < length; ++i)
            {
#ifdef SG_PLATFORM_LINUX
                write(m_fd, &buf[i], 1);
#else
                SG_UNREF_PARAM(buf); // TOOD later
#endif
            }
        }
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


