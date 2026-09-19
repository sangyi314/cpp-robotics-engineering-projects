#pragma once

namespace robot 
{
    struct RobotParams
    {
        double wheel_radius_m {0.05};
        double track_width_m {0.30};
    };

    struct Pose2D
    {
        double x_m{0.0};
        double y_m{0.0};
        double theta_rad{0.0};
    };

    struct WheelSpeeds
    {
        double left_rad_s{0.0};
        double right_rad_s{0.0};
    };

    struct BodyVelocity
    {
        double linear_m_s {0.0};
        double angular_rad_s {0.0};
    };
}