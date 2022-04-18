# CppPerfettoTrace
 A lightweight utility to easily create Perfetto compatible traces of C++ code.

Usage
-----
You need to compile with at least C++20 and build with the "ENABLE_PERFETTO_TRACE" defined
The build definition allows you to leave your trace calls in your code with no runtime performance hit if tracing is not enabled.

To include this in your own CMAKE project:
```` CMAKE
# Add this file to your project directory, named "CppPerfettoTrace.cmake"
FetchContent_Declare(
    CppPerfettoTrace
    GIT_REPOSITORY  https://github.com/SebastianTroy/CppPerfettoTrace
    GIT_TAG         origin/main
)

set(CPP_PERFETTO_TRACE_BuildExample OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(CppPerfettoTrace)

include_directories(
    "${CppPerfettoTrace_SOURCE_DIR}"
)
````

```` CMAKE
# Then in your CMakeLists.txt add
include(CppPerfettoTrace.cmake)
# And optionally add the following to enable tracing, there is no runtime overhead to leaving trace macros in your code when this is not defined
add_compile_definitions(ENABLE_PERFETTO_TRACE)

````

The API
-------
```` C++
// Intended to be placed at the beginning of each function you wish to trace, it automatically detects the functions name
#define TRACE_FUNC()
// Intended to be placed at the beginning of each lambda definition you wish to trace, (TRACE_FUNC could be used instead, but the deduced function name can be very messy)
#define TRACE_LAMBDA(name)
// Allows for finer detail to be collected on the content of a function, can be placed in if, else if, else, empty scopes e.t.c.
#define TRACE_SCOPE(name)
// Used to initiate the collection of events. As the size of the output files can rapidly get very large it is often important to narrow down the trace to a more manageble size.
// name - The name of the file that will be generated containing the event data
// eventCount - The maximum number of events to store in a window
// traceStart - When to begin collecting events e.g. std::chrono::now() + std::chrono::seconds(60)
static void AddTraceWindow(std::string name, size_t eventCount, std::chrono::steady_clock::time_point traceStart);
````

Usage Example
-------------
```` C++
#include <PerfettoTracing.h>
#include <time.h>
#include <iostream>

void SomeFunc()
{
    TRACE_FUNC()
    ...
}

int main(int argc, char** argv)
{
    size_t numberOfEventsToLog = 100;
    auto startOfTrace = std::chrono::steady_clock::now();
    PerfettoTracing::AddTraceWindow("MyApplicationTrace", numberOfEventsToLog, startOfTrace);

    TRACE_FUNC() 
    {
        TRACE_SCOPE("Init")
        SomeFunc();
        MyApp::Init(argc, argv);
        MyApp::SplashScreen(argc, argv);
        ...
    }
}
````
Example Output
--------------
The following is produced when the example executable is built and run, this can be viewed at https://ui.perfetto.dev/ though you'll need to copy it into an otherwise empty file to view it.
```` json
[
 { "name" : "main", "cat" : "CppPerfettoTrace/example/main.cpp:29", "ph" : "B", "ts" : 36323967411, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "SomeFunc", "cat" : "CppPerfettoTrace/example/main.cpp:9", "ph" : "B", "ts" : 36323967424, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "B", "ts" : 36323967430, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "E", "ts" : 36323967605, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "B", "ts" : 36323967615, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "E", "ts" : 36323967786, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "B", "ts" : 36323967807, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "E", "ts" : 36323967931, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "B", "ts" : 36323967941, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "E", "ts" : 36323968055, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "B", "ts" : 36323968062, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "E", "ts" : 36323968144, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "B", "ts" : 36323968150, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "E", "ts" : 36323968237, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "B", "ts" : 36323968243, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "E", "ts" : 36323968336, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "B", "ts" : 36323968346, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "E", "ts" : 36323968430, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "B", "ts" : 36323968436, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "E", "ts" : 36323968499, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "B", "ts" : 36323968504, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "λ::print", "cat" : "CppPerfettoTrace/example/main.cpp:13", "ph" : "E", "ts" : 36323968561, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "SomeFunc", "cat" : "CppPerfettoTrace/example/main.cpp:9", "ph" : "E", "ts" : 36323968563, "pid" : "Stack", "tid" : "Thread: 1" },
 { "name" : "main", "cat" : "CppPerfettoTrace/example/main.cpp:29", "ph" : "E", "ts" : 36323968565, "pid" : "Stack", "tid" : "Thread: 1" }

````

TODO
----
 - [ ] Multi-threaded example
 - [ ] Implement the ability to add more than just a stack trace (e.g. Graphing variable values in tandem with the stack trace could be useful)
 - [ ] More flexibility for event window creation & early finishing
