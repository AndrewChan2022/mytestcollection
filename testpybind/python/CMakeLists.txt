cmake_minimum_required(VERSION 3.1)

#add_definitions(-DEXCLUDE_LOG_MODULE)
#add_compile_definitions(MG_ENABLE_OPENSSL=1)

# cpp version (be different for other platform, TODO).
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(PYTHON_MODULE_NAME mylibexample)
project(${PYTHON_MODULE_NAME})


################## pybind11
add_subdirectory(pybind11)


################## src
file(GLOB mysrc "./src/*.cpp"  "./src/*.h")

################## include
include_directories(./src)


################## app
pybind11_add_module(${PYTHON_MODULE_NAME} ${mysrc})
# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
# define (VERSION_INFO) here.
target_compile_definitions(${PYTHON_MODULE_NAME} PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})

################## link

message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

if(APPLE)

    #set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lib/macos/assimp)
    #find_library(ASSIMP_LIB NAMES libassimp.dylib PATHS ${LIBS_DIR} NO_DEFAULT_PATH)
    set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../test/thirdparty/lib/macos/)
    find_library(mymulti NAMES mymulti.dylib PATHS ${LIBS_DIR} NO_DEFAULT_PATH)

    target_link_libraries(${APP_NAME} PRIVATE  mylib mymulti)

else()

    set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../test/thirdparty/lib/win)
    message(STATUS "LIBS_DIR: ${LIBS_DIR}")

    # set(USE_FIND ON)
    if(USE_FIND)
        ### find_library 
        find_library(mymulti_lib NAMES mymulti PATHS ${LIBS_DIR} NO_DEFAULT_PATH)
        message(STATUS "mymulti_lib: ${mymulti_lib}")

        target_link_libraries(${PYTHON_MODULE_NAME} PRIVATE mylib ${mymulti_lib})
        add_custom_command(TARGET ${PYTHON_MODULE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            ${LIBS_DIR}/mymulti.dll
            $<TARGET_FILE_DIR:${PYTHON_MODULE_NAME}> 
        )
    else()

        add_library(mymulti SHARED IMPORTED)
        set_target_properties(mymulti PROPERTIES 
            IMPORTED_LOCATION ${LIBS_DIR}/mymulti.dll
            IMPORTED_LOCATION_DEBUG ${LIBS_DIR}/mymulti.dll
            IMPORTED_LOCATION_RELEASE ${LIBS_DIR}/mymulti.dll
            IMPORTED_LOCATION_MINSIZEREL ${LIBS_DIR}/mymulti.dll
            IMPORTED_LOCATION_RELWITHDEBINFO ${LIBS_DIR}/mymulti.dll
            IMPORTED_IMPLIB ${LIBS_DIR}/mymulti.lib
            IMPORTED_IMPLIB_DEBUG ${LIBS_DIR}/mymulti.lib
            IMPORTED_IMPLIB_RELEASE ${LIBS_DIR}/mymulti.lib
        )
        message(STATUS "mymulti: ${mymulti}")

        target_link_libraries(${PYTHON_MODULE_NAME} PRIVATE mylib mymulti)
        add_custom_command(TARGET ${PYTHON_MODULE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            $<TARGET_FILE:mymulti>
            $<TARGET_FILE_DIR:${PYTHON_MODULE_NAME}>
        )
    endif()

    add_custom_command(TARGET ${PYTHON_MODULE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        $<TARGET_FILE:mylib> #TARGET_RUNTIME_DLLS
        $<TARGET_FILE_DIR:${PYTHON_MODULE_NAME}> 
    )
endif()
