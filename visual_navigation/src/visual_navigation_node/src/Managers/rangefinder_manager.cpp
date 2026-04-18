#include "rangefinder_manager.h"
#include "../visual_navigation_node_worklow.h"

void Managers::RangefinderManager::initialize()
{
    this->rangifinder_subscriber = VisualNavigationNodeWorkflow::node_handle().subscribe("/range_down", 1, &Managers::RangefinderManager::rangefinder_callback, this);
}

void Managers::RangefinderManager::rangefinder_callback(const sensor_msgs::RangeConstPtr &msg)
{
    bool before_message_state = this->min_range.has_value() && this->max_range.has_value() && this->range.has_value();

    if(!this->max_range && (abs(msg->max_range - 0) > std::numeric_limits<float>::epsilon())){
        this->max_range = msg->max_range;
        ROS_INFO("[RANGEFINDER MANGER] Got max range: %f", *this->max_range);
    }

    if(!this->min_range && (abs(msg->min_range - 0) > std::numeric_limits<float>::epsilon())){
        this->min_range = msg->min_range;
        ROS_INFO("[RANGEFINDER MANGER] Got min range: %f", *this->min_range);
    }

    if(this->max_range && this->min_range && msg->range <= this->max_range && msg->range >= this->min_range){
        this->range = msg->range;
    }

    bool after_message_state = this->min_range && this->max_range && this->range;

    if(before_message_state != after_message_state)
        after_message_state ? VisualNavigationNodeWorkflow::event_queue().push_event(std::make_unique<EventSystem::RangeFinderInitializedEvent>()) : void();
}
