
#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomBroadcast.hpp"
#include "Qcom/QcomEgmConfigReq.hpp"
#include "Qcom/qogr/qogr_crc.h"

#define SG_QCOM_EGM_CONFIG_DEN          (0x1)
#define SG_QCOM_EGM_CONFIG_TOK          (0x1 << 1)
#define SG_QCOM_EGM_CONFIG_MAXDEN       (0x1 << 2)
#define SG_QCOM_EGM_CONFIG_MINRTP       (0x1 << 3)
#define SG_QCOM_EGM_CONFIG_MAXRTP       (0x1 << 4)
#define SG_QCOM_EGM_CONFIG_MAXSD        (0x1 << 5)
#define SG_QCOM_EGM_CONFIG_MAXLINES     (0x1 << 6)
#define SG_QCOM_EGM_CONFIG_MAXBET       (0x1 << 7)
#define SG_QCOM_EGM_CONFIG_MAXNPWIN     (0x1 << 8)
#define SG_QCOM_EGM_CONFIG_MAXPWIN      (0x1 << 9)
#define SG_QCOM_EGM_CONFIG_MAXECT       (0x1 << 10)

namespace sg 
{

    uint8_t QcomEgmConfigurationRequest::Id() const
    {
        return QCOM_EGMCRP_FC;
    }

    uint8_t QcomEgmConfigurationRequest::RespId() const
    {
        return QCOM_EGMCR_FC;
    }

    bool QcomEgmConfigurationRequest::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);

        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;
            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_egmcrtype2)))
            {
                QcomDataPtr pd = it->GetEgmData(p->DLL.PollAddress);

                bool invalid_ser = false;
                uint32_t flag = 0;
                QcomEGMConfigPollData data;
                if (pd)
                {
                    std::unique_lock<std::mutex> _lock(pd->locker);

                    if (pd->data.control.serialMidBCD == p->Data.egmcr2.SN.SER)
                    {
                        pd->data.control.last_control ^= (QCOM_ACK_MASK);

                        // TODO : the following fields must not change
                        // once reported by the egm in the first egm configuration response
                        // except NUMG, DEN, TOK & CRC

                        pd->data.config.games_num = p->Data.egmcr2.NUMG;
                        if (!(pd->data.control.egm_config_state & QCOM_EGM_CONFIG_READY))
                        {
                            pd->data.control.protocol_ver = p->Data.egmcr2.NPRV;
                            pd->data.config.flag_a = p->Data.egmcr2.FLGA.FLGA;
                            pd->data.config.flag_b = p->Data.egmcr2.FLGB.FLGB;
                            pd->data.config.bsvn = p->Data.egmcr2.BGVN;
                            pd->data.config.last_gvn = p->Data.egmcr2.LGVN;
                            pd->data.config.last_var = p->Data.egmcr2.LVAR;
                            pd->data.config.flag_s = p->Data.egmcr2.FLGSH.FLGSH;
                            pd->data.config.games_num_enable = p->Data.egmcr2.NUME;

                            pd->data.control.egm_config_state |= QCOM_EGM_CONFIG_READY;

                            // The following fields are set via the EGM Configuration Poll and
                            // reported back here for verification
                            if (pd->data.custom.den != p->Data.egmcr2.DEN2)
                                flag |= SG_QCOM_EGM_CONFIG_DEN;

                            if (pd->data.custom.tok != p->Data.egmcr2.TOK2)
                                flag |= SG_QCOM_EGM_CONFIG_TOK;

                            if (pd->data.custom.maxden != p->Data.egmcr2.MAXDEN)
                                flag |= SG_QCOM_EGM_CONFIG_MAXDEN;

                            if (pd->data.custom.minrtp != p->Data.egmcr2.MINRTP)
                                flag |= SG_QCOM_EGM_CONFIG_MINRTP;

                            if (pd->data.custom.maxrtp != p->Data.egmcr2.MAXRTP)
                                flag |= SG_QCOM_EGM_CONFIG_MAXRTP;

                            if (pd->data.custom.maxsd != p->Data.egmcr2.MAXSD)
                                flag |= SG_QCOM_EGM_CONFIG_MAXSD;

                            if (pd->data.custom.maxlines != p->Data.egmcr2.MAXLINES)
                                flag |= SG_QCOM_EGM_CONFIG_MAXLINES;

                            if (pd->data.custom.maxbet != p->Data.egmcr2.MAXBET)
                                flag |= SG_QCOM_EGM_CONFIG_MAXBET;

                            if (pd->data.custom.maxnpwin != p->Data.egmcr2.MAXNPWIN)
                                flag |= SG_QCOM_EGM_CONFIG_MAXNPWIN;

                            if (pd->data.custom.maxpwin != p->Data.egmcr2.MAXPWIN)
                                flag |= SG_QCOM_EGM_CONFIG_MAXPWIN;

                            if (pd->data.custom.maxect != p->Data.egmcr2.MAXECT)
                                flag |= SG_QCOM_EGM_CONFIG_MAXECT;

                            if (flag)
                                data = pd->data.custom;
                        }
                    }
                    else
                    {
                        invalid_ser = true;
                    }

                }

                if (invalid_ser)
                {
                    COMMS_LOG(boost::format("Invalid serial MID value %1% is returned for EGM at poll address %2%\n") %
                        p->Data.egmcr2.SN.SER % static_cast<uint32_t>(p->DLL.PollAddress), CLL_Error);

                    return false;
                }

                if (flag)
                {
                    COMMS_START_LOG_BLOCK();

                    if (flag & SG_QCOM_EGM_CONFIG_DEN)
                        COMMS_LOG_BLOCK(boost::format("Invalid den value %1% is returned from EGM, should be %2%\n") %
                            p->Data.egmcr2.DEN2 % data.den, CLL_Error);

                    if (flag & SG_QCOM_EGM_CONFIG_TOK)
                        COMMS_LOG_BLOCK(boost::format("Invalid tok value %1% is returned from EGM, should be %2%\n") %
                            p->Data.egmcr2.TOK2 % data.tok, CLL_Error);

                    if (flag & SG_QCOM_EGM_CONFIG_MAXDEN)
                        COMMS_LOG_BLOCK(boost::format("Invalid maxden value %1% is returned from EGM, should be %2%\n") %
                            p->Data.egmcr2.MAXDEN % data.maxden, CLL_Error);

                    if (flag & SG_QCOM_EGM_CONFIG_MINRTP)
                        COMMS_LOG_BLOCK(boost::format("Invalid minrtp value %1% is returned from EGM, should be %2%\n") %
                            p->Data.egmcr2.MINRTP % data.minrtp, CLL_Error);

                    if (flag & SG_QCOM_EGM_CONFIG_MAXRTP)
                        COMMS_LOG_BLOCK(boost::format("Invalid maxrtp value %1% is returned from EGM, should be %2%\n") %
                            p->Data.egmcr2.MAXRTP % data.maxrtp, CLL_Error);

                    if (flag & SG_QCOM_EGM_CONFIG_MAXSD)
                        COMMS_LOG_BLOCK(boost::format("Invalid maxsd value %1% is returned from EGM, should be %2%\n") %
                            p->Data.egmcr2.MAXSD % data.maxsd, CLL_Error);

                    if (flag & SG_QCOM_EGM_CONFIG_MAXLINES)
                        COMMS_LOG_BLOCK(boost::format("Invalid maxlines value %1% is returned from EGM, should be %2%\n") %
                            p->Data.egmcr2.MAXLINES % data.maxlines, CLL_Error);

                    if (flag & SG_QCOM_EGM_CONFIG_MAXBET)
                        COMMS_LOG_BLOCK(boost::format("Invalid maxbet value %1% is returned from EGM, should be %2%\n") %
                            p->Data.egmcr2.MAXBET % data.maxbet, CLL_Error);

                    if (flag & SG_QCOM_EGM_CONFIG_MAXNPWIN)
                        COMMS_LOG_BLOCK(boost::format("Invalid maxnpwin value %1% is returned from EGM, should be %2%\n") %
                            p->Data.egmcr2.MAXNPWIN % data.maxnpwin, CLL_Error);

                    if (flag & SG_QCOM_EGM_CONFIG_MAXPWIN)
                        COMMS_LOG_BLOCK(boost::format("Invalid maxpwin value %1% is returned from EGM, should be %2%\n") %
                            p->Data.egmcr2.MAXPWIN % data.maxpwin, CLL_Error);

                    if (flag & SG_QCOM_EGM_CONFIG_MAXECT)
                        COMMS_LOG_BLOCK(boost::format("Invalid maxect value %1% is returned from EGM, should be %2%\n") %
                            p->Data.egmcr2.MAXECT % data.maxect, CLL_Error);

                    COMMS_END_LOG_BLOCK();

                    return false;
                }

                COMMS_LOG("EGM Configuration Response received\n", CLL_Info);
                return true;
            }
        }

        return false;

    }

    bool QcomEgmConfigurationRequest::BuildEGMConfigReqPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomEGMControlPollData const & data)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);
            if (p)
            {
                std::unique_lock<std::mutex> lock(p->locker);

                job->AddPoll(this->MakeEGMConfigReqPoll(poll_address, p->data.control.last_control, data));

                p->data.control.machine_eable = data.mef;

                if (data.gcr)
                {
                    if (p->data.config.games_num > 0)
                    {
                        for (uint8_t i = 0; i < p->data.config.games_num; ++i)
                        {
                            p->data.control.game_config_state[i] |= QCOM_GAME_CONFIG_REQ;
                        }
                    }
                    else
                    {
                        for (auto & s : p->data.control.game_config_state)
                        {
                            s |= QCOM_GAME_CONFIG_REQ;
                        }
                    }
                }

                if (data.psn)
                {
                    p->data.control.psn[Qcom_PSN_Events] = QCOM_RESET_PSN;
                    p->data.control.psn[Qcom_PSN_ECT] = QCOM_RESET_PSN;
                }

                return true;
            }
        }

        return false;
    }

    QcomPollPtr QcomEgmConfigurationRequest::MakeEGMConfigReqPoll(uint8_t poll_address, uint8_t last_control, QcomEGMControlPollData const& data)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        // ref Qcom1.6-15.4.1
        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_egmcrptype);
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_EGMCRP_FC;
        poll->poll.Data.egmcrp.MEF = data.mef;
        poll->poll.Data.egmcrp.GCR = data.gcr;
        poll->poll.Data.egmcrp.PSN = data.psn;
        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }

}


