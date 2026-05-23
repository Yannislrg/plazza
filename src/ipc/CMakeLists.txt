add_library(ipc STATIC
    MessageQueue.cpp
    PizzaSerializer.cpp
)

target_include_directories(ipc PUBLIC ..)
