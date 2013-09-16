cmake_minimum_required(VERSION 2.8)

project(lutop C CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

OPTION(TESTS "enable_testing" ON)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package( Boost 1.47.0 COMPONENTS system REQUIRED )
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(contrib)
add_subdirectory(src)

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

