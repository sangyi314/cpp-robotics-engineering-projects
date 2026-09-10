#include "data_processor.hpp"

#include <cstddef>
#include <queue>

void run_data_processer(ThreadSafeQueue<SensorData>& input , ThreadSafeQueue<ProcessedData>& output)
{
    constexpr std::size_t window_size {3};
    std::queue<double> window ;
    double sum {};

    while (auto data = input.pop())
    {
       const double ax = data -> imu.ax;
       window.push(ax);
       sum += ax ;

       if (window.size() > window_size)
       {
            sum -= window.front();
            window.pop();
       }

       ProcessedData result ;
       result.raw = *data ;

       result.filtered_ax = sum / static_cast<double>(window_size);

       if (!output.push(result))
       {
            break;
       }

       output.close();

    }







}