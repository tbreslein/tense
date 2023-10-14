add_executable(test test.cpp)
target_compile_definitions(test PRIVATE ${DEFAULT_COMPILE_DEFINITIONS})
target_link_libraries(test ${TENSE_DEPS} doctest)
