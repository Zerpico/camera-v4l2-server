project(camera-v4l2-server LANGUAGES CXX)

# camera library
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/camera_device" CAMERA_SRC_FILES)
add_library(camera_device STATIC ${CAMERA_SRC_FILES})
add_library(sub::cameralib ALIAS camera_device) #set alias
target_include_directories(camera_device PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/camera_device")	

# rtsp-server library
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/rtsp_server" RTSP_SRC_FILES)
add_library(rtsp_server STATIC ${RTSP_SRC_FILES})
add_library(sub::rtsplib ALIAS rtsp_server) #set alias
target_include_directories(rtsp_server PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/rtsp_server")
target_link_libraries(rtsp_server sub::cameralib live555::live555 spdlog)
IF(WIN32)
 target_link_libraries(rtsp_server ws2_32)
 add_definitions(-D__STDC_CONSTANT_MACROS)
ENDIF()

# web-server library
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/web_server" WEB_SRC_FILES)
IF(WIN32)
  add_library(web_server SHARED ${WEB_SRC_FILES})
ELSE()
  add_library(web_server STATIC ${WEB_SRC_FILES})
ENDIF()
add_library(sub::weblib ALIAS web_server) #set alias
target_include_directories(web_server PUBLIC ${PROJECT_SOURCE_DIR}/web_server)
target_link_libraries(web_server sub::cameralib drogon)
IF(WIN32)
  target_compile_definitions(web_server PUBLIC MAKEDLL)
ENDIF()  

# main executable
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} sub::cameralib spdlog sub::weblib sub::rtsplib)

# Добавляем флаги для правильной линковки статичной библиотеки в динамическую для Unix систем
if (NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic")
  set(CMAKE_SHARED_LINKER_FLAGS "-Wl,-Bsymbolic")
endif()

IF(WIN32)  
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(Zlib_SUFFIX_LIB "d")
  endif()
  add_custom_command(TARGET camera-v4l2-server POST_BUILD        
      COMMAND ${CMAKE_COMMAND} -E copy_if_different  
          "${ZLIB_ROOT}/bin/zlib${Zlib_SUFFIX_LIB}.dll"      # <--this is in-file
          $<TARGET_FILE_DIR:camera-v4l2-server>)                 # <--this is out-file path
ENDIF()