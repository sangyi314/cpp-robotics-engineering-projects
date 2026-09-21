#pragma once

#include "robot_types.hpp"
#include "encoder_types.hpp"

namespace robot
{
    class EncoderSimulator
    {
        public:
            explicit EncoderSimulator(int count_per_revolution);
            EncoderCounts update(const WheelSpeeds& actual_speeds , double dt_s );
            EncoderCounts read() const;

        private:
            int counts_per_revolution_ {};
            double left_angle_rad_ {0.0};
            double right_angle_rad_ {0.0};
            EncoderCounts counts_ ;
    };
}
