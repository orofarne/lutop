file(GLOB LUTOP_TEST_SOURCES "*.cpp")
add_executable(lutop_test ${LUTOP_TEST_SOURCES})

add_dependencies(lutop_test
  lutop_lib
)

target_link_libraries(lutop_test
  lutop_lib
  ${GTEST_BOTH_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
)

run_test(lutop_test)
