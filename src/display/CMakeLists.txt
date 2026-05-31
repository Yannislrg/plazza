add_library(display STATIC
    Display.cpp
)

target_include_directories(display PUBLIC ..)
target_link_libraries(display PUBLIC pizza kitchen reception)
