add_library(lutop_lib STATIC
    Manager.cpp
    Context.cpp
    SubProcess.cpp
    Module.cpp
    luatools.cpp
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/lua_cmsgpack.c ${CMAKE_CURRENT_BINARY_DIR}/lua_cmsgpack.c)
add_library(luacmsgpack_lib STATIC ${CMAKE_CURRENT_BINARY_DIR}/lua_cmsgpack.c)
add_dependencies(luacmsgpack_lib luacmsgpack)

add_executable(lutopd lutopd.cpp)
add_dependencies(lutopd
    lutop_lib
    luajit
    luacmsgpack_lib
)
target_link_libraries(lutopd
    lutop_lib
    luacmsgpack_lib
    ${Boost_LIBRARIES}
    ${LUAJIT_LIBRARY}
    ${MsgPack_LIBRARY}
    ${MSGPACKCLEN_LIBRARY}
    "dl"
)

install(TARGETS lutopd DESTINATION bin)
