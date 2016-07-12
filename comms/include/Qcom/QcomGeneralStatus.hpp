#ifndef __SG_QCOM_GENERAL_STATUS_HPP__
#define __SG_QCOM_GENERAL_STATUS_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg 
{

    class QcomGeneralStatus : public CommsPacketHandler
    {
    public:
        QcomGeneralStatus(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;
        uint8_t     RespId() const override;
        bool        Parse(uint8_t buf[], int length) override;

    public:
        bool            BuildGeneralStatusPoll(QcomJobDataPtr &job, uint8_t poll_address);
        bool            BuildGeneralStatusPoll(QcomJobDataPtr &job);

    private:
        QcomPollPtr     MakeGeneralStatusPoll(uint8_t poll_address, uint8_t last_control);

    private:
        std::weak_ptr<CommsQcom>     m_qcom;
    };

    typedef std::shared_ptr<QcomGeneralStatus> QcomGenerlStatusPtr;

}

#endif // QCOM_GENERAL_STATUS_HPP

