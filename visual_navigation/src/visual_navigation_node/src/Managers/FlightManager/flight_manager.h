#pragma once
#include <ros/ros.h> 
#include <std_msgs/String.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/CommandTOL.h>
#include <mavros_msgs/State.h>
#include <optional>
#include <mavros_msgs/ParamSet.h>
#include "../../UAVEvents/uav_event_enum.h"
#include "States/flight_manager_state_enum.h"
#include "Events/flight_manager_events_enum.h"
#include "Events/uav_armed_event.h"
#include "Events/flight_manager_initialized_event.h"
#include "Events/rc_control_disabled_event.h"
#include "Events/offboard_enabled_event.h"
#include "Events/uav_ready_event.h"
#include "Events/takeoff_done_event.h"
#include "../../StateSystem/state_machine.h"

namespace Managers{

    class FlightManager{
    public:
        FlightManager(){
            initialize();
        }
        ~FlightManager() = default;

    public:
        void disable_rc_control();
        void enable_offboard();
        void do_arm();
        void do_takeoff();
        void notify_uav_ready();


    private:
        void initialize();
        void state_callback(const mavros_msgs::StateConstPtr& state_msg);

    
    private:
        FLIGHT_MANAGER_STATE get_uav_state(){
            return convert_base_state_to_flight_manager_state(this->state_machine.get_state());
        }

        StateSystem::StateMachine& get_state_machine(){
            return this->state_machine;
        }

    private: 
        void add_transition(FLIGHT_MANAGER_STATE state_before, FLIGHT_MANAGER_STATE state_after, FLIGHT_MANAGER_EVENT triggering_event, std::string_view leaving_message, std::string_view entering_message, StateSystem::StateTransition::StateChangingMethod transition_method)
        {
            auto state_before_string_presentation = get_flight_manager_state_string_presentation(state_before);
            auto state_after_string_presentation = get_flight_manager_state_string_presentation(state_after);
            auto flight_manager_before_in_base_state = convert_flight_manager_state_to_base_state(state_before);
            auto flight_manager_after_in_base_state = convert_flight_manager_state_to_base_state(state_after);
            auto flight_manager_event_type_in_base_event_type = convert_flight_manager_event_type_to_base_event_type(triggering_event);
            StateSystem::StateTransition transition{flight_manager_before_in_base_state, flight_manager_after_in_base_state, 
                       StateSystem::StateTransition::get_default_state_changing_message(state_before_string_presentation, state_after_string_presentation, leaving_message),
                       StateSystem::StateTransition::get_default_state_changing_message(state_before_string_presentation, state_after_string_presentation, entering_message),
                       std::move(transition_method)};    
            
            this->state_machine.add_transition(flight_manager_before_in_base_state, flight_manager_event_type_in_base_event_type, std::move(transition));
        }

        void initialize_state_transition_table(){ 
            this->add_transition(FLIGHT_MANAGER_STATE::CREATED,FLIGHT_MANAGER_STATE::DISABLING_RC, FLIGHT_MANAGER_EVENT::FLIGHT_MANAGER_CREATED, "Leaving flight manager created state...", "Started rc disabling...",
                [this](){disable_rc_control();});

            this->add_transition(FLIGHT_MANAGER_STATE::DISABLING_RC,FLIGHT_MANAGER_STATE::ENABLING_OFFBOARD, FLIGHT_MANAGER_EVENT::RC_DISABLED, "RC successfully disabled!", "Started offboard enabling..."
                ,[this](){enable_offboard();});

            this->add_transition(FLIGHT_MANAGER_STATE::ENABLING_OFFBOARD,FLIGHT_MANAGER_STATE::ARMING, FLIGHT_MANAGER_EVENT::OFFBOARD_ENABLED, "Offboard successfully enabeld!", "Started UAV arming...",
                [this](){do_arm();});

            this->add_transition(FLIGHT_MANAGER_STATE::ARMING,FLIGHT_MANAGER_STATE::TAKEOFF, FLIGHT_MANAGER_EVENT::ARMED, "UAV successfully armed!", "Started UAV takeoff...",
                [this](){do_takeoff();});

            this->add_transition(FLIGHT_MANAGER_STATE::TAKEOFF,FLIGHT_MANAGER_STATE::READY, FLIGHT_MANAGER_EVENT::TAKEOFF_DONE, "UAV successfully takeoff!", "Flight manager preparing done!",
                [this](){notify_uav_ready();});
        }

    private:
        StateSystem::StateMachine state_machine{};
        EventSystem::EventQueue event_queue{};

    private:
        ros::Subscriber state_subscriber{};
        ros::ServiceClient takeoff_client{};
        std::optional<mavros_msgs::State> current_state{std::nullopt};
        
    private:
        static constexpr const uint8_t takeoff_altitude{10};
    };
}