#include "pipeline_options.hpp"

#include <charconv>
#include <string>
#include <string_view>
#include <system_error>
#include <stdexcept>

namespace
{
    template <typename T> 

    T read_number (std::string_view text)
    {
        if (text.empty() || text.front() == '-')
        {
            throw std::invalid_argument(
                "Invalid non-negative integer: " + std::string(text)
            );
        }
        T value {};

        const auto [end , error] = std::from_chars(text.data() , text.data() + text.size() , value);

        if (error != std::errc() || end != text.data() + text.size())
        {
            throw std::invalid_argument(
                "Invalid or out-of-range integer: " + std::string(text)
            );
        }
        return value;
    }
}

PipelineOptions parse_options(int argc, char* argv[])
{
    PipelineOptions options;

    for(int i {1} ; i < argc ; i++)
    {
        const std::string_view key {argv[i]};

        if (key == "--help")
        {
            options.show_help = true ;
            return options ;
        }

        if (key != "--samples" && key != "intevals_ms"  && key != "--capacity" && key != "--consumer-delay-ms")
        {
            throw std::invalid_argument ("Unknown option: " + std::string(key));
        }

        if (++i >= argc)
        {
            throw std::invalid_argument ("Missing value for: " + std::string(key));
        }

        const std::string_view value {argv[i]};

        if (key == "--samples") 
        {
            options.sample_count = read_number<std::uint64_t>(value);
        } 
        else if (key == "--interval-ms") 
        {
            options.sample_interval = std::chrono::milliseconds{read_number<int>(value)};
        } 
        else if (key == "--capacity") 
        {
            options.queue_capacity = read_number<std::size_t>(value);
        } else 
        {
            options.consumer_delay = std::chrono::milliseconds{read_number<int>(value)};
        }


        if (options.queue_capacity == 0) 
        {
        throw std::invalid_argument(
            "--capacity must be greater than zero");
        }

    }
    return options;
}