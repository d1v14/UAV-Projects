#include "imu_manager.h"
#include "../visual_navigation_node_worklow.h"

void Managers::IMUManager::initialize()
{
    ROS_INFO("CREATING IMU MANAGER");
    VisualNavigationNodeWorkflow::event_queue().push_event(std::make_unique<EventSystem::IMUInitializedEvent>());
}