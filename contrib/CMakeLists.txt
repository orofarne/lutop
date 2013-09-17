include(ExternalProject)

# LuaJIT
ExternalProject_Add(luajit
  URL "http://luajit.org/download/LuaJIT-2.0.1.tar.gz"
  URL_MD5 "85e406e8829602988eb1233a82e29f1f"
  DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}
  SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/LuaJIT-2.0.1
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND "make"
  INSTALL_COMMAND ""
)

set(LUAJIT_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/LuaJIT-2.0.1/src" PARENT_SCOPE)
set(LUAJIT_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/LuaJIT-2.0.1/src/libluajit.a" PARENT_SCOPE)

# lua-cmsgpack
ExternalProject_Add(luacmsgpack
  GIT_REPOSITORY "https://github.com/orofarne/lua-cmsgpack.git"
  DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}
  SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/lua-cmsgpack
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
)

set(LUACMSGPACK_SRC "${CMAKE_CURRENT_BINARY_DIR}/lua-cmsgpack/lua_cmsgpack.c" PARENT_SCOPE)

# msgpack-c-len
ExternalProject_Add(msgpackclen
  GIT_REPOSITORY "https://github.com/orofarne/msgpack-c-len.git"
  DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}
  SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/msgpack-c-len
  BUILD_IN_SOURCE 1
  INSTALL_COMMAND ""
  TEST_BEFORE_INSTALL 1
  TEST_COMMAND "./msglen_test"
)

set(MSGPACKCLEN_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}/msgpack-c-len" PARENT_SCOPE)
set(MSGPACKCLEN_LIBRARY "${CMAKE_CURRENT_BINARY_DIR}/msgpack-c-len/libmsgpack-c-len.a" PARENT_SCOPE)
