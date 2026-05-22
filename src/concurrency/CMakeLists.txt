find_package(Threads REQUIRED)

add_library(concurrency STATIC
    Thread.cpp
    Mutex.cpp
    ConditionVariable.cpp
    Process.cpp
)

target_include_directories(concurrency PUBLIC ..)
target_link_libraries(concurrency PUBLIC Threads::Threads)
