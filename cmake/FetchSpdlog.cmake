set(Spdlog_URL "https://github.com/gabime/spdlog/archive/refs/tags/v1.13.0.zip")
set(Spdlog_file "spdlog-1.13.0")
set(Spdlog_DIR  "${CMAKE_SOURCE_DIR}/3rdparty/spdlog" CACHE INTERNAL "")    

# function for download Spdlog
if (NOT EXISTS ${Spdlog_DIR})
    message(STATUS "Spdlog not found. downloading ... ")
    file (DOWNLOAD ${Spdlog_URL} ${CMAKE_SOURCE_DIR}/${Spdlog_file}.zip )
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar xvf ${CMAKE_SOURCE_DIR}/${Spdlog_file}.zip RESULT_VARIABLE result OUTPUT_QUIET)
    if(NOT result STREQUAL "0")
        message(FATAL_ERROR "Fetching and compiling Spdlog failed!")
    endif()        
    file(REMOVE ${CMAKE_SOURCE_DIR}/${Spdlog_file}.zip)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "${Spdlog_file}" "${Spdlog_DIR}")        
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${Spdlog_file}")
endif()

# set variable
set(SPDLOG_INCLUDE_DIRS ${Spdlog_DIR}/include CACHE INTERNAL "")
set(SPDLOG_LIBRARIES spdlog CACHE INTERNAL "spdlog")
