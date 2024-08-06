

set(ffmpeg_ext  "zip")
set(ffmpeg_url  "https://github.com/BtbN/FFmpeg-Builds/releases/download/autobuild-2023-11-30-12-55/ffmpeg-N-112876-ga30adf9f96-win64-gpl-shared.${ffmpeg_ext}")
set(ffmpeg_file "ffmpeg-N-112876-ga30adf9f96-win64-gpl-shared")
set(FFMPEG_DIR  "${CMAKE_BINARY_DIR}/_deps/ffmpeg-install"  CACHE INTERNAL "")

#function for download ffmpeg
function(download_ffmpeg url file)
    file(DOWNLOAD "${url}" "${CMAKE_SOURCE_DIR}/${file}.${ffmpeg_ext}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf "${CMAKE_SOURCE_DIR}/${file}.${ffmpeg_ext}")
    file(REMOVE "${CMAKE_SOURCE_DIR}/${file}.${ffmpeg_ext}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "${file}" "${FFMPEG_DIR}")        
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${file}")
endfunction()

#function for check by PkgConfig
function(check_ffmpeg)
    find_package(PkgConfig QUIET)
    # try download pkg-config
    if(NOT PkgConfig_FOUND)        
        file(DOWNLOAD "https://gist.github.com/Zerpico/a341361ad13b88822916321a4b1cea1f/raw/6a56be8d4821b8b8e79008c338192c1ce63285da/pkg-config.zip" "${CMAKE_SOURCE_DIR}/pkg-config.zip")
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf "${CMAKE_SOURCE_DIR}/pkg-config.zip")
        file(REMOVE "${CMAKE_SOURCE_DIR}/pkg-config.zip")
        execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "pkg-config" "${CMAKE_BINARY_DIR}/_deps/pkg-config")        
        execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "pkg-config")
        set(PKG_CONFIG_EXECUTABLE "${CMAKE_BINARY_DIR}/_deps/pkg-config/pkg-config.exe")
        find_package(PkgConfig)
    endif()
    pkg_check_modules(FFMPEG IMPORTED_TARGET 
        libavdevice
        libavformat
        libavcodec
        libswresample
        libswscale
        libavutil
#        libpostproc
#        libavfilter
    )       
endfunction()

function(find_library_by_name lib libpath)
    FIND_LIBRARY(FFMPEG_${lib}_LIBRARY lib  ${libpath})
endfunction()

function(custom_find_ffmpeg)
    FIND_PATH(FFMPEG_INCLUDE_DIR avformat.h
        PATHS
        ${FFMPEG_DIR}/include        
        PATH_SUFFIXES ffmpeg
        DOC "Location of FFMPEG Headers"
    )

    # if ffmpeg headers are separated to each of libavformat, libavcodec etc..
    IF( NOT FFMPEG_INCLUDE_DIR )
		FIND_PATH(FFMPEG_INCLUDE_DIR libavformat/avformat.h
			PATHS
			${FFMPEG_DIR}/include
			PATH_SUFFIXES ffmpeg
			DOC "Location of FFMPEG Headers"
		)
    ENDIF( NOT FFMPEG_INCLUDE_DIR )

    get_filename_component(FFMPEG_INCLUDE_DIR ${FFMPEG_INCLUDE_DIR} ABSOLUTE)

    FIND_PATH(FFMPEG_LIBRARY_DIRS avcodec.lib libavcodec.dll.a
        PATHS
        ${FFMPEG_DIR}/lib
        PATH_SUFFIXES ffmpeg
        DOC "Location of FFMPEG Headers"
    )

    # check if variables is not empty
    if (NOT ${FFMPEG_INCLUDE_DIR} STREQUAL "" AND NOT ${FFMPEG_LIBRARY_DIRS} STREQUAL "")
        SET(FFMPEG_FOUND ON CACHE BOOL "ffmpeg is found")
    ENDIF()

    #IF(NOT "${FFMPEG_FOUND}")        
    #    message(SEND_ERROR "Can't find FFmpeg libraries") 
    #ENDIF()
endfunction()


check_ffmpeg()
IF (NOT "${FFMPEG_FOUND}")
    IF (WIN32)
        #try download ffmpeg builds       
        IF(NOT EXISTS "${FFMPEG_DIR}") 
            message("FFmpeg not found. Downloading ...")
            download_ffmpeg(${ffmpeg_url} ${ffmpeg_file})      
        ENDIF()
    
        #try find libs by PkgConfig
        set(ENV{PKG_CONFIG_PATH} "${FFMPEG_DIR}/lib/pkgconfig")
        check_ffmpeg()
        
        IF(NOT "${FFMPEG_FOUND}")
            custom_find_ffmpeg()        
        ENDIF()
    ELSE()
        message(SEND_ERROR "Can't find FFmpeg libraries")        
    ENDIF()
ENDIF()