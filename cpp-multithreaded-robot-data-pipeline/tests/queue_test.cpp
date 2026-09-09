#include <chrono>
#include <iostream>
#include <optional>
#include <thread>

#include "thread-safe-queue.hpp"

bool test_fifo_and_close()
{
    ThreadSafeQueue<int> queue;

    const bool accepted_first = queue.push(10);
    const bool accepetd_second = queue.push(20);

    queue.close();
    queue.close();

    const auto first = queue.pop();
    const auto second = queue.pop();
    const auto end = queue.pop();

    return (accepted_first && accepetd_second && first && *first == 10 && second && *second == 20 && !end && !queue.push(30));


}

bool test_push_wakes_consumer()
{
    ThreadSafeQueue<int> queue;
    std::optional<int> received;

    std::thread consumer([&]
    {
        received = queue.pop();
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    bool accepeted = queue.push(42);

    consumer.join();
    queue.close();

    return (accepeted && received && *received ==42);
}

bool test_close_wakes_consumer()
{
    ThreadSafeQueue<int> queue;
    std::optional<int> received;

    std::thread consumer([&]{
        received = queue.pop();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    queue.close();

    consumer.join();

    return !received;
    

}

int main()
{
    if (!test_fifo_and_close())
    {
        std::cerr << "FAIL: FIFO and close rules\n";
        return 1;
    }
    std::cout << "PASS: FIFO and close rules\n";

    if (!test_push_wakes_consumer())
    {
        std::cerr << "FAIL: push wakes consumer\n";
        return 1;
    }
    std::cout << "PASS: push wakes consumer\n";

    if (!test_close_wakes_consumer())
    {
        std::cerr << "FAIL: close wakes consumer\n";
        return 1;
    }
    std::cout << "PASS: close wakes consumer\n";
    return 0;
}