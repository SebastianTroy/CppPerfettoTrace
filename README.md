# CppPerfettoTrace
A lightweight library to easily create Perfetto compatible traces of C++ code.

Example output at the end of this file.

See https://ui.perfetto.dev/ for further information and to view the trace files output by this library.

See https://troydev.co.uk/cppperfettotrace for this, and more of my projects.

Disclaimer
----------
There are many much more powerful ways to analyse, and create stack traces of, your code. This tool has runtime overhead and therefore will change how your code compiles and runs, especially with optimisation turned on.
That being said, I have found many of the best tools are platform specific, toolchain specific, require debug symbols, or have clunky or lacking ways to view the collected data.
I have personally used this tool to locate and fix bugs, and to help visualise complex systems so I thought I would publish it in case it could help someone else.

Usage
-----
You need to compile with at least C++20 and build with the "ENABLE_PERFETTO_TRACE" defined
The build definition allows you to leave your trace calls in your code with no runtime performance hit if tracing is not enabled.

You can easily copy and paste the two files into your project, add them to your build system, stick a `#define ENABLE_PERFETTO_TRACE` at the top of `PerfettoTracing.h` and bosh, you're done,

OR

To include this in your own CMAKE project as a statically linked library:
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
# Then in your project CMakeLists.txt add
include(FetchContent)
include(CppPerfettoTrace)

target_link_libraries(<InsertApplicationNameHere>
    PRIVATE
    CppPerfettoTrace
)

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

// Allows for finer detail to be collected on the content of a function, can be placed in if, else if, else, for, empty scopes e.t.c.
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
[ { "name" : "TraceStart", "ph" : "i", "ts" : 1987878044, "pid" : 0 },
 { "name" : "main", "cat" : "example/main.cpp:47", "ph" : "B", "ts" : 1984540556, "pid" : "Stack", "tid" : "1" },
 { "name" : "Iterate", "cat" : "example/main.cpp:9", "ph" : "B", "ts" : 1984540564, "pid" : "Stack", "tid" : "1" },
 { "name" : "scope::0", "cat" : "example/main.cpp:12", "ph" : "B", "ts" : 1984540570, "pid" : "Stack", "tid" : "1" },
 { "name" : "scope::0", "cat" : "example/main.cpp:12", "ph" : "E", "ts" : 1984797038, "pid" : "Stack", "tid" : "1" },
 { "name" : "scope::1", "cat" : "example/main.cpp:12", "ph" : "B", "ts" : 1984797072, "pid" : "Stack", "tid" : "1" },
 { "name" : "scope::1", "cat" : "example/main.cpp:12", "ph" : "E", "ts" : 1985050766, "pid" : "Stack", "tid" : "1" },
 { "name" : "scope::2", "cat" : "example/main.cpp:12", "ph" : "B", "ts" : 1985050787, "pid" : "Stack", "tid" : "1" },
 { "name" : "scope::2", "cat" : "example/main.cpp:12", "ph" : "E", "ts" : 1985315802, "pid" : "Stack", "tid" : "1" },
 { "name" : "scope::3", "cat" : "example/main.cpp:12", "ph" : "B", "ts" : 1985315833, "pid" : "Stack", "tid" : "1" },
 { "name" : "scope::3", "cat" : "example/main.cpp:12", "ph" : "E", "ts" : 1985566907, "pid" : "Stack", "tid" : "1" },
 { "name" : "Iterate", "cat" : "example/main.cpp:9", "ph" : "E", "ts" : 1985566918, "pid" : "Stack", "tid" : "1" },
 { "name" : "Recurse", "cat" : "example/main.cpp:19", "ph" : "B", "ts" : 1985566939, "pid" : "Stack", "tid" : "1" },
 { "name" : "Recurse", "cat" : "example/main.cpp:19", "ph" : "B", "ts" : 1985831827, "pid" : "Stack", "tid" : "1" },
 { "name" : "Recurse", "cat" : "example/main.cpp:19", "ph" : "B", "ts" : 1986083959, "pid" : "Stack", "tid" : "1" },
 { "name" : "Recurse", "cat" : "example/main.cpp:19", "ph" : "B", "ts" : 1986349223, "pid" : "Stack", "tid" : "1" },
 { "name" : "Recurse", "cat" : "example/main.cpp:19", "ph" : "B", "ts" : 1986602922, "pid" : "Stack", "tid" : "1" },
 { "name" : "Recurse", "cat" : "example/main.cpp:19", "ph" : "E", "ts" : 1986856829, "pid" : "Stack", "tid" : "1" },
 { "name" : "Recurse", "cat" : "example/main.cpp:19", "ph" : "E", "ts" : 1986856863, "pid" : "Stack", "tid" : "1" },
 { "name" : "Recurse", "cat" : "example/main.cpp:19", "ph" : "E", "ts" : 1986856864, "pid" : "Stack", "tid" : "1" },
 { "name" : "Recurse", "cat" : "example/main.cpp:19", "ph" : "E", "ts" : 1986856865, "pid" : "Stack", "tid" : "1" },
 { "name" : "Recurse", "cat" : "example/main.cpp:19", "ph" : "E", "ts" : 1986856867, "pid" : "Stack", "tid" : "1" },
 { "name" : "Threaded", "cat" : "example/main.cpp:28", "ph" : "B", "ts" : 1986856881, "pid" : "Stack", "tid" : "1" },
 { "name" : "λ::threadRun", "cat" : "example/main.cpp:32", "ph" : "B", "ts" : 1986857057, "pid" : "Stack", "tid" : "2" },
 { "name" : "λ::threadRun", "cat" : "example/main.cpp:32", "ph" : "B", "ts" : 1986857087, "pid" : "Stack", "tid" : "3" },
 { "name" : "λ::threadRun", "cat" : "example/main.cpp:32", "ph" : "B", "ts" : 1986857119, "pid" : "Stack", "tid" : "4" },
 { "name" : "λ::threadRun", "cat" : "example/main.cpp:32", "ph" : "B", "ts" : 1986857152, "pid" : "Stack", "tid" : "5" },
 { "name" : "λ::threadRun", "cat" : "example/main.cpp:32", "ph" : "E", "ts" : 1987861715, "pid" : "Stack", "tid" : "5" },
 { "name" : "λ::threadRun", "cat" : "example/main.cpp:32", "ph" : "E", "ts" : 1987861714, "pid" : "Stack", "tid" : "3" },
 { "name" : "λ::threadRun", "cat" : "example/main.cpp:32", "ph" : "E", "ts" : 1987861718, "pid" : "Stack", "tid" : "2" },
 { "name" : "Threaded", "cat" : "example/main.cpp:28", "ph" : "E", "ts" : 1987877471, "pid" : "Stack", "tid" : "1" },
 { "name" : "main", "cat" : "example/main.cpp:47", "ph" : "E", "ts" : 1987877486, "pid" : "Stack", "tid" : "1" }

````
![Output from Example program analysed in Perfetto Trace](https://img1.wsimg.com/isteam/ip/961afd39-a6a6-4a34-aeab-9ebf830fefd8/MyApplicationTrace.trace.png)

TODO
----
 - [X] Multi-threaded example
 - [X] Add to website and link to website in README
 - [X] Capture an image of the output when viewed in perfetto and place it in README and on website to make it look flashier
 - [ ] Implement the ability to add more than just a stack trace (e.g. Graphing variable values in tandem with the stack trace could be useful)
 - [ ] More flexibility for event window creation & early finishing
