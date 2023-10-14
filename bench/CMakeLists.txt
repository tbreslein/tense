add_executable(bench bench.cpp)
target_compile_definitions(bench PRIVATE ${DEFAULT_COMPILE_DEFINITIONS})
target_link_libraries(bench ${TENSE_DEPS} nanobench)
