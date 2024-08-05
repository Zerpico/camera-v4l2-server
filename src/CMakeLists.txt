cmake_minimum_required(VERSION 3.10...3.21)
project(camera-v4l2-server)

if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
if (WIN32)
  set(BUILDLIB_TYPE SHARED)
else()
  set(BUILDLIB_TYPE STATIC)
endif()

########################
# camera library
########################
aux_source_directory("camera" CAMERA_SRC_FILES)
add_library (device ${BUILDLIB_TYPE} ${CAMERA_SRC_FILES})
target_include_directories(device PUBLIC ${PROJECT_SOURCE_DIR}/camera)	

#####################
# rtsp library + Live555
#####################
#  set sources live555
set(Live555_INCLUDE_DIRS 
    ${Live555_DIR}/groupsock/include 
    ${Live555_DIR}/liveMedia/include 
    ${Live555_DIR}/UsageEnvironment/include 
    ${Live555_DIR}/BasicUsageEnvironment/include CACHE INTERNAL "")
FILE(GLOB LIVESOURCE 
    "${Live555_DIR}/groupsock/*.c*"
    "${Live555_DIR}/liveMedia/*.c*"
    "${Live555_DIR}/UsageEnvironment/*.c*" 
    "${Live555_DIR}/BasicUsageEnvironment/*.c*" )
list(REMOVE_ITEM LIVESOURCE "${Live555_DIR}/liveMedia/ADTSAudioStreamDiscreteFramer.cpp")

#target_link_libraries (${PROJECT_NAME} liblive555 ${LIVE555_LIBRARIES}) 
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)    
endif()

aux_source_directory("rtsp" RTSP_SRC_FILES)
add_library (rtspserver ${BUILDLIB_TYPE} ${LIVESOURCE} ${RTSP_SRC_FILES})
target_include_directories(rtspserver PUBLIC ${PROJECT_SOURCE_DIR}/rtsp)
target_include_directories(rtspserver PUBLIC ${Live555_INCLUDE_DIRS})	
target_compile_definitions(rtspserver PUBLIC ${Live555_CFLAGS})
if(WIN32)
  set(WINSOCK Ws2_32)
endif()
target_link_libraries(rtspserver ${WINSOCK} spdlog device)

#####################
# web library
#####################

aux_source_directory("web" WEB_SRC_FILES)
add_library (webserver ${BUILDLIB_TYPE} ${WEB_SRC_FILES})
target_include_directories(webserver PUBLIC ${PROJECT_SOURCE_DIR}/web)	
target_link_libraries(webserver device drogon spdlog)

#####################
# main executable
#####################
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries (${PROJECT_NAME} device rtspserver webserver)
