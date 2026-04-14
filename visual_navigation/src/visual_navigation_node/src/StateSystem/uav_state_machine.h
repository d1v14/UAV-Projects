#include "state_machine_enum.h"
#include "../EventSystem/event_queue.h"
#include <map>
#include "state_transition.h"

namespace StateSystem{

class UAVStateMachine{
private:
    using StateEventConnection = std::pair<UAV_STATE,EventType>;
    using StateTransitionTable = std::map<StateEventConnection, StateTransition>;

public:
    UAVStateMachine() = default;
    ~UAVStateMachine() = default;

public:
    void get_uav_state(){
        return this->current_state;
    }

    void state_processing_method(const EventSystem::EventQueue::Event& event){
        
    }

    void add_transition(UAV_STATE state, EventSystem::EventType event_type, StateSystem::StateTransition transition){
        this->transition_table.emplace(std::make_pair(state,event_type),std::move(transition));
    }



private:
    UAV_STATE current_state{UAV_STATE::START};
    static inline StateTransitionTable transition_table{};
};

}