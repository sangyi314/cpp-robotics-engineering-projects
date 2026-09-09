#include <iostream>
#include <chrono>
#include <thread>
#include <cstdint>

#include "sensor_simulator.hpp"

int main()
{
    ThreadSafeQueue<SensorData> queue;
    SensorSimulatorConfig config ;

    config.sample_count = 20;
    config.sample_interval = std::chrono::milliseconds{100};

    std::thread producer([&queue , config]{
        run_sensor_simulator(queue , config);

    });

    std::uint64_t received_count {0};

    while(auto data = queue.pop())
    {
        std::cout << "seq= " << data -> sequence
                  << " time_us= " << data -> timestamp_us
                  << " ax= " << data -> imu.ax
                  << " ay= " << data -> imu.ay
                  << " az= " << data -> imu.az << '\n';

        received_count ++;

    }

    producer.join();

    std::cout << "Received " << received_count
              << " / " << config.sample_count << " samples\n";

    return received_count == config.sample_count ? 0 : 1;
}