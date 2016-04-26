#ifndef __SG_COMMS_HPP__
#define __SG_COMMS_HPP__

#include "core/core.hpp"
#include "core/core_thread.hpp"

#include <string>

#include "comms/comms_predeclare.hpp"

#define TIMEOUT_USEC    100000 // microseconds
#define BUFF_SIZE       256

namespace sg {

    class COMMS_API Comms : public enable_shared_from_this<Comms>
    {
    public:
        enum CommsType
        {
            CT_QCOM = 0,

            CT_UNKNOWN,
            CT_NUM
        };

    public:
        Comms(std::string const& dev, CommsType type = CT_UNKNOWN);
        virtual ~Comms();

    public:
        bool    Init();
        void    Quit();
        void    Start();
        void    Stop();
        void    ReceiveResponse();
        void    CheckCommsTimeout();
        bool    IsInited() const { return m_init; }
        bool    IsStarted() const { return m_start; }
        CommsType   GetType() const { return m_type; }
        std::string const &     GetDev() const { return m_dev;}
        std::string const &     GetSlave() const { return m_slave; }
        bool    ChangeDev(std::string dev ) ;

    protected:
        virtual void    DoInit() {}
        virtual void    DoQuit() {}
        virtual void    DoStart() {}
        virtual void    DoStop() {}
        virtual void    DoCheckCommsTimeout() {} // override implementation should not use condition variable now
        virtual bool    IsPacketComplete(uint8_t buf[], int length);
        virtual bool    IsCRCValid(uint8_t buf[], int length);
        virtual void    HandlePacket(uint8_t buf[], int length);
        virtual void    SendPacket(uint8_t buf[], int length);

    private:
        void    Read();
        void    StartResponseThread();
        void    StopResponseThread();
        void    StartCheckTimeoutThread();
        void    StopCheckTimeoutThread();
        int     OpenDevFile();
    protected:
        int             m_fd;
        std::string     m_dev;
        std::string     m_slave;
        bool            m_init;
        atomic<bool>    m_start;
        CommsType       m_type;
        thread          m_reader;
        //bool        m_resp_received;
        //bool        m_resp_timeout;
        //mutex       m_response;
        //condition_variable  m_response_cond;
        thread          m_checker;

    };

    class CommsPacketHandler
    {
    public:
        CommsPacketHandler(bool has_sub_stage)
            : m_has_sub_stage(has_sub_stage)
            , m_job_num(0)
        {

        }

        virtual ~CommsPacketHandler()
        {
            StopSubStageThread();
        }

    public:
        virtual uint8_t     Id() const = 0;
        virtual bool        Parse(uint8_t buf[], int length) = 0;
        virtual void        DoSubStage() {}
                bool        HasSubStage() const { return m_has_sub_stage; }
                void        StartSubStageThread();
                void        StopSubStageThread();
                void        RunSubStage();
                void        AddJob();

    protected:
        mutex   m_sub;
        condition_variable  m_sub_cond;
        thread  m_job;
        bool    m_has_sub_stage;
        int     m_job_num;
    };

}





#endif

