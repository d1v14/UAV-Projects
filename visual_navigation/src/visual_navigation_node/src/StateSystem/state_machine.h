#pragma once
#include <map>
#include <limits>
#include "state_transition.h"
#include "../EventSystem/event_queue.h"
#include "state_enum.h"
#include <ros/ros.h>

namespace StateSystem{

class StateMachine{
private:
    using StateEventConnection = std::pair<STATE,EventSystem::EventType>;
    using StateTransitionTable = std::map<StateEventConnection, StateTransition>;

public:
    STATE get_state() const{
        return this->current_state;
    }

    void change_state(STATE new_state){
        this->current_state = new_state;
    }

    void state_processing_method(const EventSystem::EventQueue::Event& event){
        auto event_type = event->get_event_type();
        auto transition_to_new_state_by_event = this->transition_table.find(std::make_pair(current_state,event_type));
        if(transition_to_new_state_by_event == this->transition_table.end())
            return;
    
        auto& transition = transition_to_new_state_by_event->second;
        this->current_state = transition.do_transition();
    }

public:
    void add_transition(STATE state, EventSystem::EventType event_type, StateSystem::StateTransition transition){
        this->transition_table.emplace(std::make_pair(state,event_type),std::move(transition));
    }

private:
    STATE current_state{STATE::STATE_0};
    StateTransitionTable transition_table{};
};

}