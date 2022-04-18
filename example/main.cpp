#include <PerfettoTracing.h>

#include <time.h>
#include <iostream>
#include <thread>

void SomeFunc()
{
    TRACE_FUNC()

    auto print = [](int i)
    {
        TRACE_LAMBDA("print")
        std::cout << "Loop Index: " << i << std::endl;
    };

    for (int i = 0; i < 10; ++i) {
        print(i);
    }
}

void Threaded()
{
    TRACE_FUNC()
    for (int i = 0; i < 10; ++i) {
        std::jthread thr{[]()
                         {
                             TRACE_LAMBDA("sleep 50 micro seconds")
                             std::this_thread::sleep_for(std::chrono::microseconds(50));
                             std::cout << "Thread ID: " << std::this_thread::get_id() << std::endl;
                         }};
        thr.detach();
    }
}

int main(int argc, char** argv)
{
    size_t numberOfEventsToLog = 100;
    auto startOfTrace = std::chrono::steady_clock::now();
    PerfettoTracing::AddTraceWindow("MyApplicationTrace", numberOfEventsToLog, startOfTrace);

    // Most basic usage is to place this macro at the top of each function call you wish to trace
    TRACE_FUNC()

    SomeFunc();
    Threaded();
}
