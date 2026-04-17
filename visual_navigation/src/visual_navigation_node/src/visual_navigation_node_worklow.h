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
    static EventSystem::EventQueue& event_queue(){
        return node_event_queue;
    }

    static StateSystem::UAVStateMachine& state_machine(){
        return uav_state_machine;
    }

private:
    static void connect_state_machine_to_event_queue(){
        node_event_queue.set_processing_method([](const EventSystem::EventQueue::Event& event){uav_state_machine.state_processing_method(event);});
    }

private:
    static inline EventSystem::EventQueue node_event_queue{};
    static inline StateSystem::UAVStateMachine uav_state_machine{};
};