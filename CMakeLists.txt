cmake_minimum_required(VERSION 3.10...3.27 FATAL_ERROR)

#####################################################################################################
# PROJECT INFORMATION
#####################################################################################################

project (camera_v4l_rtsp  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif() 

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")
include(FetchContent)
include(CMakePrintHelpers)
include("PatchFile")

SET(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)

####################
# Live555
####################
include("FetchLive")

##################################
## Jsoncpp: try find or downloaded it
##################################
find_package(Jsoncpp QUIET)
if(NOT Jsoncpp_FOUND)
  set(BUILD_WTH_JSONCPP ON)
  FetchContent_Declare(jsoncpp
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
    GIT_TAG        1.9.4
    OVERRIDE_FIND_PACKAGE)

  SET(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "Build unittest" FORCE)
  SET(JSONCPP_WITH_TESTS OFF CACHE BOOL "Build tests" FORCE)
  SET(JSONCPP_WITH_PKGCONFIG_SUPPORT ON CACHE BOOL "Generate and install .pc files" FORCE)
  SET(JSONCPP_WITH_CMAKE_PACKAGE ON CACHE BOOL "Generate and install cmake package files" FORCE)
  SET(BUILD_STATIC_LIBS ON)
  SET(BUILD_OBJECT_LIBS ON)
  SET(BUILD_SHARED_LIBS OFF)
  FetchContent_MakeAvailable(jsoncpp)
  # stupid hack for include jsoncpp include
  if (WIN32)
    include_directories("${jsoncpp_SOURCE_DIR}/include")
  endif()
endif()


##################################
## Zlib: try find or downloaded it
##################################
#find_package(ZLIB QUIET)
#if(NOT ZLIB_FOUND)
  include("FetchZlib")
#endif()

################################
# FMT
################################
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        10.2.1
  OVERRIDE_FIND_PACKAGE)

if(WIN32)
  set(FMT_USE_WINDOWS_H ON)
endif()
FetchContent_MakeAvailable(fmt)

################################
# SPDLOG
################################
FetchContent_Declare(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.14.1
  OVERRIDE_FIND_PACKAGE)

if(WIN32)
  set(SPDLOG_BUILD_SHARED ON)
endif()
set(SPDLOG_INSTALL ON)
set(SPDLOG_COMPILED_LIB ON)
set(SPDLOG_BUILD_EXAMPLES OFF)  # Example: Disable building examples
set(SPDLOG_BUILD_TESTS OFF)     # Example: Disable building tests
set(SPDLOG_FMT_EXTERNAL ON)
set(spdlog_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(spdlog)

################################
# Drogon
################################
FetchContent_Declare(drogon
  GIT_REPOSITORY https://github.com/drogonframework/drogon.git
  GIT_TAG        v1.9.6
  OVERRIDE_FIND_PACKAGE)

set(BUILD_SHARED_LIBS ON)
set(BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
set(BUILD_ORM OFF CACHE BOOL "Build with ORM" FORCE)
set(USE_SUBMODULE ON CACHE BOOL "Build submodules" FORCE)
set(USE_SPDLOG ON CACHE BOOL "Build with use spdlog" FORCE)
set(BUILD_POSTGRESQL OFF CACHE BOOL "Build with postgres" FORCE)
set(BUILD_MYSQL OFF CACHE BOOL "Build with mysql" FORCE)
set(BUILD_SQLITE OFF CACHE BOOL "Build witg sqlite" FORCE)
set(BUILD_REDIS OFF CACHE BOOL "Build with redis" FORCE)
set(BUILD_CTL OFF CACHE BOOL "Build drogon ctl" FORCE)
FetchContent_MakeAvailable(drogon)

#stupid hack for force using jsoncpp as static
IF(BUILD_WTH_JSONCPP)
 execute_process(COMMAND ${PATCH} -d ${drogon_SOURCE_DIR} -f -p 1 -i ${CMAKE_SOURCE_DIR}/patches/003_drogon_jsoncpp_static_win32
 TIMEOUT 15
 COMMAND_ECHO STDOUT
 RESULT_VARIABLE ret)
 message("===== patched JSONCPP")
endif()

# basic project
add_subdirectory(src)

message("")
message("  CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("")