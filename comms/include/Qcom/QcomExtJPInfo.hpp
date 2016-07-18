#ifndef __QCOM_EXTJP_INFO_HPP__
#define __QCOM_EXTJP_INFO_HPP__

#include "Core.hpp"

#include "CommsPredeclare.hpp"
#include "Qcom/Qcom.hpp"

namespace sg
{

    class QcomExtJPInfo : public CommsPacketHandler
    {
    public:
        QcomExtJPInfo(CommsQcomPtr q)
            : CommsPacketHandler(false)
            , m_qcom(q)
        {

        }

    public:
        uint8_t     Id() const override;

    public:
        bool        BuildExtJPInfoPoll(QcomJobDataPtr &job, uint8_t poll_address, QcomExtJPInfoData const& data);

    private:
        QcomPollPtr MakeExtJPInfoPoll(uint8_t poll_address, uint8_t last_control, QcomExtJPInfoData const& data);

    private:
        std::weak_ptr<CommsQcom>        m_qcom;
    };

    typedef std::shared_ptr<QcomExtJPInfo>  QcomExtJPInfoPtr;
}



#endif // !__QCOM_EXTJP_INFO_HPP__


