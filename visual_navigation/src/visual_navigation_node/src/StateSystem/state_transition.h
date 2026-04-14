#include "state_machine_enum.h"
#include "../EventSystem/Events/base_event.h"
#include <functional>

namespace StateSystem{

class StateTransition{
public:


private:
    UAV_STATE state_before{};
    UAV_STATE destination_state{};
    std::function<void()> state_enter_process{};
    std::function<void()> state_leave_process{};
    
};

}