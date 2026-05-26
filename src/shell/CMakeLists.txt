add_library(shell STATIC
	Shell.cpp
	Shell.hpp
)

target_include_directories(shell PUBLIC ..)
target_link_libraries(shell PUBLIC parser)
