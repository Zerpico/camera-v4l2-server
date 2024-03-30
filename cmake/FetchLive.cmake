set(Live555_URL "https://github.com/rgaufman/live555/archive/refs/heads/master.zip")
set(Live555_file "live555-master")
set(Live555_CFLAGS -DBSD=1 -DSOCKLEN_T=socklen_t -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE=1 -DNO_SSTREAM=1 -DALLOW_RTSP_SERVER_PORT_REUSE=1 -DNO_GETIFADDRS=1 -DNO_OPENSSL=1 CACHE STRING "live555 CFGLAGS")
set(Live555_DIR  "${CMAKE_SOURCE_DIR}/3rdparty/live555" CACHE INTERNAL "")    

if(CMAKE_CXX_STANDARD LESS 20)
    set(Live555_CFLAGS ${LIVE555CFLAGS} -DNO_STD_LIB)
endif()

# function for download Live555
if (NOT EXISTS ${Live555_DIR})
    message(STATUS "Live555 not found. downloading ... ")
    file (DOWNLOAD ${Live555_URL} ${CMAKE_SOURCE_DIR}/${Live555_file}.zip )
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar xvf ${CMAKE_SOURCE_DIR}/${Live555_file}.zip RESULT_VARIABLE result OUTPUT_QUIET)
    if(NOT result STREQUAL "0")
        message(FATAL_ERROR "Fetching and compiling Live555 failed!")
    endif()        
    file(REMOVE ${CMAKE_SOURCE_DIR}/${Live555_file}.zip)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "${Live555_file}" "${Live555_DIR}")        
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${Live555_file}")
endif()  

#sources live555
set(Live555_INCLUDE_DIRS 
    ${Live555_DIR}/groupsock/include 
    ${Live555_DIR}/liveMedia/include 
    ${Live555_DIR}/UsageEnvironment/include 
    ${Live555_DIR}/BasicUsageEnvironment/include CACHE INTERNAL "")
FILE(GLOB LIVESOURCE 
    ${Live555_DIR}/groupsock/*.c
    ${Live555_DIR}/liveMedia/*.c* 
    ${Live555_DIR}/UsageEnvironment/*.c* 
    ${Live555_DIR}/BasicUsageEnvironment/*.c* )

#target live555
add_library(live555 STATIC ${LIVESOURCE})	
add_library(live555::live555 ALIAS live555) #set alias
target_include_directories(live555 PUBLIC inc ${Live555_INCLUDE_DIRS})
target_compile_definitions(live555 PUBLIC ${Live555_CFLAGS})

#target_link_libraries (${PROJECT_NAME} liblive555 ${LIVE555_LIBRARIES}) 
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()