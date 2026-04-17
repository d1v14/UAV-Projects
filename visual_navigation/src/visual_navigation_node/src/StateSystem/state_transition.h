#pragma once 
 
#include "state_machine_enum.h"
#include "../EventSystem/Events/base_event.h"
#include <functional>
#include <string_view>

namespace StateSystem{

class StateTransition{
public:
    using StateChangingMethod = std::function<void()>;
    using CallerChangeStateMethod = std::function<void(UAV_STATE)>;
public:
    StateTransition(UAV_STATE state_before, UAV_STATE destination_state,
        StateChangingMethod state_leave_method,
        StateChangingMethod state_enter_process,
        StateChangingMethod transition_method):
    state_before(state_before),
    destination_state(destination_state),
    state_enter_method(std::move(state_enter_process)),
    state_leave_method(std::move(state_leave_method)),
    transition_method(std::move(transition_method)){}

    UAV_STATE do_transition(){
        state_leave_method();
        state_enter_method();
        transition_method();
        return this->destination_state;
    }

public:
    static StateChangingMethod get_default_state_changing_message(UAV_STATE state_before, UAV_STATE destination_state, std::string_view message){
        return [state_before, destination_state, message](){ROS_INFO("[%s -> %s] %s",
            get_uav_state_name(state_before).data(),
            get_uav_state_name(destination_state).data(),
            message.data());};
    }


private:
    UAV_STATE state_before{};
    UAV_STATE destination_state{};

    StateChangingMethod state_enter_method{};
    StateChangingMethod state_leave_method{};
    StateChangingMethod transition_method{};

};

}