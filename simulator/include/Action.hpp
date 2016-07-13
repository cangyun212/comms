#ifndef __SG_ACTION_HPP__
#define __SG_ACTION_HPP__

#include "Core.hpp"

#include <string>
#include <vector>

#include "boost/program_options.hpp"

#include "Utils.hpp"
#include "Predeclare.hpp"

namespace sg 
{
    class ActionOptions;
    typedef std::shared_ptr<ActionOptions>     ActionOptionsPtr;

    struct ActionParsedOption
    {
        std::string     key;
        std::string     token;
    };
    typedef std::vector<ActionParsedOption>     ActionParsedOptions;

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
            AT_QCOM_TIME_DATE,
            AT_QCOM_LP_CURRENT_AMOUNT,
            AT_QCOM_GENERAL_PROMOTIONAL,
            AT_QCOM_SITE_DETAIL,
            AT_QCOM_EGM_CONF_REQ,
            AT_QCOM_EGM_CONF,
            AT_QCOM_GAME_CONF,
            AT_QCOM_GAME_CONF_CHANGE,
            AT_QCOM_EGM_PARAMS,
            AT_QCOM_PURGE_EVENTS,
            AT_QCOM_GENERAL_STATUS,
            AT_QCOM_PROGRESSIVE_CONFIG,
            AT_QCOM_PENDING,
            AT_QCOM_SEND,
 
            AT_HELP,
            AT_NUM
        };

    public:
        Action(ActionType type) : m_type(type), m_options(nullptr) {}
        virtual ~Action(){}

    public:
        ActionType  GetType() const { return m_type; }

    public:
        typedef std::vector<std::string>    ActionArgs;
        virtual bool Parse(ActionArgs const& args);
        virtual void BuildOptions() {}
        virtual const char* Description() const = 0;
        virtual ActionPtr Clone() = 0;

        bool TryParse(ActionArgs const& args, ActionParsedOptions &options, bool ignore_pos = true);
        void GetAllOptionsName(std::vector<std::string> &names, bool long_name = true) const;

    protected:
        template<typename T>
        ActionPtr   DoClone()
        {
            std::shared_ptr<T> ptr = MakeSharedPtr<T>(*((T*)this));

            return ptr;
        }

    protected:
        ActionType          m_type;
        ActionOptionsPtr    m_options;
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
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        bool        ListAll() const { return s_list_all; }

    private:
        static bool        s_list_all;
    };

    class PickEGMAction : public Action
    {
    public:
        PickEGMAction();
       ~PickEGMAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t     Target() const { return s_egm; }

    private:
        static uint8       s_egm;
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
    public:
        ResetDevAction();
       ~ResetDevAction();

    public:
        ActionPtr   Clone() override;
        bool        Parse(const ActionArgs &args) override;
        void        BuildOptions() override;
        const char* Description() const override;

        const std::string  & GetDev() { return s_dev; }
    private:
        static std::string s_dev;
    };
}

#endif


