set(Drogon_URL "https://github.com/drogonframework/drogon/archive/refs/tags/v1.9.3.zip")
set(Drogon_file "drogon-1.9.3")
set(Drogon_DIR  "${CMAKE_SOURCE_DIR}/3rdparty/drogon" CACHE INTERNAL "")    

# function for download Drogon
if (NOT EXISTS ${Drogon_DIR})
    message(STATUS "Drogon not found. downloading ... ")
    file (DOWNLOAD ${Drogon_URL} ${CMAKE_SOURCE_DIR}/${Drogon_file}.zip )
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar xvf ${CMAKE_SOURCE_DIR}/${Drogon_file}.zip RESULT_VARIABLE result OUTPUT_QUIET)
    if(NOT result STREQUAL "0")
        message(FATAL_ERROR "Fetching and compiling Drogon failed!")
    endif()        
    file(REMOVE ${CMAKE_SOURCE_DIR}/${Drogon_file}.zip)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "${Drogon_file}" "${Drogon_DIR}")        
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${Drogon_file}")

    #patch CMakeLists.txt for comment install target (conflicts with spdlog)
    execute_process(COMMAND ${PATCH} -d ${CMAKE_SOURCE_DIR}/3rdparty -f -p 1 -i ${CMAKE_SOURCE_DIR}/patches/001_drogon_disable_install
        TIMEOUT 15
        COMMAND_ECHO STDOUT
        RESULT_VARIABLE ret)
endif()  

#check and download trantor for drogon as dependency
file(GLOB RESULT "${Drogon_DIR}/trantor/CMakeLists.txt")
list(LENGTH RESULT RES_LEN)
if(RES_LEN EQUAL 0)
    
    message(STATUS "Trantor not found. downloading ... ")
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${Drogon_DIR}/trantor")
    file (DOWNLOAD https://github.com/an-tao/trantor/archive/refs/tags/v1.5.17.zip  "${CMAKE_SOURCE_DIR}/trantor-1.5.17.zip")
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E tar xvf ${CMAKE_SOURCE_DIR}/trantor-1.5.17.zip RESULT_VARIABLE result OUTPUT_QUIET)
    if(NOT result STREQUAL "0")
        message(FATAL_ERROR "Fetching and compiling Trantor failed!")
    endif()   
    file(REMOVE ${CMAKE_SOURCE_DIR}/trantor-1.5.17.zip)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "trantor-1.5.17" "${Drogon_DIR}/trantor")
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "trantor-1.5.17")    

    #patch CMakeLists.txt for comment install target (conflicts with spdlog)
    execute_process(COMMAND ${PATCH} -d ${CMAKE_SOURCE_DIR}/3rdparty -f -p 1 -i ${CMAKE_SOURCE_DIR}/patches/002_trantor_disable_install
        TIMEOUT 15
        COMMAND_ECHO STDOUT
        RESULT_VARIABLE ret)
endif()