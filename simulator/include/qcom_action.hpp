#ifndef __QCOM_ACTION_HPP__
#define __QCOM_ACTION_HPP__

#include "core/core.hpp"

#include <vector>

#include "simulator/predeclare.hpp"
#include "simulator/action.hpp"


namespace sg {

    class QcomSeekEGMAction : public Action
    {
    public:
        QcomSeekEGMAction();
       ~QcomSeekEGMAction();

    public:
        ActionPtr   Clone() CORE_OVERRIDE;
        const char* Description() const CORE_OVERRIDE;

    private:

    };

    class QcomEGMPollAddConfAction : public Action
    {
    public:
        QcomEGMPollAddConfAction();
       ~QcomEGMPollAddConfAction();

    public:
        ActionPtr   Clone() CORE_OVERRIDE;
        const char* Description() const CORE_OVERRIDE;

    private:

    };

    class QcomEGMConfRequestAction : public Action
    {
    public:
        QcomEGMConfRequestAction();
       ~QcomEGMConfRequestAction();

    public:
        bool        Parse(const ActionArgs &args, const ActionError **err) CORE_OVERRIDE;
        ActionPtr   Clone() CORE_OVERRIDE;
        const char* Description() const CORE_OVERRIDE;

    public:
        uint8_t     MEF() const { return m_mef; }
        uint8_t     GCR() const { return m_gcr; }
        uint8_t     PSN() const { return m_psn; }

    private:
        uint8_t    m_mef;
        uint8_t    m_gcr;
        uint8_t    m_psn;

    };

    class QcomEGMConfAction : public Action
    {
    public:
        QcomEGMConfAction();
        ~QcomEGMConfAction();

    public:
        bool        Parse(const ActionArgs &args, const ActionError **err) CORE_OVERRIDE;
        ActionPtr   Clone() CORE_OVERRIDE;
        const char* Description() const CORE_OVERRIDE;

    public:
        int    JUR() const { return m_jur; }
        int    DEN() const { return m_den; }
        int    TOK() const { return m_tok; }
        int    MAXDEN() const { return m_maxden; }
        int    MINRTP() const { return m_minrtp; }
        int    MAXRTP() const { return m_maxrtp; }
        int    MAXSD() const { return m_maxsd; }
        int    MAXLINES() const { return m_maxlines; }
        int    MAXBET() const { return m_maxbet; }
        int    MAXNPWIN() const { return m_maxnpwin; }
        int    MAXPWIN() const { return m_maxpwin; }
        int    MAXECT() const { return m_maxect; }

    private:
        int   m_jur;
        int   m_den;
        int   m_tok;
        int   m_maxden;
        int   m_minrtp;
        int   m_maxrtp;
        int   m_maxsd;
        int   m_maxlines;
        int   m_maxbet;
        int   m_maxnpwin;
        int   m_maxpwin;
        int   m_maxect;
    };

    class QcomBroadcastAction : public Action
    {
    public:
        QcomBroadcastAction();
        ~QcomBroadcastAction();

    public:
        bool        Parse(const ActionArgs &args, const ActionError **err) CORE_OVERRIDE;
        ActionPtr   Clone() CORE_OVERRIDE;
        const char* Description() const CORE_OVERRIDE;

    public:
        uint32_t     GetBroadcastType() const { return m_broadcast_type; }
        std::string GetGPMBroadcastText() const { return m_gpm_text; }
        std::string GetSDSBroadcastText() const { return m_sds_text; }
        std::string GetSDLBroadcastText() const { return m_sdl_text; }

    private:
        uint32_t  m_broadcast_type;
        std::string m_gpm_text;
        std::string m_sds_text;
        std::string m_sdl_text;
    };

    class QcomGameConfigurationAction : public Action
    {
    public:
        QcomGameConfigurationAction();
        ~QcomGameConfigurationAction();

    public:
        bool        Parse(const ActionArgs &args, const ActionError **err) CORE_OVERRIDE;
        ActionPtr   Clone() CORE_OVERRIDE;
        const char* Description() const CORE_OVERRIDE;

    public:
        int    VAR() const { return m_var; }
        int    VAR_LOCK() const { return m_var_lock; }
        int    GAME_ENABLE() const { return m_game_enable; }
        int    PNMUM() const { return m_pnum; }
        const std::vector<int> &      LP() const { return m_lp; }
        const std::vector<int> &      CAMT() const { return m_camt; }

    private:
        int  m_var;
        int  m_var_lock;
        int  m_game_enable;
        int  m_pnum;
        std::vector<int> m_lp;
        std::vector<int> m_camt;
    };

}


#endif

