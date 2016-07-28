#ifndef __SG_COMMS_HPP__
#define __SG_COMMS_HPP__

#include "Core.hpp"

#include <thread>
#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "Console/ConsolePrinter.hpp"
#include "CommsPredeclare.hpp"

#ifdef SG_PLATFORM_LINUX
#define TIMEOUT_USEC    100000 // microseconds
#else
#define TIMEOUT_USEC    10000 // milliseconds
#endif
#define SG_COMM_BUFF_SIZE           256
#define SG_COMM_WAKEUP_TIME         6000
#ifdef SG_PLATFORM_LINUX
#define SG_RESPONSE_TIMEOUT         200
#define SG_JOB_TIMEOUT              400
#define SG_COMM_OP_TIMEOUT          10
#else
#define SG_RESPONSE_TIMEOUT         300
#define SG_JOB_TIMEOUT              400
#define SG_COMM_OP_TIMEOUT          10000
#endif
#define SG_RETRY_TIME               300
//#define SG_TRT_TIMEOUT              5

#define COMMS_LOG(t, l)             SG_WS_SAFE_LOG(t, l)
#define COMMS_START_LOG_BLOCK()     SG_START_WS_LOG_BLOCK()
#define COMMS_LOG_BLOCK(t, l)       SG_WS_LOG_BLOCK(t, l)
#define COMMS_END_LOG_BLOCK()       SG_END_WS_LOG_BLOCK()

#define COMMS_PRINT(t)              SG_WS_SAFE_PRINT(t)
#define COMMS_START_PRINT_BLOCK()   SG_START_WS_PRINT_BLOCK()
#define COMMS_PRINT_BLOCK(t)        SG_WS_PRINT_BLOCK(t)
#define COMMS_END_PRINT_BLOCK()     SG_END_WS_PRINT_BLOCK()

namespace sg 
{
    class COMMS_API Comms : public std::enable_shared_from_this<Comms>
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
        std::string const &     GetDev() const { return m_dev_name;}
        bool    ChangeDev(std::string const& dev ) ;

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

    protected:
        typedef std::chrono::milliseconds cstime;
        typedef std::chrono::milliseconds::rep cstt;
        cstt Now() const
        {
            std::chrono::steady_clock::time_point tp = std::chrono::steady_clock::now(); // thread safe
            return std::chrono::duration_cast<cstime> (tp.time_since_epoch()).count();
        }

        std::string         m_dev_name;
        bool                m_init;
        std::atomic<bool>   m_start;
        CommsType           m_type;
        std::thread         m_reader;
        void               *m_dev;
        std::atomic<cstt>   m_resp_time;
        bool                m_resp_timeout;
        std::mutex          m_response;
        std::thread         m_checker;
        std::condition_variable m_response_cond;
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
        virtual uint8_t     Id() const { BOOST_ASSERT(false); return 0; }
        virtual uint8_t     RespId() const { BOOST_ASSERT(false); return 0; }
        virtual bool        Parse(uint8_t buf[], int length);
        virtual void        DoSubStage() {}
                bool        HasSubStage() const { return m_has_sub_stage; }
                void        StartSubStageThread();
                void        StopSubStageThread();
                void        RunSubStage();
                void        AddJob();

    protected:
        std::mutex              m_sub;
        std::condition_variable m_sub_cond;
        std::thread             m_job;
        bool                    m_has_sub_stage;
        int                     m_job_num;
    };

}





#endif


