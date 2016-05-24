#ifndef __ACTION_HPP__
#define __ACTION_HPP__

#include "Core.hpp"

#include <string>
#include <vector>

#include "boost/program_options.hpp"

#include "Utils.hpp"
#include "Predeclare.hpp"

namespace sg 
{
    class ValueSematic
    {
    public:
        ValueSematic(){}
        virtual ~ValueSematic(){}

        virtual boost::program_options::value_semantic* value() const = 0;
    };

    typedef std::shared_ptr<ValueSematic> ValueSematicPtr;

    template <typename T>
    class OptionValue : public ValueSematic
    {
    public:
        OptionValue(T* value) : m_value(value){}
        ~OptionValue(){}

        boost::program_options::value_semantic * value() const override 
        {
            boost::program_options::typed_value<T> *value_ptr = new boost::program_options::typed_value<T>(m_value);

            return value_ptr;
        }

    private:
        T* m_value;
    };

    template<typename T>
    ValueSematicPtr Value(T* value)
    {
        return MakeSharedPtr<OptionValue<T> >(value);
    }

    template<typename T>
    ValueSematicPtr Value()
    {
        return MakeSharedPtr<OptionValue<T> >(nullptr);
    }

    struct ActionOption
    {
        ActionOption(std::string const& n, ValueSematicPtr const& v, std::string m) : name(n), value(v), message(m){}

        std::string name;
        ValueSematicPtr value;
        std::string message;
    };

    typedef std::shared_ptr<ActionOption> ActionOptionPtr;

    struct ActionPosOption
    {
        ActionPosOption(std::string const& option, ValueSematicPtr const& v, int c) : option(option), value(v), max_count(c) {}

        std::string option;
        ValueSematicPtr value;
        int max_count;
    };

    typedef std::shared_ptr<ActionPosOption>    ActionPosOptionPtr;
    typedef std::vector<ActionOptionPtr> ActionOptions;
    typedef std::shared_ptr<ActionOptions> ActionOptionsPtr;

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
            AT_QCOM_GAME_CONF_CHANGE,
            AT_QCOM_EGM_PARAMS,
            AT_QCOM_PURGE_EVENTS,
 
            AT_HELP,
            AT_NUM
        };

    public:
        Action(ActionType type)
            : m_type(type), m_options(nullptr), m_pos_options(nullptr){}
        virtual ~Action(){}

    public:
        ActionType  GetType() const { return m_type; }

    public:
        typedef std::vector<std::string>    ActionArgs;
        virtual bool Parse(ActionArgs const& args);
        virtual const char* Description() const = 0;

        virtual ActionPtr Clone() = 0;

    public:
        ActionOptionsPtr GetOptions(){ return m_options; }

    protected:
        template<typename T>
        ActionPtr   DoClone()
        {
            std::shared_ptr<T> ptr = MakeSharedPtr<T>(*((T*)this));

            return ptr;
        }
        void AddOption(std::string const& option, ValueSematicPtr const& value = nullptr, std::string const& message = std::string());
        void AddPosOption(ActionPosOptionPtr const& option);
        void FillOptionsDescription(boost::program_options::options_description &desc,
                                    boost::program_options::options_description &vis_desc,
                                    boost::program_options::positional_options_description &pos_desc);
        void FillOptionsDescription(boost::program_options::options_description &desc,
                                    boost::program_options::options_description &vis_desc);

    protected:
        ActionType  m_type;
		
		typedef std::vector<ActionPosOptionPtr> VectorPosOptionPtr;
        typedef std::shared_ptr<VectorPosOptionPtr> ActionPosOptionsPtr;

        ActionOptionsPtr        m_options;
        ActionPosOptionsPtr     m_pos_options;
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
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        bool        ListAll() const { return m_list_all; }

    private:
        static bool        m_list_all;
    };

    class PickEGMAction : public Action
    {
    public:
        PickEGMAction();
       ~PickEGMAction();

    public:
        bool        Parse(const ActionArgs &args) override;
        ActionPtr   Clone() override;
        const char* Description() const override;

    public:
        uint8_t     Target() const { return m_egm; }

    private:
        static uint8       m_egm;
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
            ActionPtr   Clone() override;
            bool        Parse(const ActionArgs &args) override;
            const char* Description() const override;

            const std::string  & GetDev()  { return m_dev; }
        private:
            static std::string m_dev;
    };
}

#endif


