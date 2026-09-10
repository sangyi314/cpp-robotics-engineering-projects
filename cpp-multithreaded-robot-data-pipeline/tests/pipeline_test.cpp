#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include "csv_logger.hpp"
#include "data_processor.hpp"
#include "sensor_simulator.hpp"

bool near (double actual , double expected)
{
    return std::abs(actual - expected) < 1e-9;
}

bool test_processor()
{
    ThreadSafeQueue<SensorData> input;
    ThreadSafeQueue<ProcessedData> output;

    const std::array<double, 4> raw_ax{2.0, 4.0, 9.0, 11.0};
    const std::array<double, 4> expected{2.0, 3.0, 5.0, 8.0};

    for (std::size_t i = 0; i < raw_ax.size(); ++i) 
    {
        SensorData data{};
        data.type = SensorType::IMU;
        data.sequence = i;
        data.imu.ax = raw_ax[i];

        if (!input.push(data)) 
        {
            return false;
        }
    }
    input.close();
    run_data_processer(input, output);

    for (std::size_t i = 0; i < expected.size(); ++i) 
    {
        const auto data = output.pop();

        if (!data
            || data->raw.sequence != i
            || !near(data->raw.imu.ax, raw_ax[i])
            || !near(data->filtered_ax, expected[i])) 
        {
            return false;
        }
    }

    return !output.pop();
}

bool test_csv_logger()
{
    ProcessedData data{};
    data.raw.type = SensorType::IMU;
    data.raw.sequence = 7;
    data.raw.timestamp_us = 123456;
    data.raw.imu = {1.25, -2.0, 9.81, 0.1, -0.2, 0.3};
    data.filtered_ax = 1.125;

    std::ostringstream csv;

    if (!write_csv_header(csv) || !write_csv_row(csv, data)) 
    {
        return false;
    }

    const std::string expected =
        "sequence,timestamp_us,raw_ax,raw_ay,raw_az,"
        "raw_gx,raw_gy,raw_gz,filtered_ax\n"
        "7,123456,1.250000,-2.000000,9.810000,"
        "0.100000,-0.200000,0.300000,1.125000\n";

    if (csv.str() != expected) 
    {
        return false;
    }

    std::ostringstream broken;
    broken.setstate(std::ios::badbit);

    return !write_csv_header(broken)
        && !write_csv_row(broken, data);
}

bool test_pipeline(std::uint64_t sample_count)
{
    ThreadSafeQueue<SensorData> raw_queue;
    ThreadSafeQueue<ProcessedData> processed_queue;

    SensorSimulatorConfig config{};
    config.sample_count = sample_count;

    config.sample_interval = std::chrono::milliseconds{0};

    std::ostringstream csv;
    bool ok = write_csv_header(csv);

    std::thread processor([&] {
        run_data_processer(raw_queue, processed_queue);
    });

    std::thread producer([&raw_queue, config] {
        run_sensor_simulator(raw_queue, config);
    });

    std::uint64_t received = 0;
    std::uint64_t last_timestamp = 0;

    while (auto data = processed_queue.pop()) {
        if (data->raw.sequence != received
            || data->raw.type != SensorType::IMU
            || !near(
                data->raw.imu.ax,
                0.1 * static_cast<double>(received))
            || data->raw.timestamp_us < last_timestamp) {
            ok = false;
        }

        last_timestamp = data->raw.timestamp_us;

        if (!write_csv_row(csv, *data)) {
            ok = false;
        }

        ++received;
    }

    producer.join();
    processor.join();

    const std::string text = csv.str();

    const auto line_count = static_cast<std::uint64_t>(
        std::count(text.begin(), text.end(), '\n'));

    return ok
        && received == sample_count
        && line_count == sample_count + 1;
}

int main()
{
    if (!test_processor()) {
        std::cerr
            << "FAIL: moving average or raw data preservation\n";
        return 1;
    }

    std::cout << "PASS: processor\n";

    if (!test_csv_logger()) {
        std::cerr << "FAIL: CSV format or error reporting\n";
        return 1;
    }

    std::cout << "PASS: CSV logger\n";

    for (const auto count :
         std::array<std::uint64_t, 5>{0, 1, 2, 20, 1000}) {
        if (!test_pipeline(count)) {
            std::cerr
                << "FAIL: pipeline, samples=" << count << '\n';
            return 1;
        }

        std::cout
            << "PASS: pipeline, samples=" << count << '\n';
    }

    return 0;
}