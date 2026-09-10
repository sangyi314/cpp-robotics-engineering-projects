#pragma once

#include "sensor_data.hpp"

struct ProcessedData
{
    SensorData raw;
    double filtered_ax {};
};