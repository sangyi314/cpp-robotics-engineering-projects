cmake_minimum_required (VERSION 3.28)

project(cpp_differential_drive_robot_kinematics,odometry,and_PID_control_simulation
        VERSION 1.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(robot_sim
    src/main.cpp
    src/differential_drive.cpp
    src/motion_model.cpp
    )

target_include_directories(robot_sim
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )


