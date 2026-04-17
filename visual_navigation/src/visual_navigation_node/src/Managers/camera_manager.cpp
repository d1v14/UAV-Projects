#include "camera_manager.h"
#include "../visual_navigation_node_worklow.h"

void Managers::CameraManager::initialize()
{
    ROS_INFO("CREATING CAMERA MANAGER");
    VisualNavigationNodeWorkflow::event_queue().push_event(std::make_unique<EventSystem::CameraInitializedEvent>(1));
}
