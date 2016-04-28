#ifndef __ACTION_HPP__
#define __ACTION_HPP__

#include "Core.hpp"

#include <string>
#include <vector>

#include "Utils.hpp"
#include "Predeclare.hpp"

namespace sg 
{
    struct ActionError
    {
        enum ErrorType
        {
            ET_UNKNOWN = 0,
            ET_NO_ERROR,

            ET_NUM
        };

        ActionError()
            : code(ET_UNKNOWN)
        {

        }

        ErrorType   code;

        std::string GetErrStr() const;
    };

    class Action
    {
    public:
        enum ActionType
        {
            AT_QUIT = 0,
            AT_LIST_EGM,
            AT_PICK_EGM,
            AT_RESET_DEV,

            AT_QCOM_SEEKEGM,
            AT_QCOM_EGM_POLL_ADD_CONF,
            AT_QCOM_EGM_CONF_REQ,
            AT_QCOM_EGM_CONF,
            AT_QCOM_BROADCAST,
            AT_QCOM_GAME_CONF,
 
            AT_HELP,
            AT_NUM
        };

    public:
        Action(ActionType type)
            : m_type(type)
        {
        }
        virtual ~Action() {}

    public:
        ActionType  GetType() const { return m_type; }
        ActionError const& GetCurrErr() const { return m_err; }

    public:
        typedef std::vector<std::string>    ActionArgs;
        virtual bool    Parse(ActionArgs const& args, ActionError const** err = nullptr);
        virtual const char* Description() const = 0;

        virtual ActionPtr Clone() = 0;

    protected:
        template<typename T>
        ActionPtr   DoClone()
        {
            std::shared_ptr<T> ptr = MakeSharedPtr<T>(*((T*)this));

            return ptr;
        }

    protected:
        ActionType  m_type;
        ActionError m_err;
    };

    class QuitAction : public Action
    {
    public:
        QuitAction();
       ~QuitAction();

    public:
        ActionPtr   Clone() override;
        const char* Description() const override;
    };

    class ListEGMAction : public Action
    {
    public:
        ListEGMAction();
       ~ListEGMAction();

    public:
        bool        Parse(const ActionArgs &args, const ActionError **err) override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        bool        ListAll() const { return m_list_all; }

    private:
        bool        m_list_all;
    };

    class PickEGMAction : public Action
    {
    public:
        PickEGMAction();
       ~PickEGMAction();

    public:
        bool        Parse(const ActionArgs &args, const ActionError **err) override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t     Target() const { return m_egm; }

    private:
        uint8_t     m_egm;
    };

    class HelpAction : public Action
    {
    public:
        HelpAction();
       ~HelpAction();

    public:
        ActionPtr   Clone() override;
        const char* Description() const override;
    };

    class ResetDevAction : public Action
    {
        public :
            ResetDevAction();
            ~ResetDevAction();

        public:
            ActionPtr Clone() override;
            bool      Parse(const ActionArgs &args, const ActionError **err) override;
            const char* Description() const override;

            const std::string  & GetDev()  { return m_dev; }
        private:
            std::string m_dev;
    };
}

#endif


