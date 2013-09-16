include(ExternalProject)

ExternalProject_Add(luajit
  URL "http://luajit.org/download/LuaJIT-2.0.1.tar.gz"
  URL_MD5 "85e406e8829602988eb1233a82e29f1f"
  DOWNLOAD_DIR ${CMAKE_CURRENT_BINARY_DIR}
  SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/LuaJIT-2.0.1
  BUILD_IN_SOURCE 1
  LOG_BUILD 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND "make"
  INSTALL_COMMAND ""
)

include_directories("${CMAKE_CURRENT_BINARY_DIR}/LuaJIT-2.0.1/src")
set(LUAJIT_LIB "${CMAKE_CURRENT_BINARY_DIR}/LuaJIT-2.0.1/src/libluajit.a")
