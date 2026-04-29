#include "flight_manager.h"
#include "../../visual_navigation_node_worklow.h"

void Managers::FlightManager::initialize_position_sender()
{
    auto node_handle = VisualNavigationNodeWorkflow::node_handle();

    this->destination_position_publisher = node_handle.advertise<geometry_msgs::PoseStamped>("mavros/setpoint_position/local", 10,this);
    this->current_position_subscriber = node_handle.subscribe<geometry_msgs::PoseStamped>("mavros/local_position/pose", 10, &FlightManager::current_position_callback,this);

    offboard_timer = node_handle.createTimer(ros::Duration(0.05), &FlightManager::timer_callback, this);
}

void Managers::FlightManager::disable_rc_control()
{
    auto client = VisualNavigationNodeWorkflow::node_handle().serviceClient<mavros_msgs::ParamSet>("/mavros/param/set");
    mavros_msgs::ParamSet srv;
    
    srv.request.param_id = "NAV_RCL_ACT";
    srv.request.value.integer = 0; 
    client.call(srv);

    srv.request.param_id = "COM_RCL_EXCEPT";
    srv.request.value.integer = 4; 
    client.call(srv);

    ROS_INFO("RC CHECK DISABLED IN PX4");
    this->event_queue.push_event(std::make_unique<RCControlDisabledEvent>());

}

void Managers::FlightManager::enable_offboard()
{
    if(!set_mode_client)
    {
        auto node_handle = VisualNavigationNodeWorkflow::node_handle();
        set_mode_client = node_handle.serviceClient<mavros_msgs::SetMode>("mavros/set_mode");
    }

    mavros_msgs::SetMode offboard_set_mode;
    offboard_set_mode.request.custom_mode = "OFFBOARD";

    if (set_mode_client-> call(offboard_set_mode) && offboard_set_mode.response.mode_sent) 
    {
        ROS_INFO("OFFBOARD ENABLED");
    } 
    else 
    {
        ROS_ERROR("Не удалось отправить запрос на смену режима");
    }
}

void Managers::FlightManager::do_arm()
{
    if(!arming_client)
    {
        auto node_handle = VisualNavigationNodeWorkflow::node_handle();
        arming_client = node_handle.serviceClient<mavros_msgs::CommandBool>("/mavros/cmd/arming");
    }

    mavros_msgs::CommandBool arming_command{};
    arming_command.request.value = true;
    if(arming_client->call(arming_command) && arming_command.response.success){
        ROS_INFO("ARMED");
    }
}

void Managers::FlightManager::do_takeoff()
{
    ROS_INFO("STARTING TAKEOFF");
    auto takeoff_position = *current_position;
    takeoff_position.pose.position.z = takeoff_altitude;
    this->destination_position = takeoff_position;
    ROS_INFO("TAKEOFF: DESTINATION POSITION GOT ALTITUDE %d",takeoff_altitude);
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

    auto node_handle = VisualNavigationNodeWorkflow::node_handle();
    this->state_subscriber = VisualNavigationNodeWorkflow::node_handle().subscribe("/mavros/state", 1, &Managers::FlightManager::state_callback, this);
}

void Managers::FlightManager::state_callback(const mavros_msgs::StateConstPtr &state_msg)
{
    if (!current_state) {
        current_state = *state_msg;
        this->event_queue.push_event(std::make_unique<FlightManagerInitilizedEvent>());
        return;
    }


    if (state_msg->mode == "OFFBOARD" && current_state->mode != "OFFBOARD") {
        ROS_INFO("STATE: Offboard mode confirmed!");
        this->event_queue.push_event(std::make_unique<OffboardEnabledEvent>());
    }

    if (state_msg->armed && !current_state->armed) {
        ROS_INFO("STATE: UAV Armed confirmed!");
        this->event_queue.push_event(std::make_unique<UAVArmedEvent>());
    }
    if (!state_msg->armed && current_state->armed) {
        ROS_WARN("STATE: UAV DISARMED!"); 
    }

    // Сохраняем текущее состояние как "предыдущее" для следующего вызова
    this->current_state = *state_msg;
}

void Managers::FlightManager::current_position_callback(const geometry_msgs::PoseStampedConstPtr &msg)
{
    this->current_position = *msg;
}

void Managers::FlightManager::timer_callback(const ros::TimerEvent &event){
    static bool is_warmed_up = false;
    static uint8_t sended_positions = 0;
    if(!current_position)
        return;

    geometry_msgs::PoseStamped sending_position{};
    if(!destination_position)
        sending_position = *current_position;
    else
        sending_position = *destination_position;
    
    sending_position.header.stamp = ros::Time::now();
    sending_position.header.frame_id = "map" ;
    destination_position_publisher.publish(sending_position);

    if(!is_warmed_up)
    {
        if(sended_positions < 100)
            sended_positions++;
        else
        {
            is_warmed_up = true;
             ROS_INFO("WARMED UP, SENDING EVENT");
            this->event_queue.push_event(std::make_unique<PositionSenderCreatedEvent>());
        }
    }
}
