#pragma once

#include "robot_types.hpp"

#include <cstdint>

namespace robot
{
    struct EncoderCounts
    {
        std::int64_t left_ticks{0};
        std::int64_t right_ticks{0};
    };
}