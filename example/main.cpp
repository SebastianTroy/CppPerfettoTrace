#include <PerfettoTracing.h>

#include <time.h>
#include <iostream>
#include <thread>

void Iterate(unsigned repeats)
{
    TRACE_FUNC()

    for (unsigned i = 0; i < repeats; ++i) {
        TRACE_SCOPE(std::to_string(i))
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / repeats));
    }
}

void Recurse(unsigned repeats, unsigned depth)
{
    TRACE_FUNC()
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / repeats));
    if (depth > 0) {
        Recurse(repeats, depth - 1);
    }
}

void Threaded(unsigned repeats)
{
    TRACE_FUNC()
    for (unsigned i = 0; i < repeats; ++i) {
        std::jthread thr{[]()
                         {
                             TRACE_LAMBDA("threadRun")
                             std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                         }};
        thr.detach();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1010));
}

int main(int /*argc*/, char** /*argv*/)
{
    size_t numberOfEventsToLog = 100;
    auto startOfTrace = std::chrono::steady_clock::now();
    PerfettoTracing::AddTraceWindow("MyApplicationTrace", numberOfEventsToLog, startOfTrace);

    // Most basic usage is to place this macro at the top of each function call you wish to trace
    TRACE_FUNC()

    unsigned repeats = 4;
    Iterate(repeats);
    Recurse(repeats, repeats);
    Threaded(repeats);
}
