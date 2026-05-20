add_library(pizza STATIC
	Pizza.cpp
    Pizza.hpp
    factory/PizzaFactory.cpp
)

target_include_directories(pizza PUBLIC ..)
