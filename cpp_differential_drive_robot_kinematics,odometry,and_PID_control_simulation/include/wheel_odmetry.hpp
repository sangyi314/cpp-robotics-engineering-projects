#pragma once

#include "encoder_types.hpp"
#include "robot_types.hpp"

namespace robot
{
    class WheelOdometry
    {
    public:
        WheelOdometry(const RobotParams& params, int counts_per_revolution);
        void reset(const Pose2D& initial_pose, const EncoderCounts& initial_counts);
        Pose2D update(const EncoderCounts& current_counts);
        Pose2D pose() const;

    private:
        double meters_per_count_;
        double track_width_m_;
        Pose2D pose_{};
        EncoderCounts previous_counts_{};
        bool initialized_{false};
    };
}
