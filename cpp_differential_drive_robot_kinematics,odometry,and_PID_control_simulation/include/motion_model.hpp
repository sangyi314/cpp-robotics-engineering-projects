#pragma once

#include "robot_types.hpp"

namespace robot
{
    Pose2D integrate_pose(const Pose2D& pose , const BodyVelocity& velocity , double dt_s);
}