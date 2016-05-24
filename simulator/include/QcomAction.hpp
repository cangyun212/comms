#ifndef __SG_QCOM_ACTION_HPP__
#define __SG_QCOM_ACTION_HPP__

#include "Core.hpp"

#include <vector>

#include "Predeclare.hpp"
#include "Action.hpp"


namespace sg 
{

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

    class QcomEGMConfRequestAction : public Action
    {
    public:
        QcomEGMConfRequestAction();
       ~QcomEGMConfRequestAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t     MEF() const { return m_mef; }
        uint8_t     GCR() const { return m_gcr; }
        uint8_t     PSN() const { return m_psn; }

    private:
       static uint8       m_mef;
       static uint8       m_gcr;
       static uint8       m_psn;

    };

    class QcomEGMConfAction : public Action
    {
    public:
        QcomEGMConfAction();
        ~QcomEGMConfAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t     JUR() const { return m_jur; }
        uint32_t    DEN() const { return m_den; }
        uint32_t    TOK() const { return m_tok; }
        uint32_t    MAXDEN() const { return m_maxden; }
        uint16_t    MINRTP() const { return m_minrtp; }
        uint16_t    MAXRTP() const { return m_maxrtp; }
        uint16_t    MAXSD() const { return m_maxsd; }
        uint16_t    MAXLINES() const { return m_maxlines; }
        uint32_t    MAXBET() const { return m_maxbet; }
        uint32_t    MAXNPWIN() const { return m_maxnpwin; }
        uint32_t    MAXPWIN() const { return m_maxpwin; }
        uint32_t    MAXECT() const { return m_maxect; }

    private:
        static uint8   m_jur;
        static uint32  m_den;
        static uint32  m_tok;
        static uint32  m_maxden;
        static uint16  m_minrtp;
        static uint16  m_maxrtp;
        static uint16  m_maxsd;
        static uint16  m_maxlines;
        static uint32  m_maxbet;
        static uint32  m_maxnpwin;
        static uint32  m_maxpwin;
        static uint32  m_maxect;
    };

    class QcomBroadcastAction : public Action
    {
    public:
        QcomBroadcastAction();
        ~QcomBroadcastAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint32_t    GetBroadcastType() const { return m_broadcast_type; }
        std::string GetGPMBroadcastText() const { return m_gpm_text; }
        std::string GetSDSBroadcastText() const { return m_sds_text; }
        std::string GetSDLBroadcastText() const { return m_sdl_text; }

    private:
        static uint32      m_broadcast_type;
        static std::string m_gpm_text;
        static std::string m_sds_text;
        static std::string m_sdl_text;
    };

    class QcomGameConfigurationAction : public Action
    {
    public:
        QcomGameConfigurationAction();
        ~QcomGameConfigurationAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t     VAR() const { return m_var; }
        uint8_t     VAR_LOCK() const { return m_var_lock; }
        uint8_t     GAME_ENABLE() const { return m_game_enable; }
        uint8_t     PNMUM() const { return m_pnum; }
        void        LP(std::vector<uint8_t> & lp) const;
        void        CAMT(std::vector<uint32_t> & camt) const;

    private:
        static uint8                   m_var;
        static uint8                   m_var_lock;
        static uint8                   m_game_enable;
        static uint8                   m_pnum;
        static std::vector<uint8>      m_lp;
        static std::vector<uint32>     m_camt;
    };



    class QcomGameConfigurationChangeAction : public Action
    {
    public:
        QcomGameConfigurationChangeAction();
        ~QcomGameConfigurationChangeAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t    VAR() const { return m_var; }
        uint8_t    GAME_ENABLE() const { return m_game_enable; }

    private:
        static uint8  m_var;
        static uint8  m_game_enable;
    };

    class QcomEGMParametersAction : public Action
    {
    public:
        QcomEGMParametersAction();
        ~QcomEGMParametersAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t  RESERVE() const { return m_reserve; }
        uint8_t  AUTOPLAY() const { return m_autoplay; }
        uint8_t  CRLIMITMODE() const { return m_crlimitmode; }
        uint8_t  OPR() const { return m_opr; }
        uint32_t LWIN() const { return m_lwin; }
        uint32_t CRLIMIT() const { return m_crlimit; }
        uint8_t  DUMAX() const { return m_dumax; }
        uint32_t DULIMIT() const { return m_dulimit; }
        uint16_t TZADJ() const { return m_tzadj; }
        uint32_t PWRTIME() const { return m_pwrtime; }
        uint8_t  PID() const { return m_pid; }
        uint16_t EODT() const { return m_eodt; }
        uint32_t NPWINP() const { return m_npwinp; }
        uint32_t SAPWINP() const { return m_sapwinp; }

    private:
        static  uint8    m_reserve;
        static  uint8    m_autoplay;
        static  uint8    m_crlimitmode;
        static  uint8    m_opr;
        static  uint32   m_lwin;
        static  uint32   m_crlimit;
        static  uint8    m_dumax;
        static  uint32   m_dulimit;
        static  uint16   m_tzadj;
        static  uint32   m_pwrtime;
        static  uint8    m_pid;
        static  uint16   m_eodt;
        static  uint32   m_npwinp;
        static  uint32   m_sapwinp;
    };

    class QcomPurgeEventsAction : public Action
    {
    public:
        QcomPurgeEventsAction();
        ~QcomPurgeEventsAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t    PSN() const { return m_psn; }
        uint8_t    EVTNO() const { return m_evtno; }

    private:
        static uint8  m_psn;
        static uint8  m_evtno;
    };
}


#endif

