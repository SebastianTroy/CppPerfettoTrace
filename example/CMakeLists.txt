add_executable(ExampleTraceGenerator)

add_compile_definitions(ENABLE_PERFETTO_TRACE)

target_sources(ExampleTraceGenerator
    PUBLIC
    main.cpp
)

target_include_directories(ExampleTraceGenerator
    PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/CppPerfettoTrace
)

target_link_libraries(ExampleTraceGenerator
    PRIVATE
    CppPerfettoTrace
)
