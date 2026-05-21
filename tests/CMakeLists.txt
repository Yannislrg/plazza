add_executable(plazza_tests
    concurrency/ThreadTest.cpp
    concurrency/MutexTest.cpp
    concurrency/ProcessTest.cpp
    concurrency/MutexConcurrencyTest.cpp
    concurrency/ConditionVariableTest.cpp
    concurrency/ProcessConcurrencyTest.cpp
)

target_include_directories(plazza_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/concurrency
)

target_link_libraries(plazza_tests PRIVATE
    concurrency
    GTest::gtest_main
)

plazza_enable_target_warnings(plazza_tests)

include(GoogleTest)
gtest_discover_tests(plazza_tests)
