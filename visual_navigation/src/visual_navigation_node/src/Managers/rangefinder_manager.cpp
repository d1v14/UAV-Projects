#include "rangefinder_manager.h"
#include "../visual_navigation_node_worklow.h"

void Managers::RangefinderManager::initialize()
{
    ROS_INFO("CREATING RANGEFINDER MANAGER");
    VisualNavigationNodeWorkflow::event_queue().push_event(std::make_unique<EventSystem::RangeFinderIntitalizedEvent>());
}