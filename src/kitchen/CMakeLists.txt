add_library(kitchen STATIC
	IngredientStock/IngredientStock.cpp
	cook/Cook.cpp
	threadPool/ThreadPool.cpp
)

target_include_directories(kitchen PUBLIC ..)

target_link_libraries(kitchen PUBLIC concurrency ipc pizza)
