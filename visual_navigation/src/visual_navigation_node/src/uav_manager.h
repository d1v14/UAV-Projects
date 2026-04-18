#pragma once

#include "uav_states_enum.h"
#include "EventSystem/event_queue.h"
#include <map>
#include <optional>
#include "Managers/camera_manager.h"
#include "Managers/rangefinder_manager.h"
#include "Managers/imu_manager.h"
#include "Managers/FlightManager/flight_manager.h"
#include "StateSystem/state_machine.h"

class UAVManager{
public:
    UAVManager(){
        initialize_state_transition_table();
    };
    ~UAVManager() = default;

public:
    UAV_STATE get_uav_state(){
        return convert_base_state_to_uav_state(this->state_machine.get_state());
    }

    StateSystem::StateMachine& get_state_machine(){
        return this->state_machine;
    }

private: 
    void add_transition(UAV_STATE state_before, UAV_STATE state_after, UAV_EVENT triggering_event, std::string_view leaving_message, std::string_view entering_message, StateSystem::StateTransition::StateChangingMethod transition_method)
    {
        auto state_before_string_presentation = get_uav_state_string_presentation(state_before);
        auto state_after_string_presentation = get_uav_state_string_presentation(state_after);
        auto uav_state_before_in_base_state = convert_uav_state_to_base_state(state_before);
        auto uav_state_after_in_base_state = convert_uav_state_to_base_state(state_after);
        auto uav_event_type_in_base_event_type = convert_uav_event_type_to_base_event_type(triggering_event);
        StateSystem::StateTransition transition{uav_state_before_in_base_state, uav_state_after_in_base_state, 
                   StateSystem::StateTransition::get_default_state_changing_message(state_before_string_presentation, state_after_string_presentation, leaving_message),
                   StateSystem::StateTransition::get_default_state_changing_message(state_before_string_presentation, state_after_string_presentation, entering_message),
                   std::move(transition_method)};    
        
        this->state_machine.add_transition(uav_state_before_in_base_state, uav_event_type_in_base_event_type, std::move(transition));

    }

    void initialize_state_transition_table(){ 
        this->add_transition(UAV_STATE::PREFLIGHT,UAV_STATE::INITIALIZE_CAMERA, UAV_EVENT::START, "Leaving preflight state...", "Started camera intialization...",
            [this](){create_camera_manager();});

        this->add_transition(UAV_STATE::INITIALIZE_CAMERA,UAV_STATE::INITIALIZE_RANGEFINDER, UAV_EVENT::CAMERA_INITIALIZED, "Camera successfully initialized!", "Started rangefinder intialization..."
            ,[this](){create_rangefinder_manager();});
            
        this->add_transition(UAV_STATE::INITIALIZE_RANGEFINDER,UAV_STATE::INITIALIZE_IMU, UAV_EVENT::RANGEFINDER_INTIALIZED, "Rangefinder successfully initialized!", "Started IMU intialization...",
            [this](){create_imu_manager();});
            
        this->add_transition(UAV_STATE::INITIALIZE_IMU,UAV_STATE::INITIALIZE_FLIGHT_MANAGER, UAV_EVENT::IMU_INITIALIZED, "IMU successfully initialized!", "Started flight manager initializing...",
            [this](){create_flight_manager();});
    }

    void create_camera_manager(){
        this->camera_manager.emplace();
    }

    void create_rangefinder_manager(){
        this->rangefinder_manager.emplace();
    }

    void create_imu_manager(){
        this->imu_manager.emplace();
    }

    void create_flight_manager(){
        this->flight_manager.emplace();
    }

    void create_visual_navigation_pipeline(){
        ROS_INFO("CREATING VISUAL NAVIGATION PIPELINE");
    }

private:
    StateSystem::StateMachine state_machine{};

private:
    std::optional<Managers::CameraManager> camera_manager{std::nullopt};
    std::optional<Managers::RangefinderManager> rangefinder_manager{std::nullopt};
    std::optional<Managers::IMUManager> imu_manager{std::nullopt};
    std::optional<Managers::FlightManager> flight_manager{std::nullopt};
};

