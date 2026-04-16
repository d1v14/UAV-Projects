#pragma once
#include "EventSystem/event_queue.h"
#include "StateSystem/uav_state_machine.h"

class VisualNavigationNodeWorkflow
{
public:
    VisualNavigationNodeWorkflow(){
        connect_state_machine_to_event_queue();
    }

public:
    EventSystem::EventQueue& event_queue(){
        return node_event_queue;
    }

    StateSystem::UAVStateMachine& state_machine(){
        return uav_state_machine;
    }

private:
    void connect_state_machine_to_event_queue(){
        this->node_event_queue.set_processing_method([this](const EventSystem::EventQueue::Event& event){this->uav_state_machine.state_processing_method(event);});
    }

private:
    static inline EventSystem::EventQueue node_event_queue{};
    static inline StateSystem::UAVStateMachine uav_state_machine{};
};