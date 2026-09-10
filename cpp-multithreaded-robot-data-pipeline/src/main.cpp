#include <iostream>
#include <chrono>
#include <thread>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <system_error>

#include "sensor_simulator.hpp"
#include "data_processor.hpp"
#include "processed_data.hpp"
#include "csv_logger.hpp"

int main()
{
    const std::filesystem::path output_path{"output/imu_data.csv"};

    std::error_code error;
    std::filesystem::create_directories(output_path.parent_path() , error);

    if(error)
    {
        std::cerr << "Cannot create output directory: " << error.message() << '\n';
        return 1;
    }

    std::ofstream csv(output_path , std::ios::out | std::ios::trunc);

    if(!csv)
    {
        std::cerr << "Cannot open CSV: " << output_path << '\n';
        return 1;
    }

    if(!write_csv_header(csv))
    {
         std::cerr << "Cannot write CSV header\n";
        return 1;
    }

    ThreadSafeQueue<SensorData> raw_queue {5};
    ThreadSafeQueue<ProcessedData> processed_queue {5};
    SensorSimulatorConfig config ;

    config.sample_count = 20;
    config.sample_interval = std::chrono::milliseconds{10};

    std::thread processer([&raw_queue , &processed_queue]{
        run_data_processer(raw_queue , processed_queue);
    });

    std::thread producer([&raw_queue , config]{
        run_sensor_simulator(raw_queue , config);

    });

    std::uint64_t received_count {0};
    std::uint64_t written_count {0};
    bool csv_ok {true};

    while(auto data = processed_queue.pop())
    {
        std::cout << "seq= " << data -> raw.sequence
                  << " time_us= " << data -> raw.timestamp_us
                  << " ax= " << data -> raw.imu.ax
                  << " ay= " << data -> raw.imu.ay
                  << " az= " << data -> raw.imu.az 
                  << " filtered_ax=   " << data->filtered_ax << '\n';

        received_count ++;

        if(csv_ok)
        {
            csv_ok = write_csv_row(csv , *data);

            if(csv_ok)
            {
                written_count++;
            }
        }
    }
    processer.join();
    producer.join();

    csv.close();
    if(!csv)
    {
        csv_ok = false ;
    }

    std::cout << "Received " << received_count
              << " / " << config.sample_count << " samples\n";

    if (!csv_ok) {
        std::cerr
            << "CSV write or close failed; file may be incomplete\n";
        return 1;
    }

    std::cout << "Saved " << written_count
              << " rows to " << output_path << '\n';

    return received_count == config.sample_count
        && written_count == received_count ? 0 : 1;
}