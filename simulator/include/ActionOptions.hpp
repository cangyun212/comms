#ifndef __SG_ACTION_OPTIONS_HPP__
#define __SG_ACTION_OPTIONS_HPP__

#include "Core.hpp"

#include <vector>
#include <string>
#include <algorithm>

#include "boost/program_options.hpp"

#include "Utils.hpp"

namespace sg
{
    class ActionValue
    {
    public:
        ActionValue() {}
        virtual ~ActionValue() {}

        virtual boost::program_options::value_semantic* value(bool multitoken) const = 0;
    };

    typedef std::shared_ptr<ActionValue> ActionValuePtr;

    template <typename T>
    class ActionOptionValue : public ActionValue
    {
    public:
        ActionOptionValue(T* value) : m_value(value) {}
       ~ActionOptionValue() {}

        boost::program_options::value_semantic * value(bool multitoken) const override
        {
            boost::program_options::typed_value<T> *p = new boost::program_options::typed_value<T>(m_value);
            if (!multitoken)
                return p;
            else
                return p->multitoken();
        }

    private:
        T* m_value;
    };

    template <typename T>
    ActionValuePtr Value(T *value)
    {
        return MakeSharedPtr<ActionOptionValue<T> >(value);
    }

    struct ActionOption
    {
        ActionOption(std::string n, std::string m, ActionValuePtr const& v = nullptr, bool multi = false, int c = -1) 
            : name(std::move(n)), message(std::move(m)), value(v), multitoken(multi), max(c)
        {}

        std::string     name;
        std::string     message;
        ActionValuePtr  value;
        bool            multitoken;
        int             max;
    };

    class ActionOptions
    {
    public:
        ActionOptions() {}
       ~ActionOptions() {}

    public:
        template<typename F>
        void    Traverse(F const& func)
        {
            std::for_each(m_options.begin(), m_options.end(), func);
        }

        void    AddOption(ActionOption option) { m_options.push_back(std::move(option)); }

    private:
        std::vector<ActionOption>   m_options;
    };

#define SG_SET_FLAG_OPTION(option, value) if (vm.count(option)) value = 1; else value = 0;

#define SG_FILL_DESC(desc, vis_desc, pos_desc) [&](ActionOption const& option) {\
                                                    if (option.value) {\
                                                        desc.add_options()(option.name.c_str(), option.value->value(option.multitoken), "");\
                                                    }\
                                                    else {\
                                                        desc.add_options()(option.name.c_str(), "");\
                                                    }\
                                                    if (!option.message.empty()) {\
                                                        vis_desc.add_options()(option.name.c_str(), option.message.c_str());\
                                                    }\
                                                    if (option.max > 0) {\
                                                        pos_desc.add(option.name.c_str(), option.max);\
                                                    }\
                                                }

#define SG_PARSE_OPTION(args, p)    BOOST_ASSERT(args.size());\
                                    std::vector<std::string> argv(args.begin() + 1, args.end());\
                                    boost::program_options::variables_map vm;\
                                    boost::program_options::options_description desc;\
                                    boost::program_options::options_description vis_desc;\
                                    boost::program_options::positional_options_description pos_desc;\
                                    p->Traverse(SG_FILL_DESC(desc, vis_desc, pos_desc));\
                                    try {\
                                        if (pos_desc.max_total_count())\
                                            boost::program_options::store(\
                                                boost::program_options::command_line_parser(argv).options(desc).positional(pos_desc).run(),\
                                                vm);\
                                        else\
                                            boost::program_options::store(\
                                                boost::program_options::command_line_parser(argv).options(desc).run(), vm);\
                                        boost::program_options::notify(vm);\
                                    }\
                                    catch (boost::program_options::error const& error) {\
                                        COMMS_LOG(boost::format("%1%\n") % error.what(), CLL_Error);\
                                        return false;\
                                    }\
                                    catch (boost::numeric::bad_numeric_cast const&) {\
                                        COMMS_LOG("Option value is out of range\n", CLL_Error);\
                                        return false;\
                                    }\
                                    catch (boost::bad_lexical_cast const&) {\
                                        COMMS_LOG("Invalid option value\n", CLL_Error);\
                                        return false;\
                                    }\
                                    catch(...) {\
                                        COMMS_LOG("Parse option error\n", CLL_Error);\
                                        return false;\
                                    }

}


#endif // !__SG_ACTION_OPTIONS_HPP__



