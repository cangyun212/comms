#ifndef __SG_QCOM_HPP__
#define __SG_QCOM_HPP__

#include "Core.hpp"

#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include "CommsPredeclare.hpp"
#include "Comms.hpp"
#include "Qcom/qogr/qogr_qcom.h"

#define SG_QCOM_POLLCYCLE_TIME  250

#define QCOM_MSG_EXTEND_SIZE 4 // (CNTL+FC+CRC) = (1 + 1 + 2)
#define QCOM_GET_PACKET_LENGTH(MSG_SIZE) (MSG_SIZE + QCOM_MSG_EXTEND_SIZE)
#define QCOM_DLL_HEADER_SIZE (sizeof(qc_dlltype))
#define QCOM_CRC_SIZE   2

#define QCOM_ACK_MASK       (0x01 << 7)
#define QCOM_LAMAPOLL_MASK  (0x01)
#define QCOM_CNTL_POLL_BIT  (0x01)

#define QCOM_MINIMUM_ADDRESS    1
#define QCOM_MAXMUM_ADDRESS     250

#define QCOM_VER_1_5            0x00
#define QCOM_VER_1_6            0x01

#define QCOM_GAME_NOT_CONFIG    0x00
#define QCOM_GAME_CONFIG_SET    0x01
#define QCOM_GAME_CONFIG_REQ    0x02
#define QCOM_GAME_CONFIG_READY  0x04
#define QCOM_GAME_CONFIG_GVN    0x08
#define QCOM_GAME_PC_CHANGE     0x10

#define QCOM_MAX_GAME_NUM       0xFF

#define QCOM_RESET_PSN          0x00

#define QCOM_EGM_NOT_CONFIG     0x00
#define QCOM_EGM_CONFIG_SET     0x01
#define QCOM_EGM_CONFIG_READY   0x02
#define QCOM_EGM_HASH_READY     0x04
#define QCOM_EGM_NAM_SET        0x08
#define QCOM_EGM_NAM_READY      0x10

#define QCOM_CTO_FAIL           0x00
#define QCOM_CTO_CANCEL         0x01
#define QCOM_CTO_APPROVE        0x02

#define QCOM_AUTHNO_BYTE_NUM    16

#define QCOM_SPAM_TYPE_A        1
#define QCOM_SPAM_TYPE_B        2

#define QCOM_METER_GROUP_NUM    3 // now only 0 -2 is available, group 3 - 15 is reserved
#define QCOM_METER_NUM          16 // each meter group has 0x00 - 0x0F meters, some of them are reserved

namespace sg 
{
    inline uint8_t  QcomNextPSN(uint8_t psn)
    {
        return (psn + 1) % 256;
    }

    enum QcomPollSeqType
    {
        Qcom_PSN_Events = 0,
        Qcom_PSN_ECT,

        Qcom_PSN_NUM
    };

    enum QcomEGMConfigState
    {
        Qcom_EGM_NoConfig = 0,
        Qcom_EGM_Config,

        Qcom_EGM_ConfigState_Num
    };

    struct QcomEGMControlData
    {
        uint32_t    serialMidBCD; // manually entered or hard code in the egm and returned by seek cmd
        uint8_t     poll_address; // 1 <= valid address <= 25
        uint8_t     last_control; // ACK/NAK bit
        uint8_t     protocol_ver; // 0x00 for Qcom1.5.x egm and 0x01 for Qcom1.6.x egm
        uint8_t     machine_enable; // if equal 1, then egm enable, otherwise egm disable
        uint8_t     seed[QCOM_MAX_PHA_SH_LENGTH];
        uint8_t     hash[QCOM_MAX_PHA_SH_LENGTH];
        uint8_t     game_config_state[QCOM_MAX_GAME_NUM]; // indicate egm game config state
        uint8_t     psn[Qcom_PSN_NUM];
        uint8_t     egm_config_state;
    };  

    struct QcomEGMControlPollData
    {
        uint8_t    mef;
        uint8_t    gcr;
        uint8_t    psn;
    };

    struct QcomEGMConfigPollData
    {
        // QCOM v1.6 additional fields
        uint32_t    den;
        uint32_t    tok;
        uint32_t    maxden;
        uint16_t    minrtp;
        uint16_t    maxrtp;
        uint16_t    maxsd;
        uint16_t    maxlines;
        uint32_t    maxbet;
        uint32_t    maxnpwin;
        uint32_t    maxpwin;
        uint32_t    maxect;
        // 
        uint8_t     jur;
    };

    struct QcomEGMConfigRespData
    {
        uint8_t         flag_a; // flag indicates which device are expected
        uint8_t         flag_b; // the same above, ref Qcom1.6-15.6.12
        uint16_t        bsvn; // base software version number, ref Qcom1.6-15.1.4
        uint8_t         games_num; // total number of available games
        uint8_t         games_num_enable; // total number of games that can be enable
        uint16_t        last_gvn; // game version number of the last initiated game in the egm
        uint8_t         last_var; // game variation number as above
        //uint8_t         flag_s; // bit 7 for shared progressive component flag, ref Qcom1.6-10.9; bit 8 for denomination hot-switching, ref Qcom1.6-15.6.12
        uint8_t         shared_progressive;
        uint8_t         denom_hot_switching;
    };

    struct QcomLinkedProgressiveData
    {
        uint32_t        lpamt[QCOM_REMAX_BMLPCA];
        uint16_t        pgid[QCOM_REMAX_BMLPCA];
        uint8_t         plvl[QCOM_REMAX_BMLPCA];
        uint8_t         pnum;
    };

    // merge progressive config data and progressive change data together
    struct QcomProgressiveConfigData
    {
        uint32_t        sup[QCOM_REMAX_PCP]; // ref Qcom1.6-15.4.6, new jackpot start up amount
        uint32_t        pinc[QCOM_REMAX_PCP]; // ref Qcom1.6-15.4.6, new jackpot level percentage increment x 10000
        uint32_t        ceil[QCOM_REMAX_PCP]; // ref Qcom1.6-15.4.6, new jackpot level ceiling
        uint32_t        auxrtp[QCOM_REMAX_PCP]; // ref Qcom1.6-15.4.6, new auxiliary RTP for the level x 10000
        uint32_t        init_contri[QCOM_REMAX_EGMGCP]; // ref Qcom1.6-15.4.3, if SAP, will be added on top of current amount; if LP, is the initial jackpot current amount
        //char            name[QCOM_REMAX_PCP][21]; // level name, display purpose only
        uint8_t         flag_p[QCOM_REMAX_EGMGCP]; // ref Qcom1.6-15.4.3, check bit 7 to denote the level as a LP, otherwise the level is a SAP
        //uint8_t         pnum;
    };

    struct QcomLinkProgressivePoolData
    {
        QcomProgressiveConfigData config;
        uint32_t        camt[QCOM_REMAX_EGMGCP];
        uint32_t        overflow[QCOM_REMAX_EGMGCP];
        uint8_t         pnum;
        uint8_t         configured;
    };

    struct QcomVariationData
    {
        uint16_t        pret[QCOM_REMAX_EGMGCR]; // ref Qcom1.6-15.6.11, theoretical RTP of the non-progressive component, BCD value for Qcom 1.5, hex x 100 for Qcom 1.6
        uint8_t         var[QCOM_REMAX_EGMGCR]; // ref Qcom1.6-15.6.11
        uint8_t         vnum;
    };

    struct QcomGameSettingData
    {
        uint16_t        pgid; // ref Qcom1.6-15.4.3, 0xFFFF if non-progressive or SAP game, 0x0001 ~ 0xFFFE if LP game
        uint8_t         var; // ref Qcom1.6-15.4.3, BCD value, current variation to use
        uint8_t         var_lock; // ref Qcom1.6-15.4.3, set to disable variation hot-switching
        uint8_t         game_enable; // ref Qcom1.6-15.4.3, indicates the game is enable or not
    };

    struct QcomGameConfigData
    {
        QcomProgressiveConfigData       progressive;
        QcomGameSettingData             settings;
    };

    struct QcomProgressiveLevelData
    {
        //uint8_t         type[QCOM_REMAX_PMR]; // ref Qcom1.6-15.6.3 level type
        uint32_t        camt[QCOM_REMAX_PMR]; // ref Qcom1.6-15.6.3 current progressive level contribution
        uint16_t        hits[QCOM_REMAX_PMR]; // ref Qcom1.6-15.6.3 total hits on the progressive level
        uint32_t        wins[QCOM_REMAX_PMR]; // ref Qcom1.6-15.6.3 total wins for the progressive level
        float           hrate[QCOM_REMAX_PMR];// ref Qcom1.6-15.6.3 theoretical hit rate
    };

    struct QcomProgressiveData
    {
        QcomProgressiveLevelData levels;
        uint32_t        pamt; // ref Qcom1.6.-15.6.3 lp turnover meter
        uint8_t         pnum; // total level number 0 - 8
        //uint32_t        pinc[QCOM_REMAX_PCP]; // ref Qcom1.6-15.4.6, new jackpot level percentage increment x 10000
        //uint32_t        ceil[QCOM_REMAX_PCP]; // ref Qcom1.6-15.4.6, new jackpot level ceiling
        //uint32_t        auxrtp[QCOM_REMAX_PCP]; // ref Qcom1.6-15.4.6, nw auxiliary RTP for the level x 10000
    };

    //struct QcomProgressiveChangeData
    //{
    //    uint32_t            sup[QCOM_REMAX_PCP]; // ref Qcom1.6-15.4.6, new jackpot level start-up amount
    //    QcomProgressiveData prog;
    //    uint8_t             pnum;
    //};

    struct QcomExtJPInfoData
    {
        uint16_t            epgid[QCOM_REMAX_EXTJIP];
        uint8_t             lumf[QCOM_REMAX_EXTJIP];
        char                lname[QCOM_REMAX_EXTJIP][QCOM_EXTJIP_MAX_LNAME];
        uint16_t            rtp;
        uint8_t             levels;
        uint8_t             display;
        uint8_t             icon;
    };

    struct QcomBetMetersData
    {
        uint16_t    maxl;
        uint16_t    maxb;
        uint16_t    gbfa;
        uint16_t    gbfb;
        uint32_t    cmet[QCOM_BMR_MAX_CMET];
    };

    struct QcomMultiGameVarMetersData
    {
        uint32_t    str;  // Total Game Stroke, ref Qcom1.6-15.6.6
        uint32_t    turn; // Total Game Turnover, ref Qcom1.6-15.6.6
        uint32_t    win;  // Total Game Wins added to credit meter. ref Qcom1.6.15.6.6
        uint32_t    pwin; // Total Game Linked Progressive Wins, ref Qcom1.6-15.6.6
        uint32_t    gwin; // Total Games Won, ref Qcom1.6-15.6.6
    };

    struct QcomPlayerChoiceMetersData
    {
        uint32_t    pcmet[QCOM_PCMR_MAX_RE];
        uint8_t     num;
    };

    struct QcomMeterGroup
    {
        uint32_t    met[QCOM_METER_NUM];
    };

    struct QcomMeterGroupContributionData
    {
        QcomMeterGroup      groups[QCOM_METER_GROUP_NUM];
        uint32_t            pamt; // LP Turnover meter. The total game turnover applicable to PGID, ref Qcom1.6-15.6.8
        //uint16_t            lgvn; // The Game Version Number of the last initiated game on the EGM, ref Qcom1.6-15.6.8
        //uint16_t            pgid;
    };

    struct QcomECTToEGMPollData
    {
        uint32_t            eamt;
        uint8_t             id;
        uint8_t             cashless;
    };

    struct QcomGameData
    {
        QcomGameConfigData              config;
        QcomVariationData               variations;
        QcomProgressiveData             prog;
        QcomBetMetersData               betm;
        QcomMultiGameVarMetersData      mgvm;
        QcomPlayerChoiceMetersData      pcm;
        QcomMeterGroupContributionData  mgc;
        uint16_t                        gvn;
        uint8_t                         var_hot_switching; // ref Qcom1.6-15.6.11, set if the game support on-the-fly variation switching
        uint8_t                         lp_only; // ref Qcom1.6-15.6.11
        uint8_t                         plbm; // ref Qcom1.6-15.6.11
        uint8_t                         customSAP; // ref Qcom1.6-15.6.4
    };

    struct QcomEGMParametersData
    {
        // ref all in Qcom1.6-15.4.5
        uint8_t     reserve;
        uint8_t     auto_play;
        uint8_t     crlimit_mode;
        uint8_t     opr;
        uint32_t    lwin;           // cents
        uint32_t    crlimit;        // cents
        uint32_t    dulimit;        // cents
        uint8_t     dumax;
        int16_t     tzadj;          // uints of minutes
        uint8_t     pid;
        uint32_t    pwrtime;        // senconds
        uint32_t    npwinp;         // cents
        uint32_t    sapwinp;        // cents
        uint16_t    eodt;           // End of Day Time. Units: mniutes since midnight (valid range is 0...1439)
    };

    struct QcomEGMStatusData
    {
        uint8_t     flag_a;
        uint8_t     flag_b;
        uint8_t     state;
    };

    struct QcomProgHashRequestData
    {
        uint8_t     seed[QCOM_MAX_PHA_SH_LENGTH];
        uint8_t     new_seed;
        uint8_t     mef;
    };

    struct QcomSysLockupRequestData
    {
        char        text[QCOM_SALRP_TEXT_SIZE];
        uint8_t     len;
        uint8_t     no_resetkey;
        uint8_t     continue_style;
        uint8_t     question_style;
        uint8_t     lamp_test;
        uint8_t     fanfare;
    };

    struct QcomCashTicketOutRequestAckPollData
    {
        char        certification[QCOM_TORAP_MAX_CTEXT];
        uint8_t     authno[QCOM_AUTHNO_BYTE_NUM];
        uint32_t    amount;
        uint16_t    serial;
        uint8_t     flag;
        uint8_t     clen;
    };

    struct QcomCashTicketInRequestAckPollData
    {
        uint8_t     authno[QCOM_AUTHNO_BYTE_NUM];
        uint32_t    amount;
        uint8_t     fcode;
    };

    struct QcomEGMGeneralMaintenancePollData
    {
        uint8_t     meter_group_0;
        uint8_t     meter_group_1;
        uint8_t     meter_group_2;
        uint8_t     note_acceptor_status;
        uint8_t     mef;
        uint8_t     var;
        uint8_t     player_choice_meter;
        uint8_t     bet_meters;
        uint8_t     progconfig;
        uint8_t     gameconfig;
        uint8_t     progmeters;
        uint8_t     multigame;
        uint8_t     gef;
    };

    struct QcomNoteAcceptorMaintenanceData
    {
        uint8_t     five;
        uint8_t     ten;
        uint8_t     twenty;
        uint8_t     fifty;
        uint8_t     hundred;
    };

    struct QcomNoteAcceptorStatusData
    {
        QcomNoteAcceptorMaintenanceData nam;
        char        nads[QCOM_NASR_NADS_SIZE];
        uint8_t     full;

    };

    struct QcomHopperTicketPrinterData
    {
        uint32_t    refill;
        uint32_t    collim;
        uint32_t    ticket;
        uint32_t    dorefill;
    };

    struct QcomGeneralResetPollData
    {
        uint8_t     fault;
        uint8_t     lockup;
        uint8_t     state;
    };

    struct QcomSPAMPollData
    {
        char        text[QCOM_SPAMP_TEXT_SIZE];
        uint8_t     len;
        uint8_t     prominence;
        uint8_t     fanfare;

    };

    struct QcomTowerLightMaintenancePollData
    {
        uint8_t     yellow_on;
        uint8_t     blue_on;
        uint8_t     red_on;
        uint8_t     yellow_flash;
        uint8_t     blue_flash;
        uint8_t     red_flash;
    };

    struct QcomEGMData
    {
        QcomEGMControlData              control;
        QcomEGMConfigPollData           custom;
        QcomEGMConfigRespData           config;
        QcomEGMParametersData           param;
        QcomEGMStatusData               status;
        QcomExtJPInfoData               extjpinfo;
        QcomNoteAcceptorStatusData      nasr;
        QcomHopperTicketPrinterData     htp;
        //QcomMeterGroupContributionData  mgc;
        QcomGameData                    games[QCOM_MAX_GAME_NUM];
    };


    struct QcomData
    {
        QcomEGMData     data;
        std::mutex      locker;
    };

    typedef std::shared_ptr<QcomData>    QcomDataPtr;

    struct QcomPoll
    {
        uint8_t length;
        union
        {
            uint8_t data[SG_COMM_BUFF_SIZE];
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
        QcomJobData(JobType type, size_t poll_num = 2);

    public:
        size_t      GetPollNum() const;
        QcomPollPtr GetPoll(size_t index) const;
        void        AddPoll(QcomPollPtr const & poll);
        QcomPollPtr GetBroadcast() const { return m_broadcast; };
        void        SetBroadcast(QcomPollPtr const & broadcast) { SG_ASSERT(!m_broadcast); m_broadcast = broadcast; };
        JobType     GetType() const { return m_type; }

    private:
        typedef std::vector<QcomPollPtr>   PollsType;
        PollsType   m_polls;
        QcomPollPtr m_broadcast;
        JobType     m_type;
    };

    typedef std::shared_ptr<QcomJobData>     QcomJobDataPtr;

//    class QcomLocker
//    {
//    public:
//        QcomLocker() : m_p(nullptr) {}
//        QcomLocker(std::vector<QcomDataPtr> *p, std::mutex &m) : m_p(p), m_lock(m) {}
//        QcomLocker(QcomLocker &&o) : m_p(std::move(o.m_p)), m_lock(std::move(o.m_lock)){}
//       ~QcomLocker() { m_p = nullptr; }
//
//    public:
//        std::vector<QcomDataPtr>&       Data() { return *m_p; }
//
//    private:
//        std::vector<QcomDataPtr>       *m_p;
//        std::unique_lock<std::mutex>    m_lock;
//    };

    class COMMS_API CommsQcom : public Comms
    {
    public:
        CommsQcom(std::string const& dev);
       ~CommsQcom();

    public:
        void    SeekEGM();
        void    PollAddress(uint8_t poll_address);
        void    TimeData();
        void    LinkJPCurrentAmount(QcomLinkedProgressiveData const& data);
        void    GeneralPromotional(std::string const& text);
        void    SiteDetail(std::string const& stext, std::string const& ltext);

        void    GeneralStatus(uint8_t poll_address);
        void    EGMConfRequest(uint8_t poll_address, QcomEGMControlPollData const& data);
        void    EGMConfiguration(uint8_t poll_address, QcomEGMConfigPollData const& data);
        void    GameConfiguration(uint8_t poll_address, uint16_t gvn, uint8_t pnum, QcomGameConfigData const& data);
        void    GameConfigurationChange(uint8_t poll_address, uint16_t gvn, QcomGameSettingData const& data);
        void    EGMParameters(uint8_t poll_address, QcomEGMParametersData const& data);
        void    ProgressiveChange(uint8_t poll_address, uint16_t gvn, uint8_t pnum, QcomProgressiveConfigData const& data);
        void    ExtJPInfo(uint8_t poll_address, QcomExtJPInfoData const& data);
        void    ProgHashRequest(uint8_t poll_address, QcomProgHashRequestData const& data);
        void    SystemLockup(uint8_t poll_address, QcomSysLockupRequestData const& data);
        void    PurgeEvents(uint8_t poll_address, uint8_t evtno);
        void    CashTicketOutAck(uint8_t poll_address, QcomCashTicketOutRequestAckPollData const& data);
        void    CashTicketInAck(uint8_t poll_address, QcomCashTicketInRequestAckPollData const& data);
        void    CashTicketOutReqeust(uint8_t poll_address);
        void    EGMGeneralMaintenance(uint8_t poll_address, uint16_t gvn, QcomEGMGeneralMaintenancePollData const& data);
        void    RequestAllLoggedEvents(uint8_t poll_address);
        void    NoteAcceptorMaintenance(uint8_t poll_address, QcomNoteAcceptorMaintenanceData const& data);
        void    HopperTicketPrinterMaintenance(uint8_t poll_address, uint8_t test, QcomHopperTicketPrinterData const& data);
        void    LPAwardAck(uint8_t poll_address);
        void    GeneralReset(uint8_t poll_address, QcomGeneralResetPollData const& data);
        void    SPAM(uint8_t poll_address, uint8_t type, QcomSPAMPollData const& data);
        void    TowerLightMaintenance(uint8_t poll_address, QcomTowerLightMaintenancePollData const& data);
        void    ECTToEGM(uint8_t poll_address, QcomECTToEGMPollData const& data);
        void    ECTFromEGMRequest(uint8_t poll_address);
        void    ECTLockupReset(uint8_t poll_address, uint8_t deined);
        void    PendingPoll(size_t poll_num = 2);
        void    SendPoll();

    public:
        QcomDataPtr     GetEgmData(uint8_t poll_address);
        QcomDataPtr     AddNewEgm();
        size_t          GetEgmNum();
        //QcomLocker      LockEGMData();
        void            CaptureEGMData(std::vector<QcomDataPtr> & data);

        template <typename Predicate>
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

        template <typename F>
        void Traverse(F const& func)
        {
            std::unique_lock<std::mutex> lock(m_egms_guard);

            for (auto & i : m_egms)
            {
                func(i);
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
        bool    AddLPConfigData(uint8_t poll_address, uint16_t gvn, uint16_t pgid, uint8_t pnum, QcomProgressiveConfigData const& data);
        bool    UpdateLPConfigData(uint16_t pgid, uint8_t pnum, QcomProgressiveConfigData const& data);
        bool    GetLPConfigData(QcomLinkedProgressiveData &data);
        bool    ReMapPGID(uint8_t poll_address, uint16_t gvn, uint16_t new_pgid, uint16_t old_pgid, uint8_t shared);
        bool    DecoratePoll(QcomPollPtr &p);

    private:
        void    HandleResponse(uint8_t buf[], int length);

    private:
        typedef std::unordered_map<uint8_t, CommsPacketHandlerPtr>    HandlerType;
        HandlerType     m_handler;
        HandlerType     m_resp_handler;

        typedef std::vector<QcomDataPtr>    EgmPoolType;
        EgmPoolType     m_egms;

        std::mutex      m_egms_guard;

        std::thread     m_worker;
        std::mutex      m_job;
        std::condition_variable  m_job_cond;

        cstt            m_tpc;
        bool            m_skip;

        bool            m_pending;
        QcomJobDataPtr  m_pending_job;

        bool            m_lpbroadcast;
        std::mutex      m_lp_guard;
        typedef std::shared_ptr<QcomLinkProgressivePoolData> QcomLinkProgressivePoolDataPtr;
        typedef std::map<uint16_t, QcomLinkProgressivePoolDataPtr> LPPool;
        LPPool          m_lps;
        typedef LPPool::const_iterator   LPPoolHandle;
        LPPoolHandle    m_curr_lp;
        typedef std::unordered_set<uint16_t>                LPGameSet;
        typedef std::shared_ptr<LPGameSet>                  LPGameSetPtr;
        typedef std::unordered_map<uint8_t, LPGameSetPtr>   LPEGM;
        typedef std::shared_ptr<LPEGM>                      LPEGMPtr;
        typedef std::unordered_map<uint16_t, LPEGMPtr>      LPEGMPool;
        LPEGMPool       m_lp_egms;


        typedef std::list<QcomJobDataPtr>   JobQueue;
        JobQueue        m_jobs;
    };
}




#endif



