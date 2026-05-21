cmake_minimum_required(VERSION 3.28)

project(plazza)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(BUILD_TESTS "Build unit tests" OFF)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

if(APPLE)
  execute_process(
      COMMAND xcrun --sdk macosx --show-sdk-path
      OUTPUT_VARIABLE MACOS_SDK_PATH
      OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  add_compile_options(-isysroot ${MACOS_SDK_PATH})
endif()

function(plazza_enable_target_warnings target)
  target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -fPIC)
endfunction()

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)

add_subdirectory(src)

if(BUILD_TESTS)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
  enable_testing()
  add_subdirectory(tests)
endif()

find_program(CLANG_FORMAT NAMES clang-format-20 clang-format)
if(CLANG_FORMAT)
  add_custom_target(format
    COMMAND ${CLANG_FORMAT} -i ${ALL_SOURCE_FILES}
    COMMENT "Formatting code with ${CLANG_FORMAT}..."
  )
  add_custom_target(format-check
    COMMAND ${CLANG_FORMAT} --dry-run -Werror ${ALL_SOURCE_FILES}
    COMMENT "Checking code format with ${CLANG_FORMAT}..."
  )
endif()

find_program(CLANG_TIDY NAMES clang-tidy-20 clang-tidy)
if(CLANG_TIDY)
  add_custom_target(tidy
    COMMAND ${CLANG_TIDY} ${ALL_SOURCE_FILES} -p ${CMAKE_BINARY_DIR}
      --extra-arg=-Wno-unknown-warning-option
    COMMENT "Linting code with ${CLANG_TIDY}..."
  )
endif()
