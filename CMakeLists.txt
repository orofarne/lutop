cmake_minimum_required(VERSION 2.8)

project(lutop C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

OPTION(TESTS "enable_testing" ON)

# Initialize CXXFLAGS.
set(CMAKE_CXX_FLAGS                "-Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

# Compiler-specific C++11 activation.
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
  if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
    message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
  endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else ()
  message(FATAL_ERROR "Your C++ compiler does not support C++11.")
endif ()

# Setup boost
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package( Boost 1.47.0 COMPONENTS system REQUIRED )
include_directories(${Boost_INCLUDE_DIRS})

# LuaJIT
add_subdirectory(contrib)

# Code
add_subdirectory(src)

# GTest magic
if(TESTS)
  find_package(Threads REQUIRED)
  find_package(GTest)

  message(STATUS "GTest include dir: ${GTEST_INCLUDE_DIR}")
  include_directories(${GTEST_INCLUDE_DIR})

  if(NOT GTEST_BOTH_LIBRARIES)
    if(EXISTS /usr/src/gtest)
      add_subdirectory( /usr/src/gtest ${PROJECT_BINARY_DIR}/gtest )
      set(GTEST_BOTH_LIBRARIES gtest gtest_main)
    else(EXISTS /usr/src/gtest)
      message(FATAL_ERROR "GTest not found")
    endif(EXISTS /usr/src/gtest)
  endif(NOT GTEST_BOTH_LIBRARIES)

  add_custom_target(test)
  macro(run_test test_target)
    add_custom_target(${test_target}_runtest
        COMMAND ${test_target}
        DEPENDS ${test_target}
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
    add_dependencies(test ${test_target}_runtest)
  endmacro()

  add_subdirectory(tests)
endif(TESTS)
