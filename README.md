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
[ { "name" : "TraceStart", "ph" : "i", "ts" : 9925939514, "pid" : 0 },
 { "name" : "main", "cat" : "example/main.cpp:62", "ph" : "B", "ts" : 9922655170, "pid" : 0, "tid" : 0 },
 { "name" : "Iterate", "cat" : "example/main.cpp:13", "ph" : "B", "ts" : 9922655174, "pid" : 0, "tid" : 0 },
 { "name" : "scope::0", "cat" : "example/main.cpp:16", "ph" : "B", "ts" : 9922655182, "pid" : 0, "tid" : 0 },
 { "name" : "globalNum", "cat" : "example/main.cpp:19", "ph" : "C", "ts" : 9922910528, "pid" : 0, "tid" : 0, "args" : { "": "1" } },
 { "name" : "scope::0", "cat" : "example/main.cpp:16", "ph" : "E", "ts" : 9922910546, "pid" : 0, "tid" : 0 },
 { "name" : "scope::1", "cat" : "example/main.cpp:16", "ph" : "B", "ts" : 9922910556, "pid" : 0, "tid" : 0 },
 { "name" : "globalNum", "cat" : "example/main.cpp:19", "ph" : "C", "ts" : 9923165188, "pid" : 0, "tid" : 0, "args" : { "": "2" } },
 { "name" : "scope::1", "cat" : "example/main.cpp:16", "ph" : "E", "ts" : 9923165201, "pid" : 0, "tid" : 0 },
 { "name" : "scope::2", "cat" : "example/main.cpp:16", "ph" : "B", "ts" : 9923165205, "pid" : 0, "tid" : 0 },
 { "name" : "globalNum", "cat" : "example/main.cpp:19", "ph" : "C", "ts" : 9923418056, "pid" : 0, "tid" : 0, "args" : { "": "3" } },
 { "name" : "scope::2", "cat" : "example/main.cpp:16", "ph" : "E", "ts" : 9923418069, "pid" : 0, "tid" : 0 },
 { "name" : "scope::3", "cat" : "example/main.cpp:16", "ph" : "B", "ts" : 9923418074, "pid" : 0, "tid" : 0 },
 { "name" : "globalNum", "cat" : "example/main.cpp:19", "ph" : "C", "ts" : 9923668063, "pid" : 0, "tid" : 0, "args" : { "": "4" } },
 { "name" : "scope::3", "cat" : "example/main.cpp:16", "ph" : "E", "ts" : 9923668088, "pid" : 0, "tid" : 0 },
 { "name" : "Iterate", "cat" : "example/main.cpp:13", "ph" : "E", "ts" : 9923668089, "pid" : 0, "tid" : 0 },
 { "name" : "Recurse", "cat" : "example/main.cpp:25", "ph" : "B", "ts" : 9923668092, "pid" : 0, "tid" : 0 },
 { "name" : "globalNum", "cat" : "example/main.cpp:29", "ph" : "C", "ts" : 9923920903, "pid" : 0, "tid" : 0, "args" : { "": "5" } },
 { "name" : "Recurse", "cat" : "example/main.cpp:25", "ph" : "B", "ts" : 9923920941, "pid" : 0, "tid" : 0 },
 { "name" : "globalNum", "cat" : "example/main.cpp:29", "ph" : "C", "ts" : 9924174667, "pid" : 0, "tid" : 0, "args" : { "": "6" } },
 { "name" : "Recurse", "cat" : "example/main.cpp:25", "ph" : "B", "ts" : 9924174680, "pid" : 0, "tid" : 0 },
 { "name" : "globalNum", "cat" : "example/main.cpp:29", "ph" : "C", "ts" : 9924430036, "pid" : 0, "tid" : 0, "args" : { "": "7" } },
 { "name" : "Recurse", "cat" : "example/main.cpp:25", "ph" : "B", "ts" : 9924430050, "pid" : 0, "tid" : 0 },
 { "name" : "globalNum", "cat" : "example/main.cpp:29", "ph" : "C", "ts" : 9924683032, "pid" : 0, "tid" : 0, "args" : { "": "8" } },
 { "name" : "Recurse", "cat" : "example/main.cpp:25", "ph" : "B", "ts" : 9924683048, "pid" : 0, "tid" : 0 },
 { "name" : "Recurse", "cat" : "example/main.cpp:25", "ph" : "E", "ts" : 9924938008, "pid" : 0, "tid" : 0 },
 { "name" : "Recurse", "cat" : "example/main.cpp:25", "ph" : "E", "ts" : 9924938020, "pid" : 0, "tid" : 0 },
 { "name" : "Recurse", "cat" : "example/main.cpp:25", "ph" : "E", "ts" : 9924938021, "pid" : 0, "tid" : 0 },
 { "name" : "Recurse", "cat" : "example/main.cpp:25", "ph" : "E", "ts" : 9924938022, "pid" : 0, "tid" : 0 },
 { "name" : "Recurse", "cat" : "example/main.cpp:25", "ph" : "E", "ts" : 9924938023, "pid" : 0, "tid" : 0 },
 { "name" : "Threaded", "cat" : "example/main.cpp:36", "ph" : "B", "ts" : 9924938034, "pid" : 0, "tid" : 0 },
 { "name" : "λ::threadRun", "cat" : "example/main.cpp:40", "ph" : "B", "ts" : 9924938214, "pid" : 0, "tid" : 1 },
 { "name" : "λ::threadRun", "cat" : "example/main.cpp:40", "ph" : "B", "ts" : 9924938244, "pid" : 0, "tid" : 2 },
 { "name" : "λ::threadRun", "cat" : "example/main.cpp:40", "ph" : "B", "ts" : 9924938291, "pid" : 0, "tid" : 3 },
 { "name" : "λ::threadRun", "cat" : "example/main.cpp:40", "ph" : "B", "ts" : 9924938322, "pid" : 0, "tid" : 4 },
 { "name" : "globalNum", "cat" : "example/main.cpp:43", "ph" : "C", "ts" : 9925178029, "pid" : 0, "tid" : 1, "args" : { "": "7" } },
 { "name" : "λ::threadRun", "cat" : "example/main.cpp:40", "ph" : "E", "ts" : 9925178041, "pid" : 0, "tid" : 1 },
 { "name" : "globalNum", "cat" : "example/main.cpp:43", "ph" : "C", "ts" : 9925401137, "pid" : 0, "tid" : 2, "args" : { "": "6" } },
 { "name" : "λ::threadRun", "cat" : "example/main.cpp:40", "ph" : "E", "ts" : 9925401152, "pid" : 0, "tid" : 2 },
 { "name" : "globalNum", "cat" : "example/main.cpp:43", "ph" : "C", "ts" : 9925622775, "pid" : 0, "tid" : 3, "args" : { "": "5" } },
 { "name" : "λ::threadRun", "cat" : "example/main.cpp:40", "ph" : "E", "ts" : 9925622787, "pid" : 0, "tid" : 3 },
 { "name" : "globalNum", "cat" : "example/main.cpp:43", "ph" : "C", "ts" : 9925844499, "pid" : 0, "tid" : 4, "args" : { "": "4" } },
 { "name" : "λ::threadRun", "cat" : "example/main.cpp:40", "ph" : "E", "ts" : 9925844514, "pid" : 0, "tid" : 4 },
 { "name" : "Threaded", "cat" : "example/main.cpp:36", "ph" : "E", "ts" : 9925938762, "pid" : 0, "tid" : 0 },
 { "name" : "main", "cat" : "example/main.cpp:62", "ph" : "E", "ts" : 9925938773, "pid" : 0, "tid" : 0 }
````

![Output from Example program analysed in Perfetto Trace](https://img1.wsimg.com/isteam/ip/961afd39-a6a6-4a34-aeab-9ebf830fefd8/MyApplicationTrace.png)

TODO
----
 - [X] Multi-threaded example
 - [X] Add to website and link to website in README
 - [X] Capture an image of the output when viewed in perfetto and place it in README and on website to make it look flashier
 - [X] Implement the ability to add more than just a stack trace (e.g. Graphing variable values in tandem with the stack trace could be useful)
 - [ ] More flexibility for event window creation & early finishing
 - [X] BUG the map of user definable options doesn't work when using the function directly
 - [ ] Complete overhaul
 - - [ ] Keep track of a stack of "Tracers", the user can create one and it will begin and end in its scope RAII style
 - - - A trace with a sub-trace in it would simply report the name of the subtrace for the duration the sub-trace was running
 - - - The "Time window" approach from before would still be nice to keep, 
 - - [ ] Add full compatibility for all event types
 - - [ ] Use templates and concepts to replace the macros but keep the ability for compile-time removal of all tracing code
 - - - Is this even possible? There is no guarantee that a function call will be optimised away even if the function is empty...
