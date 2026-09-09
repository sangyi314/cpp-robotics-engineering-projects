#include "sensor_simulator.hpp"

#include <thread>

void run_sensor_simulator(ThreadSafeQueue<SensorData>& output, const SensorSimulatorConfig& config)
{
    const auto start_time = std::chrono::steady_clock::now();

    for (std::uint64_t i{0}; i < config.sample_count; ++i)
    {
        SensorData data{};
        data.type = SensorType::IMU;
        data.sequence = i;

        const auto elapsed =
            std::chrono::steady_clock::now() - start_time;

        data.timestamp_us = static_cast<std::uint64_t>(
            std::chrono::duration_cast<
                std::chrono::microseconds>(elapsed).count());

        data.imu.ax = 0.1 * static_cast<double>(i);
        data.imu.ay = 0.0;
        data.imu.az = 9.81;

        if (!output.push(data))
        {
            break;
        }

        std::this_thread::sleep_for(config.sample_interval);
    }

    output.close();
}