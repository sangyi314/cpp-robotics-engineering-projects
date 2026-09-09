#pragma once

#include <chrono>
#include <cstdint>

#include "sensor_data.hpp"
#include "thread_safe_queue.hpp"

struct SensorSimulatorConfig
{
    std::uint64_t sample_count {};
    std::chrono::milliseconds sample_interval {100};
};

void run_sensor_simulator(ThreadSafeQueue<SensorData>& output, const SensorSimulatorConfig& config);