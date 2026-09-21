#pragma once

#include "robot_types.hpp"

namespace robot
{
    WheelSpeeds inverse_kinematics(const RobotParams& params , const BodyVelocity& body);
    
    BodyVelocity forward_kinematics(const RobotParams& params , const WheelSpeeds& wheels);
}