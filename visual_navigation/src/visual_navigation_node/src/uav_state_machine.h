#include <state_machine_enum.h>


class UAVStateMachine{
public:
    UAVStateMachine() = default;
    ~UAVStateMachine() = default;

public:
    void state_machine_loop(){
    }

private:
    UAV_STATE current_state{UAV_STATE::START};
};