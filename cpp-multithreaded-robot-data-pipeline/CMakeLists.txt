cmake_minimum_required (VERSION 3.28)

project (robot_data_pipeline
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(robot_pipeline
    src/main.cpp
)

target_include_directories(robot_pipeline
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)