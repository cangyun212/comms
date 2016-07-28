#ifndef __SG_QCOM_ACTION_HPP__
#define __SG_QCOM_ACTION_HPP__

#include "Core.hpp"

#include <vector>

#include "Predeclare.hpp"
#include "Action.hpp"


namespace sg 
{
    class QcomPendingAction : public Action
    {
    public:
        QcomPendingAction();
       ~QcomPendingAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        size_t      PollNum() const { return s_pollnum; }

    private:
        static size_t   s_pollnum;
    };

    class QcomSendAction : public Action
    {
    public:
        QcomSendAction();
       ~QcomSendAction();

    public:
        ActionPtr   Clone() override;
        const char* Description() const override;

    private:

    };

    class QcomSeekEGMAction : public Action
    {
    public:
        QcomSeekEGMAction();
       ~QcomSeekEGMAction();

    public:
        ActionPtr   Clone() override;
        const char* Description() const override;

    private:

    };

    class QcomEGMPollAddConfAction : public Action
    {
    public:
        QcomEGMPollAddConfAction();
       ~QcomEGMPollAddConfAction();

    public:
        ActionPtr   Clone() override;
        const char* Description() const override;

    private:

    };

    class QcomEGMGeneralStatusAction : public Action
    {
    public:
        QcomEGMGeneralStatusAction();
       ~QcomEGMGeneralStatusAction();

    public:
        ActionPtr   Clone() override;
        const char* Description() const override;
    };

    class QcomEGMConfRequestAction : public Action
    {
    public:
        QcomEGMConfRequestAction();
       ~QcomEGMConfRequestAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t     MEF() const { return s_mef; }
        uint8_t     GCR() const { return s_gcr; }
        uint8_t     PSN() const { return s_psn; }

    private:
       static uint8       s_mef;
       static uint8       s_gcr;
       static uint8       s_psn;

    };

    class QcomEGMConfAction : public Action
    {
    public:
        QcomEGMConfAction();
       ~QcomEGMConfAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t     JUR() const { return s_jur; }
        uint32_t    DEN() const { return s_den; }
        uint32_t    TOK() const { return s_tok; }
        uint32_t    MaxDEN() const { return s_maxden; }
        uint16_t    MinRTP() const { return s_minrtp; }
        uint16_t    MaxRTP() const { return s_maxrtp; }
        uint16_t    MaxSD() const { return s_maxsd; }
        uint16_t    MaxLines() const { return s_maxlines; }
        uint32_t    MaxBet() const { return s_maxbet; }
        uint32_t    MaxNPWin() const { return s_maxnpwin; }
        uint32_t    MaxPWin() const { return s_maxpwin; }
        uint32_t    MaxECT() const { return s_maxect; }

    private:
        static uint8   s_jur;
        static uint32  s_den;
        static uint32  s_tok;
        static uint32  s_maxden;
        static uint16  s_minrtp;
        static uint16  s_maxrtp;
        static uint16  s_maxsd;
        static uint16  s_maxlines;
        static uint32  s_maxbet;
        static uint32  s_maxnpwin;
        static uint32  s_maxpwin;
        static uint32  s_maxect;
    };

    class QcomGameConfigurationAction : public Action
    {
    public:
        QcomGameConfigurationAction();
        ~QcomGameConfigurationAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint16_t    GVN() const { return s_gvn; }
        uint16_t    PGID() const { return s_pgid; }
        uint8_t     VAR() const { return s_var; }
        uint8_t     VARLock() const { return s_var_lock; }
        uint8_t     GameEnable() const { return s_game_enable; }
        uint8_t     ProgressiveConfig(uint8_t *lp, uint32_t *camt);

    private:
        static uint16                   s_gvn;
        static uint16                   s_pgid;
        static uint8                    s_var;
        static uint8                    s_var_lock;
        static uint8                    s_game_enable;
        static std::vector<uint8>       s_lp;
        static std::vector<uint32>      s_camt;
    };



    class QcomGameConfigurationChangeAction : public Action
    {
    public:
        QcomGameConfigurationChangeAction();
       ~QcomGameConfigurationChangeAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint16_t    GVN() const { return s_gvn; }
        uint16_t    PGID() const { return s_pgid; }
        uint8_t     VAR() const { return s_var; }
        uint8_t     GameEnable() const { return s_game_enable; }

    private:
        static uint16   s_gvn;
        static uint16   s_pgid;
        static uint8    s_var;
        static uint8    s_game_enable;
    };

    class QcomEGMParametersAction : public Action
    {
    public:
        QcomEGMParametersAction();
       ~QcomEGMParametersAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t  Reserve() const { return s_reserve; }
        uint8_t  AutoPlay() const { return s_autoplay; }
        uint8_t  CRLimitMode() const { return s_crlimitmode; }
        uint8_t  OPR() const { return s_opr; }
        uint32_t LWin() const { return s_lwin; }
        uint32_t CRLimit() const { return s_crlimit; }
        uint8_t  DUMax() const { return s_dumax; }
        uint32_t DULimit() const { return s_dulimit; }
        int16_t  TZADJ() const { return s_tzadj; }
        uint32_t PWRTime() const { return s_pwrtime; }
        uint8_t  PID() const { return s_pid; }
        uint16_t EODT() const { return s_eodt; }
        uint32_t NPWinP() const { return s_npwinp; }
        uint32_t SAPWinP() const { return s_sapwinp; }

    private:
        static  uint8    s_reserve;
        static  uint8    s_autoplay;
        static  uint8    s_crlimitmode;
        static  uint8    s_opr;
        static  uint32   s_lwin;
        static  uint32   s_crlimit;
        static  uint8    s_dumax;
        static  uint32   s_dulimit;
        static  int16    s_tzadj;
        static  uint32   s_pwrtime;
        static  uint8    s_pid;
        static  uint16   s_eodt;
        static  uint32   s_npwinp;
        static  uint32   s_sapwinp;
    };

    class QcomPurgeEventsAction : public Action
    {
    public:
        QcomPurgeEventsAction();
       ~QcomPurgeEventsAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t     EVTNO() const { return m_evtno; }

    private:
        static uint8  m_evtno;
    };

    class QcomProgressiveConfigAction : public Action
    {
    public:
        QcomProgressiveConfigAction();
       ~QcomProgressiveConfigAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t     ProgChangeData(uint32_t *sup, uint32_t *pinc, uint32_t *ceil, uint32_t *auxrtp) const;
        uint16_t    GVN() const { return s_gvn; }

    private:
        static std::vector<uint32>      s_sup;
        static std::vector<uint32>      s_pinc;
        static std::vector<uint32>      s_ceil;
        static std::vector<uint32>      s_auxrtp;
        static uint16                   s_gvn;
    };

    class QcomExtJPInfoAction : public Action
    {
    public:
        QcomExtJPInfoAction();
       ~QcomExtJPInfoAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t     ExtJPData(uint16_t *epgid, uint8_t *lumf, char (*lname)[16]) const;
        uint16_t    ExtJPRTP() const { return s_rtp; }
        uint8_t     ExtJPDisplay() const { return s_display; }
        uint8_t     ExtJPIcon() const { return s_icon; }

    private:
        static std::vector<uint16>      s_epgid;
        static std::vector<uint8>       s_umf;
        static std::vector<std::string> s_name;
        static uint16                   s_rtp;
        static uint8                    s_display;
        static uint8                    s_icon;
    };

    class QcomProgHashRequestAction : public Action
    {
    public:
        QcomProgHashRequestAction();
       ~QcomProgHashRequestAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t     MEF() const { return s_mef; }
        uint8_t     Seed(uint8_t *seed, size_t len);

    private:
        static std::vector<uint8>       s_seed;
        static uint8                    s_mef;
        static uint8                    s_new_seed;
    };

    class QcomSysLockupRequestAction : public Action
    {
    public:
        QcomSysLockupRequestAction();
       ~QcomSysLockupRequestAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        std::string Text() const { return s_text; }
        uint8_t     NoResetKey() const { return s_noreset_key; }
        uint8_t     ContinueStyle() const { return s_continue; }
        uint8_t     QuestionStyle() const { return s_question; }
        uint8_t     LampTest() const { return s_lamp_test; }
        uint8_t     Fanfare() const { return s_fanfare; }

    private:
        static std::string          s_text;
        static uint8                s_noreset_key;
        static uint8                s_continue;
        static uint8                s_question;
        static uint8                s_lamp_test;
        static uint8                s_fanfare;
    };

    class QcomTimeDateAction : public Action
    {
    public:
        QcomTimeDateAction();
       ~QcomTimeDateAction();

    public:
        ActionPtr   Clone() override;
        const char* Description() const override;

    };

    class QcomLPCurrentAmountAction : public Action
    {
    public:
        QcomLPCurrentAmountAction();
       ~QcomLPCurrentAmountAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t     LPData(uint32_t *lpamt, uint16_t *pgid, uint8_t *plvl);

    private:
        static std::vector<uint32>      s_lpamt;
        static std::vector<uint16>      s_pgid;
    };

    class QcomGeneralPromotionalAction : public Action
    {
    public:
        QcomGeneralPromotionalAction();
       ~QcomGeneralPromotionalAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        std::string Text() const { return s_text; }

    private:
        static std::string s_text;
    };

    class QcomSiteDetailAction : public Action
    {
    public:
        QcomSiteDetailAction();
       ~QcomSiteDetailAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        std::string SText() const { return s_stext; }
        std::string LText() const { return s_ltext; }

    private:
        static std::string s_stext;
        static std::string s_ltext;

    };
}


#endif

