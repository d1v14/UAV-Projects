#include "EventSystem/event_queue.h"
#include "StateSystem/uav_state_machine.h"
class VisualNavigationNodeWorkflow
{
public:
    EventSystem::EventQueue& event_queue(){
        return node_event_queue;
    }

private:
    static inline EventSystem::EventQueue node_event_queue{};
    static inline StateSystem::UAVStateMachine uav_state_machine{};
};