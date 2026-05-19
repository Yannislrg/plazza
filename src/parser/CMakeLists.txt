add_library(parser STATIC
	Parser.cpp
	Parser.hpp
)

target_include_directories(parser PUBLIC ..)
