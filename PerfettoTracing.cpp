#include "PerfettoTracing.h"

#include <fmt/core.h>

#include <fstream>
#include <sstream>
#include <thread>
#include <iostream>
#include <thread>

PerfettoTracing::StackTracer PerfettoTracing::TraceFunction(std::optional<std::map<std::string, std::string> > args, const std::experimental::source_location location)
{
    return CreateStackTracer(location.function_name(), args, location);
}

PerfettoTracing::StackTracer PerfettoTracing::TraceLambda(const std::string& name, std::optional<std::map<std::string, std::string> > args, const std::experimental::source_location location)
{
    return CreateStackTracer(fmt::format("λ::{}", name), args, location);
}

PerfettoTracing::StackTracer PerfettoTracing::TraceScope(const std::string& name, std::optional<std::map<std::string, std::string> > args, const std::experimental::source_location location)
{
    return CreateStackTracer(fmt::format("scope::{}", name), args, location);
}

void PerfettoTracing::AddTraceWindow(std::string name, size_t eventCount, std::chrono::steady_clock::time_point traceStart)
{
    traceWindows_.push_back({ name, eventCount, traceStart });
    std::sort(std::begin(traceWindows_), std::end(traceWindows_), [](const TraceWindow& a, const TraceWindow& b) -> bool
    {
        return a.startTime < b.startTime;
    });
}

void PerfettoTracing::AddEvent(PerfettoTracing::Event&& event)
{
    if (IsTracing()) {
        if (events_.size() >= traceWindows_.front().samplesToCollect) {
            WriteToFile(traceWindows_.front().name, false);
            traceWindows_.erase(traceWindows_.begin());
            if (!traceWindows_.empty()) {
                events_.reserve(traceWindows_.front().samplesToCollect);
            }
        } else {
            events_.emplace_back(std::move(event));
        }
    }
}

void PerfettoTracing::AddEvent(std::string name, std::string sourceLocation, EventType type, std::chrono::steady_clock::time_point timeStamp, size_t process, std::thread::id threadId, std::optional<std::string> id, std::optional<std::map<std::string, std::string>> args)
{
    AddEvent(Event{ name, sourceLocation, type, timeStamp, std::nullopt, process, threadId, std::move(id), std::move(args) });
}

PerfettoTracing::StackTracer PerfettoTracing::CreateStackTracer(const std::string& name, std::optional<std::map<std::string, std::string> > args, const std::experimental::source_location& location)
{
    return StackTracer(name, fmt::format("{}:{}", location.file_name(), location.line()), std::this_thread::get_id(), args);
}

std::string PerfettoTracing::ToString(const std::map<std::string, std::string>& pairs)
{
    std::ostringstream ostr;

    ostr << "{ ";
    for (auto& [ key, value ] : pairs) {
        ostr << fmt::format(R"("{}": "{}")", key, value);
    }
    ostr << " }";

    return ostr.str();
}

bool PerfettoTracing::IsTracing()
{
    return !traceWindows_.empty() && std::chrono::steady_clock::now() >= traceWindows_.front().startTime;
}

PerfettoTracing::StackTracer::StackTracer(std::string name, std::string sourceLocation, std::thread::id thread, std::optional<std::map<std::string, std::string> > args)
    : name_(name)
    , sourceLocation_(sourceLocation)
    , thread_(thread)
    , args_(args)
{
    AddEvent(name, sourceLocation, EventType::DurationBegin, std::chrono::steady_clock::now(), 0, thread, std::nullopt, args);
}

PerfettoTracing::StackTracer::~StackTracer()
{
    AddEvent(name_, sourceLocation_, EventType::DurationEnd, std::chrono::steady_clock::now(), 0, thread_, std::nullopt, args_);
}

void PerfettoTracing::WriteToFile(std::string fileName, bool append)
{
    // Map the thread IDs to numbers starting at 0
    std::unordered_map<std::thread::id, size_t> betterThreadIds;

    // Map Object events to
    // map<classname, >
    // std::unordered_map<std::string, std::map<std::string, size_t>> betterObjectIds;

    std::ofstream fileWriter(std::filesystem::current_path().append(fileName + ".trace").string(), append ? std::ofstream::app : std::ofstream::trunc);

    // Start of file needs to include a begin array char and the first event is included to make subsequent events easier to add
    if (!append) {
        fileWriter << fmt::format(R"([ {{ "name" : "{}", "ph" : "{}", "ts" : {}, "pid" : 0 }})",
                                  "TraceStart",
                                  static_cast<char>(EventType::Instantaneous),
                                  std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
    }

    for (const Event& e : events_) {
        if (!betterThreadIds.contains(e.thread)) {
            betterThreadIds.insert(std::make_pair(e.thread, betterThreadIds.size()));
        }

        fileWriter << fmt::format(R"(,{} {{ "name" : "{}", "cat" : "{}", "ph" : "{}", "ts" : {}, "pid" : {}, "tid" : {}{}{}{} }})",
                                  "\n",
                                  e.name,
                                  e.sourceLocation,
                                  static_cast<char>(e.type),
                                  std::chrono::duration_cast<std::chrono::microseconds>(e.timeStamp.time_since_epoch()).count(),
                                  e.process,
                                  betterThreadIds.at(e.thread),
                                  e.id.has_value() ? e.id.value() : "",
                                  e.duration.has_value() ? fmt::format(R"__(, "dur" : {})__", std::chrono::duration_cast<std::chrono::microseconds>(e.duration.value()).count()) : "",
                                  e.args.has_value() ? fmt::format(R"__(, "args" : {})__", ToString(e.args.value())) : "");
    }

    events_.clear();
    std::cout << fmt::format("Trace file {}: {}", append ? "added to" : "created", traceDirectory_ + fileName + ".trace") << std::endl;
}

PerfettoTracing::FlushOnExit::~FlushOnExit()
{
    if (IsTracing()) {
        WriteToFile(traceWindows_.front().name, false);
    }
}

//PerfettoTracing::ObjectTracer::ObjectTracer(std::string&& classname, std::string&& constructor)
//    : classname_(classname)
//    , thread_(std::this_thread::get_id())
//{
//    AddEvent(classname_, constructor, EventType::ObjectCreated, std::chrono::steady_clock::now(), 0, thread_, fmt::format(R"__(, "id" : "{:#06x}")__", id));
//}

//PerfettoTracing::ObjectTracer::ObjectTracer(const ObjectTracer& other)
//    : classname_(other.classname_)
//    , thread_(std::this_thread::get_id())
//{
//    AddEvent(classname_, "copy constructed", EventType::ObjectCreated, std::chrono::steady_clock::now(), 0, thread_, fmt::format(R"__(, "id" : "{:#06x}")__", id));
//}

//PerfettoTracing::ObjectTracer::ObjectTracer(ObjectTracer&& other)
//    : classname_(other.classname_)
//    , thread_(std::this_thread::get_id())
//{
//    AddEvent(classname_, "move constructed", EventType::ObjectCreated, std::chrono::steady_clock::now(), 0, thread_, fmt::format(R"__(, "id" : "{:#06x}")__", id));
//}

//PerfettoTracing::ObjectTracer::~ObjectTracer()
//{
//    AddEvent(classname_, "destroyed", EventType::ObjectDestroyed, std::chrono::steady_clock::now(), 0, thread_, fmt::format(R"__(, "id" : "{:#06x}")__", id));
//}

//PerfettoTracing::ObjectTracer& PerfettoTracing::ObjectTracer::operator=(const ObjectTracer& other)
//{
//    classname_ = other.classname_;
//    thread_ = std::this_thread::get_id();
//    AddEvent(classname_, "copy assigned", EventType::ObjectCreated, std::chrono::steady_clock::now(), 0, thread_, fmt::format(R"__(, "id" : "{:#06x}")__", id));
//    return *this;
//}

//PerfettoTracing::ObjectTracer& PerfettoTracing::ObjectTracer::operator=(ObjectTracer&& other)
//{
//    classname_ = other.classname_;
//    thread_ = std::this_thread::get_id();
//    AddEvent(other.classname_, "move assigned", EventType::ObjectCreated, std::chrono::steady_clock::now(), 0, thread_, fmt::format(R"__(, "id" : "{:#06x}")__", id));
//    return *this;
//}
