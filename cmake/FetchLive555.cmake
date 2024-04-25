set(LIVE555_URL "https://download.videolan.org/contrib/live555/live.2023.01.19.tar.gz")
set(LIVE555_file "live")
set(LIVE555_CFLAGS -DBSD=1 -DSOCKLEN_T=socklen_t -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE=1 -DNO_SSTREAM=1 -DALLOW_RTSP_SERVER_PORT_REUSE=1 -DNO_GETIFADDRS=1 -DNO_OPENSSL=1 CACHE STRING "live555 CFGLAGS")
set(LIVE555_DIR  "${CMAKE_SOURCE_DIR}/3rdparty/live555" CACHE INTERNAL "")

if(CMAKE_CXX_STANDARD LESS 20)
    list(APPEND Live555_CFLAGS -DNO_STD_LIB)
endif()

# function for download live555
if (NOT EXISTS ${LIVE555_DIR})
    message(STATUS "live555 not found. downloading ... ")
    file (DOWNLOAD ${LIVE555_URL} ${CMAKE_SOURCE_DIR}/${LIVE555_file}.tar.gz )
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar xvf ${CMAKE_SOURCE_DIR}/${LIVE555_file}.tar.gz RESULT_VARIABLE unpack_result OUTPUT_QUIET)
    if(NOT unpack_result STREQUAL "0")
        message(FATAL_ERROR "Fetching and compiling live555 failed!")
    endif()
    file(REMOVE ${CMAKE_SOURCE_DIR}/${LIVE555_file}.tar.gz)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "${LIVE555_file}" "${LIVE555_DIR}" RESULT_VARIABLE copy_result)
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${LIVE555_file}")

    if(NOT copy_result STREQUAL "0")
       execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${LIVE555_DIR}")
       message(FATAL_ERROR "Copy directory live555 failed!")
    endif()
endif()  

set(LIVE555_FOUND ON CACHE BOOL "live55 is found" )   

#sources live555
set(LIVE555_INCLUDE_DIRS ${LIVE555_DIR}/groupsock/include ${LIVE555_DIR}/liveMedia/include ${LIVE555_DIR}/UsageEnvironment/include ${LIVE555_DIR}/BasicUsageEnvironment/include CACHE INTERNAL "")
FILE(GLOB LIVESOURCE ${LIVE555_DIR}/groupsock/*.c* ${LIVE555_DIR}/liveMedia/*.c* ${LIVE555_DIR}/UsageEnvironment/*.c* ${LIVE555_DIR}/BasicUsageEnvironment/*.c* )

#target live555
add_library(live555 STATIC ${LIVESOURCE})	
add_library(live555::live555 ALIAS live555) #set alias
target_include_directories(live555 PUBLIC inc ${LIVE555_INCLUDE_DIRS})
target_compile_definitions(live555 PUBLIC ${LIVE555_CFLAGS})

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()
