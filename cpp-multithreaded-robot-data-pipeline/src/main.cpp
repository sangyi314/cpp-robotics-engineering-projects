#include <iostream>

#include "sensor_data.hpp"

int main()
{
    SensorData data;
    data.type = SensorType::IMU ;
    data.timestamp_us = 1000 ;

    data.imu.ax = 0.1 ;

    std::cout << "The timestamp of IMU is " << data.timestamp_us << '\n';
    return 0;
}