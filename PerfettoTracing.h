#ifndef PERFETTOTRACING_H
#define PERFETTOTRACING_H

#include <fmt/core.h>

#include <optional>
#include <thread>
#include <vector>
#include <map>
#include <chrono>
#include <string>
#include <experimental/source_location>
#include <filesystem>

#ifdef ENABLE_PERFETTO_TRACE
  #ifndef STRINGIFY
    #define STRINGIFY(x) #x
  #endif
  #ifndef TOSTRING
    #define TOSTRING(x) STRINGIFY(x)
  #endif
  #define TRACE_FUNC(...) auto tracer = PerfettoTracing::TraceFunction(__VA_ARGS__);
  #define TRACE_LAMBDA(name, ...) auto tracer = PerfettoTracing::TraceLambda(name __VA_OPT__(,) __VA_ARGS__);
  #define TRACE_SCOPE(name, ...) auto tracer = PerfettoTracing::TraceScope(name __VA_OPT__(,) __VA_ARGS__);
  #define TRACE_VALUE(value) PerfettoTracing::TraceValue(STRINGIFY(value), value);
//  #define TRACE_OBJECT(name) PerfettoTracing::ObjectTracer _tracer = PerfettoTracing::ObjectTracer(name);
#else
  #define TRACE_FUNC()
  #define TRACE_LAMBDA(name)
  #define TRACE_SCOPE(name)
  #define TRACE_VALUE(name, value)
//  #define TRACE_OBJECT(name)
#endif

/**
 * @brief The PerfettoTracing class allows the creation of log files that can be
 * viewed in a browser at https://ui.perfetto.dev/. This allows for performance
 * tracing of a program, and also graphing of values over time. A very powerful
 * data visualisation tool, with support for stack analysis, as well as event
 * frequency and data value evolution analysis.
 *
 * While this can be used to roughly profile relative performance, a strack-
 * tracer tool would be much better. Instead this can be used to nicely
 * visualise what your program is doing, spot bugs, bottlenecks, unexpected
 * behaviour, etc.
 *
 * See documentation for file format here:
 * https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/preview#
 */
class PerfettoTracing {
public:
    enum class EventType : char {
        DurationBegin =   'B', // "Begin", follow with a matching 'E' event to create a DurationEvent
        DurationEnd =     'E', // "End", should follow a matching 'B' event to create a DurationEvent
        Duration =        'X', // "CompleteEvent", combines a B & E event in one entry
        Instantaneous =   'i', // "Instantaneous" event, an event with no duration
        Counter =         'C', // "Counter", used to track a value over time
        ObjectCreated =   'N', // "Object Event", used to track creation of an object
        ObjectSnapshot =  'O', // "Object Event", used to track an object when something happens
        ObjectDestroyed = 'D', // "Object Event", used to track destruction of an object
    };

    enum class EventScope : char {
        Global =  'g',
        Process = 'p',
        Thread =  't',
    };

    /**
     * name: (name) The name of the event, as displayed in Trace Viewer
     * cat: (sourceLocation) The event categories. This is a comma separated
     *      list of categories for the event. The categories can be used to hide
     *      events in the Trace Viewer UI.
     * ph: (type) The event type. This is a single character which changes
     *     depending on the type of event being output. The valid values are
     *     listed in the table below. We will discuss each phase type below.
     * ts: (timeStamp) The tracing clock timestamp of the event. The timestamps
     *     are provided at microsecond granularity.
     * tts: (duration) Optional. The thread clock timestamp of the event. The
     *      timestamps are provided at microsecond granularity.
     * pid: (traceSection) The process ID for the process that output this
     *      event.
     * tid: (traceSubSection) The thread ID for the thread that output this
     *      event.
     * args: (args) Any arguments provided for the event. Some of the event
     *       types have required argument fields, otherwise, you can put any
     *       information you wish in here. The arguments are displayed in Trace
     *       Viewer when you view an event in the analysis section.
     */
    struct Event {
        std::string name;
        std::string sourceLocation;
        EventType type;
        std::chrono::steady_clock::time_point timeStamp;
        std::optional<std::chrono::steady_clock::duration> duration;
        size_t process;
        std::thread::id thread;
        std::optional<std::string> id;
        std::optional<std::map<std::string, std::string>> args;
    };

    struct TraceWindow {
        std::string name;
        size_t samplesToCollect;
        std::chrono::steady_clock::time_point startTime;
    };

    class StackTracer {
    public:
        StackTracer(std::string name, std::string sourceLocation, std::thread::id threadId, std::optional<std::map<std::string, std::string>> args);
        StackTracer(const StackTracer& other) = delete;
        StackTracer(StackTracer&& other) = delete;
        ~StackTracer();

        StackTracer& operator=(const StackTracer& other) = delete;
        StackTracer operator=(StackTracer&& other) = delete;

    private:
        std::string name_;
        std::string sourceLocation_;
        std::thread::id  thread_;
        std::optional<std::map<std::string, std::string>> args_;
    };

    // The idea here is to make this a member variable of any object and the following function definitions will document what happens to the object
    // when writing the output, will need to use, a-sync events? put the same objects into the same process (not 0, that is reserved for the stack) and have as few rows as possible (i.e. num rows == max number of objects to exist in parallel at any time)
//    class ObjectTracer {
//    public:
//        ObjectTracer(std::string&& classname, std::string&& constructor = "default");
//        ObjectTracer(const ObjectTracer& other);
//        ObjectTracer(ObjectTracer&& other);
//        ~ObjectTracer();

//        ObjectTracer& operator=(const ObjectTracer& other);
//        ObjectTracer& operator=(ObjectTracer&& other);
//    private:
//        static inline size_t nextUniqueId = 0;

//        std::string classname_;
//        std::thread::id  thread_;
//        size_t id = nextUniqueId++;
//    };

    [[ nodiscard ]] static StackTracer TraceFunction(std::optional<std::map<std::string, std::string>> args = std::nullopt, const std::experimental::source_location location = std::experimental::source_location::current());
    [[ nodiscard ]] static StackTracer TraceLambda(const std::string& name, std::optional<std::map<std::string, std::string>> args = std::nullopt, const std::experimental::source_location location = std::experimental::source_location::current());
    [[ nodiscard ]] static StackTracer TraceScope(const std::string& name, std::optional<std::map<std::string, std::string>> args = std::nullopt, const std::experimental::source_location location = std::experimental::source_location::current());

    template <typename T> requires std::is_arithmetic_v<T>
    static void TraceValue(const std::string& name, T value, const std::experimental::source_location location = std::experimental::source_location::current())
    {
        AddEvent(name, fmt::format("{}:{}", location.file_name(), location.line()), EventType::Counter, std::chrono::steady_clock::now(), 0, std::this_thread::get_id(), std::nullopt, std::map<std::string, std::string>{ {"", std::to_string(value)} });
    }

    static void AddTraceWindow(std::string name, size_t eventCount, std::chrono::steady_clock::time_point traceStart);
    static void AddEvent(Event&& event);
    static void AddEvent(std::string name, std::string sourceLocation, PerfettoTracing::EventType type, std::chrono::steady_clock::time_point timeStamp, size_t process, std::thread::id threadId, std::optional<std::string> id = std::nullopt, std::optional<std::map<std::string, std::string>> args = std::nullopt);

private:
    // Make sure cached events are flushed to file when the program exits
    struct FlushOnExit {
        ~FlushOnExit();
    };

    static inline std::string traceDirectory_ = std::filesystem::current_path().string();
    static inline std::vector<TraceWindow> traceWindows_ = {};
    static inline std::vector<Event> events_ = {};
    static inline FlushOnExit unfinishedWindowEventFlusher = {};

    static StackTracer CreateStackTracer(const std::string& name, std::optional<std::map<std::string, std::string>> args, const std::experimental::source_location& location);

    static std::string ToString(const std::map<std::string, std::string>& pairs);
    static bool IsTracing();
    static void CheckCache();
    static void WriteToFile(std::string fileName, bool append = false);
};

#endif // PERFETTOTRACING_H
