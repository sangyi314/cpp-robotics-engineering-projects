#include "encoder_simulator.hpp"

#include <stdexcept>
#include <cmath>
#include <numbers>

namespace 
{
    std::int64_t to_ticks(double total_angle_rad , int counts_per_revolution)
    {
        const long double ticks = std::round(static_cast<long double>(total_angle_rad) / (2.0L * std::numbers::pi_v<long double>) 
                                            *counts_per_revolution);
        const long double limit = std::ldexp(1.0L , 63);

        if (!std::isfinite(ticks) || ticks < -limit || ticks >= limit)
        {
            throw std::overflow_error("Encoder count overflow.");
        }
        return static_cast<std::int64_t>(ticks);
    }
}

namespace robot 
{
    EncoderSimulator::EncoderSimulator(int counts_per_revolution)
        :counts_per_revolution_{counts_per_revolution}
    {
        if(counts_per_revolution <= 0)
        {
            throw std::invalid_argument("Counts per revolution must be positive.");
        }
    }

    EncoderCounts EncoderSimulator::update(const WheelSpeeds& actual_speeds , double dt_s )
    {
        if (!std::isfinite(dt_s) || dt_s <= 0.0 ||
            !std::isfinite(actual_speeds.left_rad_s) ||
            !std::isfinite(actual_speeds.right_rad_s))
        {
            throw std::invalid_argument("Finite wheel speeds and a positive finite dt are required.");
        }
        
        const double next_left = left_angle_rad_ + actual_speeds.left_rad_s * dt_s;
        const double next_right = right_angle_rad_ + actual_speeds.right_rad_s * dt_s;
        const EncoderCounts next_counts{
            to_ticks(next_left, counts_per_revolution_),
            to_ticks(next_right, counts_per_revolution_)
        };
        left_angle_rad_ = next_left;
        right_angle_rad_ = next_right;
        counts_ = next_counts;
        return counts_;
    }

    EncoderCounts EncoderSimulator::read() const
    {
        return counts_;
    }
}