cmake_minimum_required(VERSION 3.10...3.21)
project(camera-v4l2-server)

# camera library
aux_source_directory("camera" CAMERA_SRC_FILES)
add_library (camera_device STATIC ${CAMERA_SRC_FILES})
target_include_directories(camera_device PUBLIC ${PROJECT_SOURCE_DIR}/camera)	

# rtsp library
aux_source_directory("rtsp" RTSP_SRC_FILES)
add_library (rtspserver STATIC ${RTSP_SRC_FILES})
target_include_directories(rtspserver PUBLIC ${PROJECT_SOURCE_DIR}/rtsp)
target_include_directories(rtspserver PUBLIC ${Live555_INCLUDE_DIRS})	
target_link_libraries(rtspserver camera_device live555)

# web library
aux_source_directory("web" WEB_SRC_FILES)
add_library (webserver STATIC ${WEB_SRC_FILES})
target_include_directories(webserver PUBLIC ${PROJECT_SOURCE_DIR}/web)	
target_link_libraries(webserver camera_device drogon)

# main executable
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries (${PROJECT_NAME} camera_device rtspserver webserver)
#TARGET_LINK_LIBRARIES(test_app drogon live555)