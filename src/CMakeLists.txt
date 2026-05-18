add_executable(plazza
	main.cpp
)

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
