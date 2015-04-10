#include "core/core.hpp" 
#include "unittestpp/unittestpp.h"

#include "simulator/action.hpp"
#include "simulator/action_factory.hpp"
#include <vector>
#include <string>

struct TestActionFactory {
    std::vector< std::string > m_test_lines;
    std::vector< int > m_expect_actionIDs;
    std::vector< std::string > m_expect_actions ;
  
    TestActionFactory() {
        std::string lines[] = {
            "q", 
            "pk", 
            ""
        };
        int expect_actionIDs[] = {
            sg::Action::AT_QUIT,
            sg::Action::AT_PICK_EGM,
            
            sg::Action::AT_NUM
        };

        std::string expect_actions[] = {
            "q",
            "pk",
            ""
        };

        int i=0;
        while(1) {
            if( lines[i] == "" || expect_actionIDs[i] == sg::Action::AT_NUM
                    ||expect_actions[i] == "" ) {
                m_test_lines.push_back("");
                m_expect_actions.push_back("");
                m_expect_actionIDs.push_back(sg::Action::AT_NUM);
                break;
            }
            i++;
            m_test_lines.push_back(lines[i]);
            m_expect_actions.push_back(expect_actions[i]);
            m_expect_actionIDs.push_back(expect_actionIDs[i]);
        }
    }
    ~TestActionFactory(){}

};

TEST_FIXTURE(TestActionFactory, Parse) {
    sg::ActionFactory fac;
    for( int i=0 ; m_test_lines[i] != "" ; i++ ){
        std::vector<std::string > args;
        CHECK_EQUAL(true , fac.Parse(m_test_lines[i] , args));
        CHECK(args[0] == m_expect_actions[i]);
    }
}

TEST_FIXTURE(TestActionFactory, CreateAction) {
    sg::ActionFactory fac;
    fac.Init();
    for(int i=0 ; m_test_lines[i] != "" ; i++ ){
        CHECK_EQUAL(fac.CreateAction(m_test_lines[i])->GetType() , m_expect_actionIDs[i]);
    }
}








