
#only on Windows

if (WIN32)

  set(Zlib_URL "https://github.com/madler/zlib/archive/refs/tags/v1.3.1.zip")
  set(Zlib_file "zlib-1.3.1")
  set(Zlib_DIR  "${CMAKE_SOURCE_DIR}/3rdparty/zlib" CACHE INTERNAL "")    

  # function for download Zlib
  if (NOT EXISTS ${Zlib_DIR})
      message(STATUS "Zlib not found. downloading ... ")
      file (DOWNLOAD ${Zlib_URL} ${CMAKE_SOURCE_DIR}/${Zlib_file}.zip )
      EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar xvf ${CMAKE_SOURCE_DIR}/${Zlib_file}.zip RESULT_VARIABLE result OUTPUT_QUIET)
      if(NOT result STREQUAL "0")
          message(FATAL_ERROR "Fetching and compiling Zlib failed!")
      endif()        
      file(REMOVE ${CMAKE_SOURCE_DIR}/${Zlib_file}.zip)
      execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "${Zlib_file}" "${Zlib_DIR}")        
      execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${Zlib_file}")
  endif()  

  mark_as_advanced(ZLIB_FORCE_STATIC)
  set(ZLIB_BUILD_DIR ${OUTPUT_BUILD_DIR}/zlib-build)
  set(ZLIB_INSTALL_DIR ${OUTPUT_BUILD_DIR}/zlib-install)
  set(ZLIB_SRC_FOLDER_NAME zlib-src)
  set(ZLIB_SRC_DIR ${OUTPUT_BUILD_DIR}/${ZLIB_SRC_FOLDER_NAME})
  set(ZLIB_BUILD_EXAMPLES OFF)

  if (CMAKE_BUILD_TYPE EQUAL "DEBUG" OR CMAKE_BUILD_TYPE EQUAL "Debug")
    set(Zlib_SUFFIX_LIB "d")
    message("Zlib_SUFFIX_LIB = ${Zlib_SUFFIX_LIB}")
  endif()
  
  if(NOT EXISTS ${CMAKE_BINARY_DIR}/3rdparty/zlib-install)    
    message(STATUS "Pre-build Zlib library ... ")

    #configure
    execute_process(COMMAND ${CMAKE_COMMAND}
      -S ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib
      -B ${CMAKE_BINARY_DIR}/3rdparty/zlib-build
      -G ${CMAKE_GENERATOR}
      -D CMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/3rdparty/zlib-install OUTPUT_QUIET)

    #build
    execute_process(COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/3rdparty/zlib-build OUTPUT_QUIET)
    #install
    execute_process(COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/3rdparty/zlib-build --target install OUTPUT_QUIET)

    execute_process(COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_BINARY_DIR}/3rdparty/zlib-install/lib/zlib${Zlib_SUFFIX_LIB}.lib ${CMAKE_BINARY_DIR}/3rdparty/zlib-install/bin/zlib${Zlib_SUFFIX_LIB}.dll)
    set(ZLIB_ROOT ${CMAKE_BINARY_DIR}/3rdparty/zlib-install)
  endif()
endif()