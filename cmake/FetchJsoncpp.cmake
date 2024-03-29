set(Jsoncpp_URL "https://github.com/open-source-parsers/jsoncpp/archive/refs/tags/1.9.4.zip")
set(Jsoncpp_file "jsoncpp-1.9.4")
set(Jsoncpp_DIR  "${CMAKE_SOURCE_DIR}/3rdparty/jsoncpp" CACHE INTERNAL "")    

# function for download Jsoncpp
if (NOT EXISTS ${Jsoncpp_DIR})
    message(STATUS "Jsoncpp not found. downloading ... ")
    file (DOWNLOAD ${Jsoncpp_URL} ${CMAKE_SOURCE_DIR}/${Jsoncpp_file}.zip )
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar xvf ${CMAKE_SOURCE_DIR}/${Jsoncpp_file}.zip RESULT_VARIABLE result OUTPUT_QUIET)
    if(NOT result STREQUAL "0")
        message(FATAL_ERROR "Fetching and compiling Jsoncpp failed!")
    endif()        
    file(REMOVE ${CMAKE_SOURCE_DIR}/${Jsoncpp_file}.zip)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "${Jsoncpp_file}" "${Jsoncpp_DIR}")        
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${Jsoncpp_file}")
endif()  

#set(Jsoncpp_FOUND ON CACHE BOOL "Jsoncpp is found" )   
#get_target_property(JSONCPP_INC_DIRS jsoncpp_static INCLUDE_DIRECTORIES)
#list(POP_FRONT JSONCPP_INC_DIR JSONCPP_INCLUDE_DIRS)
#set(JSONCPP_LIBRARIES jsoncpp_static)

set(JSONCPP_INCLUDE_DIRS ${Jsoncpp_DIR}/include CACHE INTERNAL "")
set(JSONCPP_LIBRARIES jsoncpp_static CACHE INTERNAL "jsoncpp_static")