#pragma once

#include "processed_data.hpp"
#include "thread_safe_queue.hpp"

void run_data_processer(ThreadSafeQueue<SensorData>& input , ThreadSafeQueue<ProcessedData>& output);