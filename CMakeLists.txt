cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)  
endif()

project(camera_v4l_rtsp)
set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Debug)
endif()
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

#set variables
SET(BUILD_CTL OFF CACHE BOOL "Build drogon_ctl" FORCE)
SET(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
SET(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "Build unittest" FORCE)
SET(JSONCPP_WITH_TESTS OFF CACHE BOOL "Build tests" FORCE)
SET(BUILD_SHARED_LIBS OFF)
SET(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "Build example" FORCE)
SET(BUILD_POSTGRESQL OFF CACHE BOOL "Build POSTGRESQL" FORCE)
SET(BUILD_MYSQL  OFF CACHE BOOL "Build MYSQL" FORCE)
SET(BUILD_SQLITE OFF CACHE BOOL "Build SQLITE" FORCE)
SET(BUILD_REDIS  OFF CACHE BOOL "Build REDIS" FORCE)
SET(TRANTOR_USE_TLS "" CACHE STRING "TLS provider for trantor. Valid options: 'openssl', 'botan' or ''" FORCE)
SET(USE_SPDLOG OFF CACHE BOOL "Use spdlog as logger" FORCE)
IF(WIN32)
  SET(SPDLOG_BUILD_SHARED ON CACHE BOOL "build spdlog shared" FORCE)
ELSE()
  SET(SPDLOG_BUILD_SHARED OFF CACHE BOOL "build spdlog shared" FORCE)  
ENDIF()

#fetch dependecies
include("PatchFile")
include("FetchZlib")  
include("FetchLive555")
include("FetchSpdlog")
include("FetchJsoncpp")  
include("FetchDrogon")

#set target projects
add_subdirectory(3rdparty/spdlog)
add_subdirectory(3rdparty/jsoncpp)
add_subdirectory(3rdparty/drogon)
add_subdirectory(src)

#stupid hack for using spdlog without fmt-library in drogon
target_compile_definitions(trantor PUBLIC TRANTOR_SPDLOG_SUPPORT)
target_compile_definitions(drogon PUBLIC DROGON_SPDLOG_SUPPORT)

message("")
message("  CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("")
message("    LIVE555_DIR = ${LIVE555_DIR}")
message("    Drogon_DIR = ${Drogon_DIR}")
message("    Jsoncpp_DIR = ${Jsoncpp_DIR}")
message("    Spdlog_DIR = ${Spdlog_DIR}")
message("")
