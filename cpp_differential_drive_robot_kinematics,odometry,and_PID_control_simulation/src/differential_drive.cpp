#include "differential_drive.hpp"

#include <cmath>
#include <stdexcept>

namespace
{
    void validate_params(const robot::RobotParams& params)
    {
        if(!std::isfinite(params.track_width_m) || !std::isfinite(params.wheel_radius_m)
            || params.track_width_m <= 0.0 || params.wheel_radius_m <= 0.0)
        {
            throw std::invalid_argument("Wheel radius and track width must be finite and positive.");
        }
    }
}

namespace robot
{
    WheelSpeeds inverse_kinematics(const RobotParams& params , const BodyVelocity& body)
    {
        validate_params(params);

        const double r = params.wheel_radius_m;
        const double half_track = params.track_width_m / 2.0;

        return WheelSpeeds{
            (body.linear_m_s - body.angular_rad_s * half_track) / r,
            (body.linear_m_s + body.angular_rad_s * half_track) / r
        };
    }
    
    BodyVelocity forward_kinematics(const RobotParams& params , const WheelSpeeds& wheels)
    {
        validate_params(params);

        const double r = params.wheel_radius_m;
        const double track = params.track_width_m;

        return BodyVelocity{
            r * (wheels.left_rad_s + wheels.right_rad_s) / 2.0,
            r * (wheels.right_rad_s - wheels.left_rad_s) / track
        };
    }

}