cmake_minimum_required(VERSION 3.10...3.21)
project(camera-v4l2-server)

if (MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

########################
# camera library
########################
aux_source_directory("camera" CAMERA_SRC_FILES)
add_library (device SHARED ${CAMERA_SRC_FILES})
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
add_library (rtspserver SHARED ${LIVESOURCE} ${RTSP_SRC_FILES})
target_include_directories(rtspserver PUBLIC ${PROJECT_SOURCE_DIR}/rtsp)
target_include_directories(rtspserver PUBLIC ${Live555_INCLUDE_DIRS})	
target_compile_definitions(rtspserver PUBLIC ${Live555_CFLAGS})
target_link_libraries(rtspserver Ws2_32 spdlog device)

#####################
# web library
#####################

aux_source_directory("web" WEB_SRC_FILES)
add_library (webserver SHARED ${WEB_SRC_FILES})
target_include_directories(webserver PUBLIC ${PROJECT_SOURCE_DIR}/web)	
target_link_libraries(webserver device drogon spdlog)

#####################
# main executable
#####################
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries (${PROJECT_NAME} device rtspserver webserver)

IF(WIN32)  
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(Zlib_SUFFIX_LIB "d")
  endif()
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        
      COMMAND ${CMAKE_COMMAND} -E copy_if_different  
          "${ZLIB_ROOT}/bin/zlib${Zlib_SUFFIX_LIB}.dll"      # <--this is in-file
          $<TARGET_FILE_DIR:camera-v4l2-server>)                 # <--this is out-file path
ENDIF()