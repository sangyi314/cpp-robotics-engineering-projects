# C++ Robotics Engineering Project Series

A collection of practical C++ projects for learning robotics software engineering.

The series starts with core C++ and engineering skills, then extends toward robot kinematics, odometry, control, and ROS 2. Each implemented subproject contains its own source code, CMake configuration, tests, and usage documentation.

## Current Projects

| Project | Main topics | Status |
| --- | --- | --- |
| [C++ Multithreaded Robot Data Pipeline](cpp-multithreaded-robot-data-pipeline/README.md) | Threads, bounded queues, backpressure, filtering, CSV logging, command-line options, and CTest | Core learning prototype implemented |

The repository currently contains one implemented subproject. The robotics topics below describe the intended learning direction.

## Learning Goals

- Apply modern C++ through small, complete programs.
- Practice modular design, resource management, and concurrent programming.
- Use Git, CMake, and automated tests to make projects easier to build and maintain.
- Connect software design decisions to observable behavior and reproducible experiments.
- Build a foundation for later robotics and control projects.

## Current Tools

| Area | Tools |
| --- | --- |
| Language | C++23 and the C++ standard library |
| Concurrency | `std::thread`, mutexes, and condition variables |
| Build and test | CMake 3.28 or newer, CTest, and a compatible build tool |
| Development | Linux, a compiler with C++23 support, and Git |

The current pipeline uses the standard library and platform threading support. Its build does not require ROS 2 or an external C++ test framework.

## Quick Start

Clone the repository and enter the implemented subproject:

```bash
git clone https://github.com/sangyi314/cpp-robotics-engineering-projects.git
cd cpp-robotics-engineering-projects/cpp-multithreaded-robot-data-pipeline
```

Configure, build, test, and run:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure

./build/robot_pipeline --help
./build/robot_pipeline
```

The current CMake project is inside `cpp-multithreaded-robot-data-pipeline/`; run the commands above from that directory. A normal run writes `output/imu_data.csv` relative to the current working directory and replaces the previous file.

See the [pipeline README](cpp-multithreaded-robot-data-pipeline/README.md) for parameters, thread responsibilities, CSV fields, tests, and backpressure experiments.

## Project Organization

| Path | Purpose |
| --- | --- |
| [cpp-multithreaded-robot-data-pipeline/](cpp-multithreaded-robot-data-pipeline/) | Simulated sensor data pipeline and its CMake project |
| [include/](cpp-multithreaded-robot-data-pipeline/include/) | Data types, module interfaces, and the queue template |
| [src/](cpp-multithreaded-robot-data-pipeline/src/) | Application, simulator, processor, logger, and option parser |
| [tests/](cpp-multithreaded-robot-data-pipeline/tests/) | Queue and pipeline tests |

Build files and generated CSV output are local runtime artifacts. The experiment commands in the subproject README create a separate `docs/experiments/` directory for results that can be reviewed and committed.

## Learning Direction

1. Modern C++ and software engineering fundamentals.
2. Concurrency and simulated sensor data processing.
3. Robot kinematics, odometry, and motion simulation.
4. PID control and closed-loop behavior.
5. ROS 2 integration and broader robotics applications.

The current data pipeline provides practice in threading, synchronization, data ownership, and validation before adding hardware or robotics middleware.

