
#include "Core.hpp"
#include "Utils.hpp"

#include "Qcom/QcomBroadcast.hpp"
#include "Qcom/QcomEgmConfigReq.hpp"
#include "Qcom/qogr/qogr_crc.h"

namespace sg 
{

    namespace 
    {
        bool FindEGMDataBySER(uint32_t ser, QcomDataPtr p)
        {
            std::unique_lock<std::mutex> lock(p->locker);
            if (p->data.serialMidBCD == ser)
            {
                return true;
            }

            return false;
        }
    }

    uint8_t QcomEgmConfigurationRequest::Id() const
    {
        return QCOM_EGMCRP_FC;
    }

    bool QcomEgmConfigurationRequest::Parse(uint8_t buf[], int length)
    {
        SG_UNREF_PARAM(length);

        if (auto it = m_qcom.lock())
        {
            QCOM_RespMsgType *p = (QCOM_RespMsgType*)buf;
            if (p->DLL.Length >= QCOM_GET_PACKET_LENGTH(sizeof(qc_egmcrtype2)))
            {
                QcomDataPtr pd = it->FindEgmData(std::bind(&FindEGMDataBySER, p->Data.egmcr2.SN.SER, std::placeholders::_1));

                if (pd)
                {
                    std::unique_lock<std::mutex> _lock(pd->locker);

                    pd->data.last_control ^= (QCOM_ACK_MASK);

                    // TODO : the following fields must not change
                    // once reported by the egm in the first egm configuration response
                    // except NUMG, DEN, TOK & CRC
                    pd->data.protocol_ver = p->Data.egmcr2.NPRV;
                    pd->data.egm_config_flag_a = p->Data.egmcr2.FLGA.FLGA;
                    pd->data.egm_config_flag_b = p->Data.egmcr2.FLGB.FLGB;
                    pd->data.base_gvn = p->Data.egmcr2.BGVN;
                    pd->data.total_num_games = p->Data.egmcr2.NUMG;
                    pd->data.total_num_games_enable = p->Data.egmcr2.NUME;
                    pd->data.last_gvn = p->Data.egmcr2.LGVN;
                    pd->data.last_var = p->Data.egmcr2.LVAR;
                    pd->data.flgsh = p->Data.egmcr2.FLGSH.FLGSH;

                    // The following fields are set via the EGM Configuration Poll and reported
                    // back here for verification
                    COMMS_START_LOG_BLOCK(); // TODO : we need to prevent potential dead lock here when lock pd->locker at the same time
                    if (pd->data.egm_config.den != p->Data.egmcr2.DEN2)
                    {
                        COMMS_LOG_BLOCK(
                            boost::format("Invalid den value %1% is returned from EGM, should be %2%\n") %
                            p->Data.egmcr2.DEN2 %
                            pd->data.egm_config.den, CLL_Error);
                    }

                    if (pd->data.egm_config.tok != p->Data.egmcr2.TOK2)
                    {
                        COMMS_LOG_BLOCK(
                                 boost::format("Invalid tok value %1% is returned from EGM, should be %2%\n") %
                                 p->Data.egmcr2.TOK2 %
                                 pd->data.egm_config.tok, CLL_Error);
                    }

                    if (pd->data.egm_config.maxden != p->Data.egmcr2.MAXDEN)
                    {
                        COMMS_LOG_BLOCK(
                                 boost::format("Invalid maxden value %1% is returned from EGM, should be %2%\n") %
                                 p->Data.egmcr2.MAXDEN %
                                 pd->data.egm_config.maxden, CLL_Error);
                    }

                    if (pd->data.egm_config.minrtp != p->Data.egmcr2.MINRTP)
                    {
                        COMMS_LOG_BLOCK(
                                 boost::format("Invalid minrtp value %1% is returned from EGM, should be %2%\n") %
                                 p->Data.egmcr2.MINRTP %
                                 pd->data.egm_config.minrtp, CLL_Error);
                    }

                    if (pd->data.egm_config.maxrtp != p->Data.egmcr2.MAXRTP)
                    {
                        COMMS_LOG_BLOCK(
                                 boost::format("Invalid maxrtp value %1% return from EGM, should be %2%\n") %
                                 p->Data.egmcr2.MAXRTP%
                                 pd->data.egm_config.maxrtp, CLL_Error);
                    }

                    if (pd->data.egm_config.maxsd != p->Data.egmcr2.MAXSD)
                    {
                        COMMS_LOG_BLOCK(
                                 boost::format("Invalid maxsd value %1% return from EGM, should be %2%\n") %
                                 p->Data.egmcr2.MAXSD%
                                 pd->data.egm_config.maxsd, CLL_Error);
                    }

                    if (pd->data.egm_config.maxlines != p->Data.egmcr2.MAXLINES)
                    {
                        COMMS_LOG_BLOCK(
                                 boost::format("Invalid maxlines value %1% return from EGM, should be %2%\n") %
                                 p->Data.egmcr2.MAXLINES %
                                 pd->data.egm_config.maxlines, CLL_Error);
                    }

                    if (pd->data.egm_config.maxbet != p->Data.egmcr2.MAXBET)
                    {
                        COMMS_LOG_BLOCK(
                                 boost::format("Invalid maxbet value %1% return from EGM, should be %2%\n") %
                                 p->Data.egmcr2.MAXBET %
                                 pd->data.egm_config.maxbet, CLL_Error);
                    }

                    if (pd->data.egm_config.maxnpwin != p->Data.egmcr2.MAXNPWIN)
                    {
                        COMMS_LOG_BLOCK(
                                 boost::format("Invalid maxnpwin value %1% return from EGM, should be %2%\n") %
                                 p->Data.egmcr2.MAXNPWIN %
                                 pd->data.egm_config.maxnpwin, CLL_Error);
                    }

                    if (pd->data.egm_config.maxpwin != p->Data.egmcr2.MAXPWIN)
                    {
                        COMMS_LOG_BLOCK(
                                 boost::format("Invalid maxpwin value %1% return from EGM, should be %2%\n") %
                                 p->Data.egmcr2.MAXPWIN %
                                 pd->data.egm_config.maxpwin, CLL_Error);
                    }

                    if (pd->data.egm_config.maxect != p->Data.egmcr2.MAXECT)
                    {
                        COMMS_LOG_BLOCK(
                                 boost::format("Invalid maxect value %1% return from EGM, should be %2%\n") %
                                 p->Data.egmcr2.MAXECT %
                                 pd->data.egm_config.maxect, CLL_Error);
                    }
                    COMMS_END_LOG_BLOCK();

                    return true;
                }
            }
        }

        return false;

    }

    QcomPollPtr QcomEgmConfigurationRequest::MakeEGMConfigReqPoll(uint8_t poll_address, uint8_t last_control, uint16_t mef, uint16_t gcr, uint16_t psn)
    {
        QcomPollPtr poll = MakeSharedPtr<QcomPoll>();

        std::memset(poll.get(), 0, sizeof(QcomPoll));

        // ref Qcom1.6-15.4.1
        poll->poll.DLL.PollAddress = poll_address;
        poll->poll.DLL.Length = QCOM_DLL_HEADER_SIZE + sizeof(qc_egmcrptype);
        poll->poll.DLL.ControlByte.CNTL = last_control;
        poll->poll.DLL.FunctionCode = QCOM_EGMCRP_FC;
        poll->poll.Data.egmcrp.MEF = mef;
        poll->poll.Data.egmcrp.GCR = gcr;
        poll->poll.Data.egmcrp.PSN = psn;
        PutCRC_LSBfirst(poll->data, poll->poll.DLL.Length);
        poll->length = poll->poll.DLL.Length + QCOM_CRC_SIZE;

        return poll;
    }

    void QcomEgmConfigurationRequest::BuildEGMConfigReqPoll(uint8_t poll_address, uint8_t mef, uint8_t gcr, uint8_t psn)
    {
        if (auto it = m_qcom.lock())
        {
            QcomDataPtr p = it->GetEgmData(poll_address);
            if (p)
            {
                QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_POLL);

                std::unique_lock<std::mutex> lock(p->locker);

                if (p->data.poll_address == 0)
                {
                    QcomBroadcastPtr pb = std::static_pointer_cast<QcomBroadcast>(it->GetHandler(QCOM_BROADCAST_ADDRESS));
                    if (pb)
                    {
                        pb->BuildPollAddressPoll();
                    }
                }

                if (p->data.resp_funcode == QCOM_NO_RESPONSE)
                    p->data.last_control ^= (QCOM_ACK_MASK);

                job->AddPoll(this->MakeEGMConfigReqPoll(poll_address, p->data.last_control, mef, gcr, psn));

                p->data.resp_funcode = QCOM_EGMCRP_FC;
                p->data.machine_enable = mef;
                p->data.game_config_req = gcr;
                if (psn)
                {
                    p->data.poll_seq_num[PSN_EVENTS] = 0;
                    p->data.poll_seq_num[PSN_ECT] = 0;
                }

                it->AddJob(job);
            }

        }
    }

    void QcomEgmConfigurationRequest::BuildEGMConfigReqPoll(std::vector<QcomEGMConifgReqCustomData> const& data)
    {
        if (auto it = m_qcom.lock())
        {
            bool pac_sent = false;

            QcomJobDataPtr job = MakeSharedPtr<QcomJobData>(QcomJobData::JT_POLL);

            for(auto const& d : data)
            {
                QcomDataPtr p = it->GetEgmData(d.egm);

                if (p)
                {
                    std::unique_lock<std::mutex> lock(p->locker);

                    if (!pac_sent && p->data.poll_address == 0)
                    {
                        QcomBroadcastPtr pb = std::static_pointer_cast<QcomBroadcast>(it->GetHandler(QCOM_BROADCAST_ADDRESS));
                        if (pb)
                        {
                            // TODO : it's better broadcast supply a function that
                            // it can just config specified egm poll address instead of all of them
                            pb->BuildPollAddressPoll();
                        }

                        pac_sent = true;
                    }

                    if (p->data.resp_funcode == QCOM_NO_RESPONSE)
                        p->data.last_control ^= (QCOM_ACK_MASK);

                    job->AddPoll(this->MakeEGMConfigReqPoll(d.egm, p->data.last_control, d.mef, d.gcr, d.psn));

                    p->data.resp_funcode = QCOM_EGMCRP_FC;
                    p->data.machine_enable = d.mef;
                    p->data.game_config_req = d.gcr;
                    if (d.psn)
                    {
                        p->data.poll_seq_num[PSN_EVENTS] = 0;
                        p->data.poll_seq_num[PSN_ECT] = 0;
                    }
                }
            }

            it->AddJob(job);

        }
    }

}


