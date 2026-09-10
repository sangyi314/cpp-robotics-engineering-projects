#include <iostream>
#include <chrono>
#include <thread>
#include <cstdint>

#include "sensor_simulator.hpp"
#include "data_processor.hpp"
#include "processed_data.hpp"

int main()
{
    ThreadSafeQueue<SensorData> raw_queue ;
    ThreadSafeQueue<ProcessedData> processed_queue ;
    SensorSimulatorConfig config ;

    config.sample_count = 20;
    config.sample_interval = std::chrono::milliseconds{100};

    std::thread processer([&raw_queue , &processed_queue]{
        run_data_processer(raw_queue , processed_queue);
    });

    std::thread producer([&raw_queue , config]{
        run_sensor_simulator(raw_queue , config);

    });

    std::uint64_t received_count {0};

    while(auto data = processed_queue.pop())
    {
        std::cout << "seq= " << data -> raw.sequence
                  << " time_us= " << data -> raw.timestamp_us
                  << " ax= " << data -> raw.imu.ax
                  << " ay= " << data -> raw.imu.ay
                  << " az= " << data -> raw.imu.az 
                  << " filtered_ax=   " << data->filtered_ax << '\n';

        received_count ++;

    }
    processer.join();
    producer.join();

    std::cout << "Received " << received_count
              << " / " << config.sample_count << " samples\n";

    return received_count == config.sample_count ? 0 : 1;
}