#include "state_machine_enum.h"
#include "../EventSystem/event_queue.h"
#include <map>
#include "state_transition.h"
#include <optional>

namespace StateSystem{

class UAVStateMachine{
private:
    using StateEventConnection = std::pair<UAV_STATE,EventType>;
    using StateTransitionTable = std::map<StateEventConnection, StateTransition>;

public:
    UAVStateMachine(){
        initialize_state_transition_table();
    };
    ~UAVStateMachine() = default;

public:
    UAV_STATE get_uav_state(){
        return this->current_state;
    }

    void state_processing_method(const EventSystem::EventQueue::Event& event){
        auto event_type = event->get_event_type();
        auto transition_to_new_state_by_event = this->transition_table.find({current_state,event_type});
        if(transition_to_new_state_by_event == this->transition_table.end())
            return;

        this->current_state = transition_to_new_state_by_event.do_transition();
        
    }


private:
    void add_transition(UAV_STATE state, EventSystem::EventType event_type, StateSystem::StateTransition transition){
        this->transition_table.emplace(std::make_pair(state,event_type),std::move(transition));
    }

    void initialize_state_transition_table(){ 
        StateTransition preflight_camera_transition{UAV_STATE::PREFLIGHT, 
            UAV_STATE::INITIALIZE_CAMERA, 
            StateTransition::get_default_state_changing_process(UAV_STATE::PREFLIGHT,UAV_STATE::INITIALIZE_CAMERA, "Leaving preflight state..."),
            StateTransition::get_default_state_changing_process(UAV_STATE::PREFLIGHT,UAV_STATE::INITIALIZE_CAMERA, "Started camera intialization..."),
            [this](){create_camera_manager();}};

        this->add_transition(UAV_STATE::PREFLIGHT, 
            EventSystem::EventType::START, std::move(preflight_camera_transition));

        StateTransition camera_rangefinder_transition{UAV_STATE::INITIALIZE_CAMERA, 
            UAV_STATE::INITIALIZE_RANGEFINDER,
            StateTransition::get_default_state_changing_process(UAV_STATE::INITIALIZE_CAMERA,UAV_STATE::INITIALIZE_RANGEFINDER, "Camera successfully initialized!"),
            StateTransition::get_default_state_changing_process(UAV_STATE::INITIALIZE_CAMERA,UAV_STATE::INITIALIZE_RANGEFINDER, "Started rangefinder intialization..."),
            [this](){create_rangefinder_manager();}};

        this->add_transition(UAV_STATE::INITIALIZE_CAMERA, 
            EventSystem::EventType::CAMERA_INITIALIZED, std::move(camera_rangefinder_transition));

        StateTransition rangefinder_imu_transition{UAV_STATE::INITIALIZE_RANGEFINDER, 
            UAV_STATE::INITIALIZE_IMU, 
            StateTransition::get_default_state_changing_process(UAV_STATE::INITIALIZE_RANGEFINDER,UAV_STATE::INITIALIZE_IMU, "Rangefinder successfully initialized!"),
            StateTransition::get_default_state_changing_process(UAV_STATE::INITIALIZE_RANGEFINDER,UAV_STATE::INITIALIZE_IMU, "Started IMU intialization..."),
            [this](){create_imu_manager();}};

        this->add_transition(UAV_STATE::INITIALIZE_RANGEFINDER, 
            EventSystem::EventType::RANGEFINDER_INITIALIZED, std::move(rangefinder_imu_transition));

        StateTransition imu_arming_transition{UAV_STATE::INITIALIZE_IMU, 
            UAV_STATE::ARMING, 
            StateTransition::get_default_state_changing_process(UAV_STATE::INITIALIZE_IMU,UAV_STATE::ARMING, "IMU successfully initialized!"),
            StateTransition::get_default_state_changing_process(UAV_STATE::INITIALIZE_IMU,UAV_STATE::ARMING, "Start UAV arming..."),
            [this](){create_uav_controller();}};

        this->add_transition(UAV_STATE::INITIALIZE_IMU, 
            EventSystem::EventType::IMU_INITIALIZED, std::move(imu_arming_transition));

        StateTransition arming_takeoff_transition{UAV_STATE::ARMING, 
            UAV_STATE::TAKEOFF, 
            StateTransition::get_default_state_changing_process(UAV_STATE::ARMING,UAV_STATE::TAKEOFF, "UAV succcessfully armed!"),
            StateTransition::get_default_state_changing_process(UAV_STATE::ARMING,UAV_STATE::TAKEOFF, "Starting UAV takeoff..."),
            [this](){do_takeoff();}};  

        this->add_transition(UAV_STATE::ARMING, 
            EventSystem::EventType::UAV_ARMED, std::move(arming_takeoff_transition));

        StateTransition takeoff_ready_transition{UAV_STATE::TAKEOFF, 
            UAV_STATE::READY, 
            StateTransition::get_default_state_changing_process(UAV_STATE::TAKEOFF,UAV_STATE::READY, "UAV successfully takeoff!"),
            StateTransition::get_default_state_changing_process(UAV_STATE::TAKEOFF,UAV_STATE::READY, "UAV ready for work!"),
            [this](){create_visual_navigation_pipeline();}}; 
        
        this->add_transition(UAV_STATE::TAKEOFF, 
            EventSystem::EventType::UAV_READY, std::move(takeoff_ready_transition));
    }

    void change_state(UAV_STATE new_state){
        this->current_state = new_state;
    }

    void create_camera_manager(){
        ROS_INFO("CREATING CAMERA MANAGER");
    }

    void create_rangefinder_manager(){
        ROS_INFO("CREATING RANGEFINDER MANAGER");
    }

    void create_imu_manager(){
        ROS_INFO("CREATING IMU MANAGER");
    }

    void create_uav_controller(){
        ROS_INFO("CREATING UAV CONTROLLER");
    }

    void do_takeoff(){
        ROS_INFO("DO TAKEOFF");
    }

    void create_visual_navigation_pipeline(){
        ROS_INFO("CREATING VISUAL NAVIGATION PIPELINE");
    }

private:
    UAV_STATE current_state{UAV_STATE::PREFLIGHT};
    static inline StateTransitionTable transition_table{};

};

}