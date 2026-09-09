#pragma once

#include <cstdint>

enum class SensorType
{
    IMU,
    LIDAR,
    CAMERA,
    ENCODER
};

struct IMUData
{
    double ax {};
    double ay {};
    double az {};

    double gx {};
    double gy {};
    double gz {};
};

struct SensorData
{
    SensorType type;
    std::uint64_t sequnence {};
    std::uint64_t timestamp_us {};
    IMUData imu;
};