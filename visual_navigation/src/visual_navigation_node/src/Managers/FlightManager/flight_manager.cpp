#include "flight_manager.h"
#include "../../visual_navigation_node_worklow.h"

void Managers::FlightManager::disable_rc_control()
{
    // auto param_client = VisualNavigationNodeWorkflow::node_handle().serviceClient<mavros_msgs::ParamSet>("/mavros/param/set");
    // mavros_msgs::ParamSet srv;
    // srv.request.param_id = "NAV_RCL_ACT";
    // srv.request.value.integer = 0; 
    
    // if (param_client.call(srv)) {
    //     VisualNavigationNodeWorkflow::event_queue().push_event(std::make_unique<EventSystem::RCControlDisabledEvent>());
    // }
    ROS_INFO("RC DISABLED");
    this->event_queue.push_event(std::make_unique<RCControlDisabledEvent>());
}

void Managers::FlightManager::enable_offboard()
{
    ROS_INFO("OFFBOARD ENABLED");
    this->event_queue.push_event(std::make_unique<OffboardEnabledEvent>());
}

void Managers::FlightManager::do_arm()
{
    // auto arming_client = VisualNavigationNodeWorkflow::node_handle().serviceClient<mavros_msgs::CommandBool>("/mavros/cmd/arming");
    // mavros_msgs::CommandBool arming_command{};
    // arming_command.request.value = true;
    // if(arming_client.call(arming_command) && arming_command.response.success){
    //     VisualNavigationNodeWorkflow::event_queue().push_event(std::make_unique<EventSystem::UAVArmedEvent>());
    // }
    ROS_INFO("ARMED");
    this->event_queue.push_event(std::make_unique<UAVArmedEvent>());
}

void Managers::FlightManager::do_takeoff()
{
    // auto takeoff_client = VisualNavigationNodeWorkflow::node_handle().serviceClient<mavros_msgs::CommandTOL>("/mavros/cmd/takeoff");
    // mavros_msgs::CommandTOL takeoff_command;
    // takeoff_command.request.altitude = takeoff_altitude;

    // if (takeoff_client.call(takeoff_command) && takeoff_command.response.success) {
    //     ROS_INFO("[FLIGHT MANAGER] Takeoff sent!");
    // }
    ROS_INFO("TAKEOFF DONE");
    this->event_queue.push_event(std::make_unique<TakeoffDoneEvent>());
}

void Managers::FlightManager::notify_uav_ready()
{
    ROS_INFO("NOTIFYING UAV READY");
}

void Managers::FlightManager::initialize()  
{   
    initialize_state_transition_table();
    this->event_queue.set_processing_method([this](const EventSystem::EventQueue::Event& event){this->state_machine.state_processing_method(event);});
    this->event_queue.start();
    this->state_subscriber = VisualNavigationNodeWorkflow::node_handle().subscribe("/mavros/state", 1, &Managers::FlightManager::state_callback, this);
}

void Managers::FlightManager::state_callback(const mavros_msgs::StateConstPtr &state_msg)
{
    bool state_before = this->current_state.has_value();
    this->current_state = *state_msg;
    bool state_after = this->current_state.has_value();
    if(state_after != state_before)
    {
        ROS_INFO("STATE CHANGED");
        this->event_queue.push_event(std::make_unique<FlightManagerInitilizedEvent>());
    }
}
