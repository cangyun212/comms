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
        bool        Parse(const ActionArgs &args, const ActionError **err) override;
        ActionPtr   Clone() override;
        const char* Description() const override;

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
        bool        Parse(const ActionArgs &args, const ActionError **err) override;
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
        uint8_t   m_jur;
        uint32_t  m_den;
        uint32_t  m_tok;
        uint32_t  m_maxden;
        uint16_t  m_minrtp;
        uint16_t  m_maxrtp;
        uint16_t  m_maxsd;
        uint16_t  m_maxlines;
        uint32_t  m_maxbet;
        uint32_t  m_maxnpwin;
        uint32_t  m_maxpwin;
        uint32_t  m_maxect;
    };

    class QcomBroadcastAction : public Action
    {
    public:
        QcomBroadcastAction();
        ~QcomBroadcastAction();

    public:
        bool        Parse(const ActionArgs &args, const ActionError **err) override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint32_t    GetBroadcastType() const { return m_broadcast_type; }
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
        bool        Parse(const ActionArgs &args, const ActionError **err) override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t    VAR() const { return m_var; }
        uint8_t    VAR_LOCK() const { return m_var_lock; }
        uint8_t    GAME_ENABLE() const { return m_game_enable; }
        uint8_t    PNMUM() const { return m_pnum; }
        std::vector<uint8_t> const&    LP() const { return m_lp; }
        std::vector<uint32_t> const&   CAMT() const { return m_camt; }

    private:
        uint8_t                 m_var;
        uint8_t                 m_var_lock;
        uint8_t                 m_game_enable;
        uint8_t                 m_pnum;
        std::vector<uint8_t>    m_lp;
        std::vector<uint32_t>   m_camt;
    };

}


#endif

