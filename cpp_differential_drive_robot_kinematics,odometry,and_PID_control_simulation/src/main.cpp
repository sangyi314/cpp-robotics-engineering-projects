#include "robot_types.hpp"

#include <iostream>
#include <iomanip>

int main()
{
    const robot::RobotParams params{};

    robot::Pose2D pose{};
    robot::WheelSpeeds wheels{};
    robot::BodyVelocity command{0.20, 0.0};

    std::cout << std::fixed << std::setprecision(3);

    std::cout << "wheel_radius_m = "
              << params.wheel_radius_m << '\n';

    std::cout << "track_width_m = "
              << params.track_width_m << '\n';

    std::cout << "initial pose: x=" << pose.x_m
              << " m, y=" << pose.y_m
              << " m, theta=" << pose.theta_rad
              << " rad\n";

    std::cout << "initial wheel speeds: left="
              << wheels.left_rad_s
              << " rad/s, right=" << wheels.right_rad_s
              << " rad/s\n";

    std::cout << "target body velocity: linear="
              << command.linear_m_s
              << " m/s, angular=" << command.angular_rad_s
              << " rad/s\n";

    return 0;
}