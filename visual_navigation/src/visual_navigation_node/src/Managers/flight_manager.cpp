#include "flight_manager.h"
#include "../visual_navigation_node_worklow.h"

void Managers::FlightManager::arm()
{
    ROS_INFO("ARMING UAV");
    VisualNavigationNodeWorkflow::event_queue().push_event(std::make_unique<EventSystem::UAVArmedEvent>());
}

void Managers::FlightManager::takeoff()
{
    ROS_INFO("TAKEOFF UAV");
}

void Managers::FlightManager::initialize()
{
    ROS_INFO("CREATING FLIGHT MANAGER");
}