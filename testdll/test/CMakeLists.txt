cmake_minimum_required(VERSION 3.1)

#add_definitions(-DEXCLUDE_LOG_MODULE)
#add_compile_definitions(MG_ENABLE_OPENSSL=1)

# cpp version (be different for other platform, TODO).
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(APP_NAME testmymulti)
project(${APP_NAME})

################## src
file(GLOB mysrc "./src/*.cpp"  "./src/*.h")

# add include directory
include_directories(./src)
#include_directories(../code)
include_directories(SYSTEM ./thirdparty/include)

################## include
# add all source files
add_executable(${APP_NAME}  
    ${mysrc}
)

################## link

message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

if(APPLE)

    #set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lib/macos/assimp)
    #find_library(ASSIMP_LIB NAMES libassimp.dylib PATHS ${LIBS_DIR} NO_DEFAULT_PATH)
    target_link_libraries(${APP_NAME} PRIVATE  mymulti)

else()

    #set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lib/win/assimp)
    #find_library(ASSIMP_LIB NAMES assimp-vc143-mtd.lib PATHS ${LIBS_DIR} NO_DEFAULT_PATH)
    target_link_libraries(${APP_NAME} PRIVATE mymulti)

    add_custom_command(TARGET ${APP_NAME} POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy_if_different 
       $<TARGET_FILE:mymulti> #TARGET_RUNTIME_DLLS
       $<TARGET_FILE_DIR:${APP_NAME}> 
    )

endif()