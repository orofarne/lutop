add_library(lutop_lib STATIC
    SubProcess.cpp
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lua_cmsgpack.c ${CMAKE_CURRENT_BINARY_DIR}/lua_cmsgpack.c)
add_library(luacmsgpack_lib STATIC ${CMAKE_CURRENT_BINARY_DIR}/lua_cmsgpack.c)
add_dependencies(luacmsgpack_lib luacmsgpack)

add_executable(lutop main.cpp)
add_dependencies(lutop
    lutop_lib
    luajit
    luacmsgpack_lib
)
target_link_libraries(lutop
    lutop_lib
    luacmsgpack_lib
    ${Boost_LIBRARIES}
    ${LUAJIT_LIBRARY}
    "dl"
)

install(TARGETS lutop DESTINATION bin)
