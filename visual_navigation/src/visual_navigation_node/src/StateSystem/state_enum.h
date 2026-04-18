#pragma once
#include <stdint.h>
#include <limits>

namespace StateSystem{
    using state_underlying_type = uint16_t;

    enum class STATE: state_underlying_type{
    STATE_0 = 0,
    STATE_LAST = std::numeric_limits<state_underlying_type>::max()
};

}