cmake_minimum_required(VERSION 3.10...3.21)
project(camera_v4l_rtsp)
set(CMAKE_CXX_STANDARD 17)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
	cmake_policy(SET CMP0135 NEW)
endif()

if(POLICY CMP0074)
  cmake_policy(SET CMP0074 OLD)
endif()

if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif() 

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

SET(BUILD_CTL OFF CACHE BOOL "Build drogon_ctl" FORCE)
SET(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
SET(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "Build unittest" FORCE)
SET(JSONCPP_WITH_TESTS OFF CACHE BOOL "Build tests" FORCE)
SET(BUILD_STATIC_LIBS ON)
SET(BUILD_OBJECT_LIBS OFF)
SET(BUILD_SHARED_LIBS OFF)
SET(SPDLOG_BUILD_EXAMPLE OFF CACHE BOOL "Build example" FORCE)
SET(BUILD_POSTGRESQL OFF CACHE BOOL "Build POSTGRESQL" FORCE)
SET(BUILD_MYSQL  OFF CACHE BOOL "Build MYSQL" FORCE)
SET(BUILD_SQLITE OFF CACHE BOOL "Build SQLITE" FORCE)
SET(BUILD_REDIS  OFF CACHE BOOL "Build REDIS" FORCE)
SET(LIBPQ_BATCH_MODE OFF CACHE BOOL "Build Batch POSTGRESQL" FORCE)
SET(TRANTOR_USE_TLS "" CACHE STRING "TLS provider for trantor. Valid options: 'openssl', 'botan' or ''" FORCE)
SET(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "Use header-only spdlog" FORCE)
SET(USE_SPDLOG ON CACHE BOOL "Use spdlog as logger" FORCE)
SET(SPDLOG_INSTALL ON CACHE BOOL "generate install files" FORCE)
SET(JSONCPP_WITH_CMAKE_PACKAGE ON CACHE BOOL "Generate and install cmake package files" FORCE)

#fetch dependecies
include("PatchFile")
include("FetchZlib")  
include("FetchLive")
include("FetchFmt")
include("FetchSpdlog")
include("FetchJsoncpp")  
include("FetchDrogon")

add_subdirectory(3rdparty/fmt)
add_subdirectory(3rdparty/spdlog)
add_subdirectory(3rdparty/jsoncpp)
#hack for drogon see jsoncpp
set(spdlog_DIR ${CMAKE_BINARY_DIR}/3rdparty/spdlog CACHE INTERNAL "")
set(fmt_DIR ${CMAKE_BINARY_DIR}/3rdparty/fmt CACHE INTERNAL "")
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "${JSONCPP_INCLUDE_DIRS}" "${CMAKE_BINARY_DIR}/3rdparty/jsoncpp/include/json")   
add_subdirectory(3rdparty/drogon)

#stupid hack for using spdlog without fmt-library in drogon
target_compile_definitions(trantor PUBLIC TRANTOR_SPDLOG_SUPPORT)
target_compile_definitions(drogon PUBLIC DROGON_SPDLOG_SUPPORT)

# basic project
add_subdirectory(src)

message("")
message("  CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("")
message("    Live555_DIR = ${Live555_DIR}")
message("    Drogon_DIR = ${Drogon_DIR}")
message("    Jsoncpp_DIR = ${Jsoncpp_DIR}")
message("    Spdlog_DIR = ${Spdlog_DIR}")
message("")