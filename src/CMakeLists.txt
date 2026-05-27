add_executable(plazza
	main.cpp
)

target_include_directories(plazza PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(concurrency)
add_subdirectory(parser)
add_subdirectory(shell)
add_subdirectory(pizza)
add_subdirectory(ipc)
add_subdirectory(kitchen)
add_subdirectory(reception)

target_link_libraries(plazza PRIVATE concurrency parser shell pizza ipc kitchen reception)

if(ENABLE_TSAN AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_compile_options(plazza PRIVATE
		-fsanitize=thread
		-fno-omit-frame-pointer
		-g
	)
	target_link_options(plazza PRIVATE
		-fsanitize=thread
	)
endif()
