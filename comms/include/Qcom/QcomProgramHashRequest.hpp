#ifndef __SG_QCOM_PROGRAM_HASH_HPP__
#define __SG_QCOM_PROGRAM_HASH_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"


namespace sg
{
    class QcomProgHashRequest : public CommsPacketHandler
    {
    public:
        QcomProgHashRequest(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;
        uint8_t     RespId() const override;
        bool        Parse(uint8_t buf[], int length) override;

    public:
        bool        BuildProgHashRequstPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomProgHashRequestData const& data);

    private:
        QcomPollPtr MakeProgHashRequestPoll(uint8_t poll_address, uint8_t last_control, QcomProgHashRequestData const& data);

    private:
        std::weak_ptr<CommsQcom>    m_qcom;
    };

    typedef std::shared_ptr<QcomProgHashRequest> QcomProgHashRequestPtr;
}





#endif // !__SG_QCOM_PROGRAM_HASH_HPP__



