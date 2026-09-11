#pragma once

#include <chrono>
#include <cstdint>
#include <cstddef>

struct PipelineOptions
{
    std::uint64_t sample_count {20};
    std::chrono::milliseconds sample_interval {10};
    std::size_t queue_capacity {5};
    std::chrono::milliseconds consumer_delay {0};
    bool show_help {false};
};

PipelineOptions parse_options(int argc , char* argv[]);