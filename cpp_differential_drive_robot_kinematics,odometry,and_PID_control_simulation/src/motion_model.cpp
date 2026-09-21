#include "motion_model.hpp"

#include <cmath>
#include <stdexcept>
#include <numbers>

namespace robot
{
    Pose2D integrate_pose(const Pose2D& pose , const BodyVelocity& velocity , double dt_s)
    {
        if(!std::isfinite(dt_s) || dt_s <= 0.0)
        {
            throw std::invalid_argument("dt_s must be finite and positive.");
        }

        double delta_theta {velocity.angular_rad_s * dt_s};
        double mid_theta {(delta_theta + pose.theta_rad) / 2};
        double distance {velocity.linear_m_s * dt_s};

        return Pose2D{
            pose.x_m + distance * std::cos(mid_theta),
            pose.y_m + distance * std::sin(mid_theta),
            std::remainder(pose.theta_rad + delta_theta , 2.0 * std::numbers::pi)
        };
    }
}