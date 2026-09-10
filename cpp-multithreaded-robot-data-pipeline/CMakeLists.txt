cmake_minimum_required (VERSION 3.28)

project (robot_data_pipeline
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_executable(robot_pipeline
    src/main.cpp
    src/sensor_simulator.cpp
    src/data_processor.cpp
    src/csv_logger.cpp
)

target_include_directories(robot_pipeline
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(robot_pipeline
    PRIVATE
        Threads::Threads
)

add_executable(queue_test
    tests/queue_test.cpp
)

target_include_directories(queue_test
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(queue_test
    PRIVATE
        Threads::Threads
)

enable_testing()

add_test(
    NAME queue_test
    COMMAND queue_test
)

set_tests_properties(queue_test
    PROPERTIES TIMEOUT 5
)

add_executable(pipeline_test
    tests/pipeline_test.cpp
    src/sensor_simulator.cpp
    src/data_processor.cpp
    src/csv_logger.cpp
)

target_include_directories(pipeline_test
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(pipeline_test
    PRIVATE
        Threads::Threads
)

add_test(
    NAME pipeline_test
    COMMAND pipeline_test
)

set_tests_properties(pipeline_test
    PROPERTIES TIMEOUT 10
)