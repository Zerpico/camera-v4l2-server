set(Fmt_URL "https://github.com/fmtlib/fmt/archive/refs/tags/10.2.1.zip")
set(Fmt_file "fmt-10.2.1")
set(Fmt_DIR  "${CMAKE_SOURCE_DIR}/3rdparty/fmt" CACHE INTERNAL "")    

# function for download Fmt
if (NOT EXISTS ${Fmt_DIR})
    message(STATUS "Fmt not found. downloading ... ")
    file (DOWNLOAD ${Fmt_URL} ${CMAKE_SOURCE_DIR}/${Fmt_file}.zip )
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar xvf ${CMAKE_SOURCE_DIR}/${Fmt_file}.zip RESULT_VARIABLE result OUTPUT_QUIET)
    if(NOT result STREQUAL "0")
        message(FATAL_ERROR "Fetching and compiling Fmt failed!")
    endif()        
    file(REMOVE ${CMAKE_SOURCE_DIR}/${Fmt_file}.zip)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "${Fmt_file}" "${Fmt_DIR}")        
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${Fmt_file}")
endif()

# set variable
set(Fmt_INCLUDE_DIRS ${Fmt_DIR}/include CACHE INTERNAL "")
set(Fmt_LIBRARIES Fmt CACHE INTERNAL "Fmt")
