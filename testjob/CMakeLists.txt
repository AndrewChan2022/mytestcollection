cmake_minimum_required(VERSION 3.1)


#add_definitions(-DEXCLUDE_LOG_MODULE)
#add_compile_definitions(MG_ENABLE_OPENSSL=1)

# cpp version (be different for other platform, TODO).
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(APP_NAME mytest)
project(${APP_NAME})

################## include
include_directories(./src)
include_directories(SYSTEM ./thirdparty/include)

################## exe
# add all source files
add_executable(${APP_NAME})

################## src

file(GLOB APP_HEADER_FILES
    ./src/*.h ./src/*.hpp
)
file(GLOB APP_SOURCE_FILES
    ./src/*.cpp
)
SET (ALL_SRCS
    ${APP_HEADER_FILES}
    ${APP_SOURCE_FILES}
)
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${ALL_SRCS})

target_sources(${APP_NAME} PRIVATE
    ${ALL_SRCS}
)

################## link

message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

if(APPLE)

    #set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lib/macos/assimp)
    #find_library(ASSIMP_LIB NAMES libassimp.dylib PATHS ${LIBS_DIR} NO_DEFAULT_PATH)
    #target_link_libraries(${APP_NAME} PRIVATE  mylib)

else()

    #set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lib/win/assimp)
    #find_library(ASSIMP_LIB NAMES assimp-vc143-mtd.lib PATHS ${LIBS_DIR} NO_DEFAULT_PATH)
    #target_link_libraries(${APP_NAME} PRIVATE mylib)

    #add_custom_command(TARGET ${APP_NAME} POST_BUILD
    #    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    #    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lib/win/assimp/assimp-vc143-mtd.dll
    #    $<TARGET_FILE_DIR:${APP_NAME}> 
    #)

endif()
