cmake_minimum_required(VERSION 3.10...3.21)

#####################################################################################################
# PROJECT INFORMATION
#####################################################################################################

project (camera_v4l_rtsp  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option (BUILD_FRONT "Build Web UI" OFF)

if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
include(CPM)
include(CMakePrintHelpers)
include(PatchFile)

SET(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)

####################
# Live555
####################
include("FetchLive")

####################
# FFmpeg
####################
include("FetchFFmpeg")

##################################
## Jsoncpp: try find or downloaded it
##################################
CPMAddPackage(
  NAME Jsoncpp
  GITHUB_REPOSITORY open-source-parsers/jsoncpp
  GIT_TAG 1.9.4
  OPTIONS
      "JSONCPP_WITH_POST_BUILD_UNITTEST OFF"
      "JSONCPP_WITH_TESTS OFF"
      "JSONCPP_WITH_PKGCONFIG_SUPPORT ON"
      "JSONCPP_WITH_CMAKE_PACKAGE ON"
      "BUILD_STATIC_LIBS ON"
      "BUILD_SHARED_LIBS OFF"
)
include_directories("${jsoncpp_SOURCE_DIR}/include")

include("FetchZlib")

#################################
## FMT
#################################
if(WIN32)
  set(FMTW_FLAG ON)
endif()
CPMAddPackage(
  NAME fmt
  GITHUB_REPOSITORY fmtlib/fmt
  GIT_TAG 11.1.3
  OPTIONS
      "FMT_USE_WINDOWS_H ${FMTW_FLAG}"  
)

#################################
## SPDLOG
#################################
if(WIN32)
  set(SPDLOG_SHARED_FLAG ON)
else()  
  set(SPDLOG_SHARED_FLAG OFF)
endif()
CPMAddPackage(
  NAME spdlog
  GITHUB_REPOSITORY gabime/spdlog
  GIT_TAG v1.15.1
  OPTIONS  
    "SPDLOG_BUILD_SHARED ${SPDLOG_SHARED_FLAG}"
    "SPDLOG_INSTALL ON"
    "SPDLOG_COMPILED_LIB ON"
    "SPDLOG_BUILD_EXAMPLES OFF"
    "SPDLOG_BUILD_TESTS OFF"
    "SPDLOG_FMT_EXTERNAL ON"    
)
include_directories("${spdlog_SOURCE_DIR}/include")
#################################
## Drogon
#################################
CPMAddPackage(
  NAME drogon
  GITHUB_REPOSITORY drogonframework/drogon
  GIT_TAG        v1.9.9
  GIT_SHALLOW TRUE
  DOWNLOAD_ONLY YES
)

if(WIN32)
  set(BUILD_SHARED_LIBS ON CACHE INTERNAL ON)    
endif()
set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
set(BUILD_ORM OFF CACHE BOOL "Build with ORM" FORCE)
set(USE_SUBMODULE ON CACHE BOOL "Build submodules" FORCE)
set(USE_SPDLOG ON CACHE BOOL "Build with use spdlog" FORCE)
set(BUILD_POSTGRESQL OFF CACHE BOOL "Build with postgres" FORCE)
set(BUILD_MYSQL OFF CACHE BOOL "Build with mysql" FORCE)
set(BUILD_SQLITE OFF CACHE BOOL "Build witg sqlite" FORCE)
set(BUILD_REDIS OFF CACHE BOOL "Build with redis" FORCE)
set(BUILD_CTL OFF CACHE BOOL "Build drogon ctl" FORCE)
set(spdlog_DIR ${spdlog_BUILD_DIR} CACHE STRING "Build drogon with spdlog" FORCE)

#stupid hack for force using jsoncpp as static
  execute_process(COMMAND ${PATCH} -d ${drogon_SOURCE_DIR} -f -p 1 -i ${CMAKE_SOURCE_DIR}/patches/003_drogon_jsoncpp_static_win32
  TIMEOUT 15
  COMMAND_ECHO STDOUT
  RESULT_VARIABLE ret)

add_subdirectory(${drogon_SOURCE_DIR} ${drogon_BUILD_DIR})
target_compile_definitions(drogon PUBLIC DROGON_SPDLOG_SUPPORT)
target_compile_definitions(trantor PUBLIC TRANTOR_SPDLOG_SUPPORT)
add_dependencies(drogon spdlog)

## basic project
add_subdirectory(src)

##########################
# frontend
##########################
if (BUILD_FRONT)
  add_custom_target("frontend" COMMAND npm install -c ${CMAKE_CURRENT_SOURCE_DIR}/frontend && npm run build --prefix ${CMAKE_CURRENT_SOURCE_DIR}/frontend)
endif()

message("")
message("  CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("")
