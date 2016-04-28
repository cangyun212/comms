#ifndef __SG_QCOM_HPP__
#define __SG_QCOM_HPP__

#include "Core.hpp"

#include <algorithm>
#include <vector>
#include <list>
#include <map>

#include "CommsPredeclare.hpp"
#include "Comms.hpp"
#include "Qcom/qogr/qogr_qcom.h"

#define QCOM_MSG_EXTEND_SIZE 4 // (CNTL+FC+CRC) = (1 + 1 + 2)
#define QCOM_GET_PACKET_LENGTH(MSG_SIZE) (MSG_SIZE + QCOM_MSG_EXTEND_SIZE)
#define QCOM_DLL_HEADER_SIZE (sizeof(qc_dlltype))
#define QCOM_CRC_SIZE   2

#define QCOM_ACK_MASK       (0x01 << 7)
#define QCOM_LAMAPOLL_MASK  (0x01)
#define QCOM_CNTL_POLL_BIT  (0x01)
#define QCOM_NO_RESPONSE    (-1)

#define QCOM_MINIMUM_ADDRESS    1
#define QCOM_MAXMUM_ADDRESS     250

namespace sg {

    enum PollSeqNum
    {
        PSN_EVENTS = 0,
        PSN_ECT,

        PSN_NUM
    };

    struct EGMConfigData
    {
        uint8_t  jur;
        // QCOM v1.6 additional fields
        uint32_t   den;
        uint32_t   tok;
        uint32_t   maxden;
        uint16_t   minrtp;
        uint16_t   maxrtp;
        uint16_t   maxsd;
        uint16_t   maxlines;
        uint32_t   maxbet;
        uint32_t   maxnpwin;
        uint32_t   maxpwin;
        uint32_t   maxect;
    };

    struct ProgressiveConfigData
    {
        uint8_t    pnum; // number of progressive levels in game. 0...8
        uint8_t    lp[QCOM_REMAX_EGMGCP];   // if set, denotes the level is to be set as a LP
        uint32_t   camt[QCOM_REMAX_EGMGCP]; // If SAP, CAMT is the initial contribution towards the progressive level.
                         // If LP, CAMT is the initial jackpot current amount for the LP level and may be treated by the EGM as the same as a LP Broadcast to the EGM’s PGID
    };

    struct EGMData
    {
        uint32_t        serialMidBCD; // manually entered into egm and returned by seek cmd
        uint8_t         last_control; // ACK/NAK bit
        int8_t          resp_funcode; // store the function code of poll which has the response otherwise -1
        uint8_t         poll_address; // 1 <= valid address <= 250
        uint16_t        machine_enable; // if equal 1, then egm enable, otherwise egm disable
        uint16_t        game_config_req; // if set, the egm will queue the egm game configuration response
        uint8_t         poll_seq_num[PSN_NUM]; // poll sequence number, ref Qcom1.6-15.1.9
        uint8_t         protocol_ver; // 0x00 for Qcom1.5.x egm and 0x01 for Qcom1.6.x egms
        uint8_t         egm_config_flag_a; // flag indicates which device are expected
        uint8_t         egm_config_flag_b; // the same as above, ref Qcom1.6-15.6.12
        uint16_t        base_gvn; // base software version number, ref Qcom1.6-15.1.4
        uint8_t         total_num_games; // total number of available games
        uint8_t         total_num_games_enable; // total number of games that can be enabled
        uint16_t        last_gvn; // game version number of the last initiated game in the egm
        uint8_t         last_var; // game variation number as above
        uint8_t         flgsh; // bit 7 for shared progressive component flag, ref Qcom1.6-10.9; bit 8 for denomination hot-switching, ref Qcom1.6-15.6.12
        EGMConfigData   egm_config; // store the egm config data which sent
        ProgressiveConfigData  progressive_config;// store the game config data which sent
    };

    struct QcomData
    {
        EGMData data;
        std::mutex locker;
    };

    typedef std::shared_ptr<QcomData>    QcomDataPtr;

    struct QcomEGMConifgReqCustomData
    {
        uint16_t    mef;
        uint16_t    gcr;
        uint16_t    psn;
        uint8_t     egm;
    };

    struct QcomEGMConfigCustomData
    {
        uint8_t  egm;
        EGMConfigData data;
    };    // QCOM v1.6 version

    struct QcomGameConfigCustomData
    {
        uint8_t  egm;
        uint8_t  var;
        uint8_t  var_lock;
        uint8_t  game_enable;
        ProgressiveConfigData data;

    };    // QCOM v1.6 version

    struct QcomPoll
    {
        uint8_t length;
        union
        {
            uint8_t data[BUFF_SIZE];
            QCOM_PollMsgType    poll;
        };
    };

    typedef std::shared_ptr<QcomPoll>    QcomPollPtr;

    class QcomJobData
    {
    public:
        enum JobType
        {
            JT_POLL = 0,
            JT_BROADCAST,
            JT_BROADCAST_SEEK,
            JT_QUIT,

            JT_NUM
        };

    public:
        QcomJobData(JobType type, size_t poll_num = 1);

    public:
        size_t      GetPollNum() const;
        QcomPollPtr GetPoll(size_t poll_address) const;
        void        AddPoll(QcomPollPtr const & poll);
        size_t      GetBroadcastNum() const;
        QcomPollPtr GetBroadcast(size_t index) const;
        void        AddBroadcast(QcomPollPtr const & broadcast);
        JobType     GetType() const { return m_type; }

    private:
        typedef std::vector<QcomPollPtr>   PollsType;
        PollsType   m_polls;
        PollsType   m_broadcast;
        JobType     m_type;
    };

    typedef std::shared_ptr<QcomJobData>     QcomJobDataPtr;

    class COMMS_API CommsQcom : public Comms
    {
    public:
        CommsQcom(std::string const& dev);
       ~CommsQcom();

    public:
        // Seek EGM Broadcast Poll
        void    SeekEGM(); // TODO : For test purpose refactor later
        void    PollAddress(uint8_t poll_address); // TODO : For test purpose refactor later
        void    EGMConfRequest(uint8_t poll_address, uint8_t mef, uint8_t gcr, uint8_t psn);
        void    EGMConfiguration(uint8_t poll_address,  uint8_t jur, uint32_t den, uint32_t tok,uint32_t maxden,
                        uint16_t minrtp,uint16_t maxrtp,uint16_t maxsd,uint16_t maxlines,uint32_t maxbet,
                        uint32_t maxnpwin,uint32_t maxpwin,uint32_t maxect);
        void    SendBroadcast(uint32_t broadcast_type,
                              std::string gpm_text, std::string sds_text, std::string sdl_text);
        void    GameConfiguration(uint8_t poll_address, uint8_t var, uint8_t varlock, uint8_t gameenable, uint8_t pnum,
                                  const std::vector<uint8_t>& lp, const std::vector<uint32_t>& amct);

    public:
        QcomDataPtr     GetEgmData(uint8_t poll_address);
        void            GetEgmData(std::vector<QcomDataPtr> & data);
        QcomDataPtr     AddNewEgm();
        size_t          GetEgmNum();

        template<typename Predicate>
        QcomDataPtr FindEgmData(Predicate const & p)
        {
           std::unique_lock<std::mutex> lock(m_egms_guard);

           auto it = std::find_if(m_egms.begin(), m_egms.end(), p);

           if (it != m_egms.end())
           {
               return *it;
           }
           else
           {
               return nullptr;
           }
        }

    public:
        void    RunJob();
        void    AddJob(QcomJobDataPtr const & job_data);

    public:
        CommsPacketHandlerPtr   GetHandler(uint8_t id) const;

    protected:
        void    DoInit() override;
        void    DoStart() override;
        void    DoStop() override;
        void    DoCheckCommsTimeout() override;
        bool    IsPacketComplete(uint8_t buf[], int length) override;
        bool    IsCRCValid(uint8_t buf[], int length) override;
        void    HandlePacket(uint8_t buf[], int length) override;

    private:
        void    StartJobThread();
        void    StopJobThread();

    private:
        void    HandleResponse(uint8_t buf[], int length);

    private:
        typedef std::map<uint8_t, CommsPacketHandlerPtr>    HandlerType;
        HandlerType     m_handler;
        HandlerType     m_resp_handler;

        typedef std::vector<QcomDataPtr>    EgmPoolType;
        EgmPoolType     m_egms;

        std::mutex      m_egms_guard;

        std::thread     m_worker;
        std::mutex      m_job;
        std::condition_variable  m_job_cond;

        typedef std::list<QcomJobDataPtr>   JobQueue;
        JobQueue        m_jobs;
    };
}




#endif



