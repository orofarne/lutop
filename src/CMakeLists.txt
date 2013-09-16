add_library(lutop_lib STATIC
    SubProcess.cpp
)

add_executable(lutop main.cpp)
add_dependencies(lutop
    lutop_lib
    luajit
)
target_link_libraries(lutop
    lutop_lib
    ${Boost_LIBRARIES}
    ${LUAJIT_LIBRARY}
)

install(TARGETS lutop DESTINATION bin)
