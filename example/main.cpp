#include <PerfettoTracing.h>

#include <time.h>
#include <iostream>
#include <filesystem>

void SomeFunc()
{
    TRACE_FUNC()

    auto print = [](int i)
    {
        TRACE_LAMBDA("print")
        std::cout << i << std::endl;
    };

    for (int i = 0; i < 10; ++i) {
        print(i);
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
}


