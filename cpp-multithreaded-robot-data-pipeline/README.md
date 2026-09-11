# C++ Multithreaded Robot Data Pipeline

A C++23 learning project that generates simulated IMU samples, processes them in a worker thread, and saves the results to CSV.

The application uses two bounded queues to connect a producer thread, a processing thread, and a consumer running on the main thread. Its purpose is to make synchronization, backpressure, filtering, and orderly shutdown observable in a small program.

[Back to the project series](../README.md)

## Implemented Features

- Deterministic simulated IMU samples with sequence numbers and timestamps.
- A bounded FIFO queue implemented with a mutex and condition variables.
- Blocking backpressure when a queue is full.
- A three-sample moving average applied to the acceleration field `ax`.
- Preservation of the original sample alongside the filtered value.
- CSV output with fixed floating-point formatting and stream error checks.
- Command-line control of sample count, producer delay, queue capacity, and consumer delay.
- Runtime counts, elapsed time, throughput, queue peaks, and producer wait counts.
- Three tests registered with CTest.

## Requirements

- Linux.
- CMake 3.28 or newer.
- A C++ compiler and standard library that support the project's C++23 build mode.
- A build tool supported by CMake, such as Make or Ninja.

The project uses the C++ standard library and platform threading support. CMake locates that support through `Threads::Threads`.

## Build and Run

From the repository root, enter the subproject:

```bash
cd cpp-multithreaded-robot-data-pipeline
```

Run all remaining commands in this README from that directory.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel

./build/robot_pipeline --help
./build/robot_pipeline
```

The default run generates 20 samples and writes them to `output/imu_data.csv`.

## Command-Line Options

| Option | Default | Meaning |
| --- | --- | --- |
| `--samples N` | `20` | Number of samples to generate; zero is allowed |
| `--interval-ms N` | `10` | Producer sleep duration after each successful enqueue, in milliseconds |
| `--capacity N` | `5` | Capacity of each of the two queues; must be greater than zero |
| `--consumer-delay-ms N` | `0` | Main-thread sleep duration after consuming each sample, in milliseconds |
| `--help` | — | Print usage information and exit |

Pass numeric options as a flag followed by a separate decimal integer. Negative values, invalid numbers, values outside the supported integer range, missing values, and unknown options are rejected.

```bash
./build/robot_pipeline \
  --samples 200 \
  --interval-ms 1 \
  --capacity 5 \
  --consumer-delay-ms 10
```

The producer interval is a requested sleep duration. Processing overhead, queue waits, and operating-system scheduling also affect the actual spacing between samples.

A successful complete run, or `--help`, returns exit code `0`. Invalid arguments, reported file errors, and final sample-count mismatches return `1`.

## Threads and Data Flow

| Execution context | Input | Work | Output |
| --- | --- | --- | --- |
| Producer thread | `SensorSimulatorConfig` | Generate a simulated `SensorData` sample | Raw queue |
| Processing thread | Raw queue | Preserve the raw sample and calculate `filtered_ax` | Processed queue |
| Main thread | Processed queue | Print, write CSV, count samples, and apply the optional consumer delay | Console and CSV file |

The raw queue stores `SensorData`. The processed queue stores `ProcessedData`, which contains both the original sample and its filtered acceleration. Both queues use the capacity selected by `--capacity`.

### Queue behavior

- `push()` waits while the queue is full and open.
- `pop()` waits while the queue is empty and open.
- `close()` wakes waiting producers and consumers.
- A push to a closed queue returns `false`.
- Samples already in a closed queue can still be consumed.
- Once the queue is closed and empty, `pop()` returns `std::nullopt`.

Waiting uses condition variables. During normal completion, a full queue slows the upstream stage instead of discarding samples.

### Normal shutdown

1. The producer finishes its configured sample count and closes the raw queue.
2. The processor drains the raw queue, then closes the processed queue.
3. The main thread drains the processed queue, joins both worker threads, and closes the CSV file.

This sequence lets queued samples reach the consumer before the application exits.

## Simulated Data and Filtering

Each sample has a sequence number starting at zero. The current simulator sets:

| Field | Value |
| --- | --- |
| `type` | `SensorType::IMU` |
| `imu.ax` | `0.1 * sequence` |
| `imu.ay` | `0.0` |
| `imu.az` | `9.81` |
| `imu.gx`, `imu.gy`, `imu.gz` | `0.0` |

`timestamp_us` is the elapsed time in microseconds from the simulator's start, measured with `std::chrono::steady_clock` before enqueueing the sample. It is neither a Unix timestamp nor a measurement of when the consumer receives the sample.

The processor maintains the latest three `ax` values and their sum. It divides the sum by the number of values currently in the window. The first two outputs therefore use one and two samples respectively.

For example, input values `2, 4, 9, 11` produce filtered values `2, 3, 5, 8`. Only `ax` is filtered; all raw fields remain available in `ProcessedData::raw`.

## CSV Output

The file contains one header row followed by one row per saved sample:

```csv
sequence,timestamp_us,raw_ax,raw_ay,raw_az,raw_gx,raw_gy,raw_gz,filtered_ax
```

| Columns | Meaning |
| --- | --- |
| `sequence` | Sample index, starting at zero |
| `timestamp_us` | Sample-generation time relative to the simulator's start |
| `raw_ax`, `raw_ay`, `raw_az` | Original simulated acceleration fields |
| `raw_gx`, `raw_gy`, `raw_gz` | Original simulated angular-velocity fields |
| `filtered_ax` | Moving average of the available latest three `ax` values |

Floating-point fields use six decimal places and a locale-independent decimal point. Integer fields retain integer formatting.

The program creates the `output/` directory if necessary. The path is relative to the process's current working directory, and each normal run opens the CSV in truncate mode, replacing previous contents. With `--samples 0`, the output contains only the header.

When using the commands in this README, the file is in the subproject's `output/` directory. The CTest command-line test runs from the build directory and creates its own `build/output/imu_data.csv`.

## Runtime Statistics

| Output | Meaning |
| --- | --- |
| `Received` | Samples consumed by the main thread, compared with the requested count |
| `Saved` | Rows accepted by the CSV stream; success is reported after checking file close |
| `Elapsed` | Seconds from queue/thread setup through completion of CSV close |
| `Rate` | Saved row count divided by elapsed seconds |
| `peak` | Largest number of samples simultaneously stored in that queue |
| `push_waits` | Push calls that found an open queue full when checking under its mutex |

`Rate` includes the configured sleeps, synchronization, thread scheduling, console output, and CSV writing. It describes the application run rather than the processor's isolated calculation speed. Initial argument parsing and CSV header setup occur before the elapsed-time measurement.

`push_waits` is a count, not a duration or a count of condition-variable wakeups. Exact timing and wait counts vary between runs. Each queue's `peak` should remain at or below its configured capacity.

## Tests

After building, run:

```bash
ctest --test-dir build --output-on-failure
```

| Test | Coverage |
| --- | --- |
| `queue_test` | FIFO ordering, repeated close, draining after close, rejection of pushes after close, and consumer wakeup scenarios |
| `pipeline_test` | Moving-average results, raw-data preservation, CSV formatting, failed-stream reporting, and pipeline runs with 0, 1, 2, 20, and 1000 samples |
| `cli_interval_test` | The executable accepts `--interval-ms` and exits successfully with zero samples |

The pipeline test also checks sample order, simulated values, nondecreasing timestamps, and CSV line count. A successful CTest run reports three passing tests.

## Backpressure Experiments

These three scenarios keep the sample count and producer interval fixed while changing consumer delay or queue capacity.

Create a directory for run logs:

```bash
mkdir -p docs/experiments
```

### 1. Baseline

```bash
./build/robot_pipeline \
  --samples 200 --interval-ms 1 \
  --capacity 5 --consumer-delay-ms 0 \
  > docs/experiments/baseline.txt
```

### 2. Slow consumer

```bash
./build/robot_pipeline \
  --samples 200 --interval-ms 1 \
  --capacity 5 --consumer-delay-ms 10 \
  > docs/experiments/slow-consumer.txt
```

### 3. Larger queues with the same slow consumer

```bash
./build/robot_pipeline \
  --samples 200 --interval-ms 1 \
  --capacity 50 --consumer-delay-ms 10 \
  > docs/experiments/larger-queue.txt
```

Inspect the final statistics from all three logs:

```bash
tail -n 6 docs/experiments/*.txt
```

Each successful run should receive and save 200 samples. The CSV from the last run should contain 201 lines, including the header:

```bash
wc -l output/imu_data.csv
```

The CSV is replaced on each run; the separate text logs retain each run's counts and statistics.

### What to compare

| Scenario | Expected behavior |
| --- | --- |
| Baseline | The consumer can usually keep up; queues tend to stay small |
| Slow consumer | Queues tend to fill, causing upstream push calls to wait |
| Larger queues | More samples can be buffered, but the same slow consumer still limits sustained throughput |

With a 10 ms consumer sleep per sample, throughput is typically near or below 100 samples per second once overhead is included. Increasing capacity adds buffering space; it does not make the consumer process each sample faster.

Record elapsed time, rate, and both queues' peaks and wait counts on your own machine. Include the compiler, build configuration, operating system, and whether the run used a virtual machine when sharing results. These are expected trends, not fixed timing assertions.

## Source Map

| Files | Responsibility |
| --- | --- |
| [src/main.cpp](src/main.cpp) | Application setup, threads, consumption, file handling, and statistics |
| [include/sensor_data.hpp](include/sensor_data.hpp), [include/processed_data.hpp](include/processed_data.hpp) | Raw and processed sample types |
| [include/thread_safe_queue.hpp](include/thread_safe_queue.hpp) | Bounded queue, synchronization, close behavior, and queue statistics |
| [include/sensor_simulator.hpp](include/sensor_simulator.hpp), [src/sensor_simulator.cpp](src/sensor_simulator.cpp) | Simulator configuration and sample generation |
| [include/data_processor.hpp](include/data_processor.hpp), [src/data_processor.cpp](src/data_processor.cpp) | Moving-average processing |
| [include/csv_logger.hpp](include/csv_logger.hpp), [src/csv_logger.cpp](src/csv_logger.cpp) | CSV formatting and output-stream checks |
| [include/pipeline_options.hpp](include/pipeline_options.hpp), [src/pipeline_options.cpp](src/pipeline_options.cpp) | Defaults and command-line parsing |
| [tests/queue_test.cpp](tests/queue_test.cpp), [tests/pipeline_test.cpp](tests/pipeline_test.cpp) | Test programs |
| [CMakeLists.txt](CMakeLists.txt) | Executable targets, threading support, and all three CTest registrations |

## Current Scope

- Input is deterministic simulated IMU data; hardware acquisition and ROS 2 integration are future work.
- The pipeline has one producer, one processing worker, and one consumer.
- Filtering currently covers only `ax`, with a fixed window size of three.
- The simulator uses relative sleeps and does not provide a hard real-time sampling guarantee.
- Shutdown follows completion of the configured sample count. Signal-based cancellation and worker-exception propagation are not implemented.

