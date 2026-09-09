#include <iostream>
#include <chrono>
#include <thread>
#include <cstdint>

#include "sensor_data.hpp"
#include "thread-safe-queue.hpp"

int main()
{
    ThreadSafeQueue<SensorData> queue;
    constexpr std::uint64_t sample_count {20};
    const auto start_time {std::chrono::steady_clock::now()};

    std::thread procuducer([&queue , sample_count , start_time]{
        for(std::uint64_t i {0} ; i < sample_count ; i++)
        {
            SensorData data;
            data.type = SensorType::IMU ;
            data.sequence = i ;

            const auto elapsed {std::chrono::steady_clock::now() - start_time};

            data.timestamp_us = static_cast<std::uint64_t>
            (std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count());

            data.imu.ax = 0.1 * static_cast<double>(i);
            data.imu.ay = 0.0 ;
            data.imu.az = 9.81 ;

            if (!queue.push(data))
            {
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        queue.close();

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

    procuducer.join();

    std::cout << "Received " << received_count
              << " / " << sample_count << " samples\n";

    return received_count == sample_count ? 0 : 1;
}