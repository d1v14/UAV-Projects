#pragma once
#include <ros/ros.h>
#include "EventSystem/event_queue.h"
#include "uav_manager.h"
#include "UAVEvents/start_event.h"

#include <optional>
class VisualNavigationNodeWorkflow
{
public:
    VisualNavigationNodeWorkflow(){
        inititalize();
    }

public:
    void start(){
        node_event_queue.push_event(std::make_unique<EventSystem::StartEvent>());
        node_event_queue.start();
    }

public:
    static EventSystem::EventQueue& event_queue(){
        return node_event_queue;
    }

    static UAVManager& get_uav_manager(){
        return uav_manager;
    }

    static ros::NodeHandle& node_handle(){
        return *handle;
    }

private:
    void connect_state_machine_to_event_queue(){
        this->node_event_queue.set_processing_method([this](const EventSystem::EventQueue::Event& event){this->uav_manager.get_state_machine().state_processing_method(event);});
    }

    void inititalize(){
        handle = ros::NodeHandle();
        connect_state_machine_to_event_queue();
    }

private:
    static inline std::optional<ros::NodeHandle> handle{std::nullopt};
    static inline EventSystem::EventQueue node_event_queue{};
    static inline UAVManager uav_manager{};
};