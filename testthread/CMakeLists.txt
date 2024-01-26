cmake_minimum_required(VERSION 3.1)

#add_definitions(-DEXCLUDE_LOG_MODULE)
#add_compile_definitions(MG_ENABLE_OPENSSL=1)

# cpp version (be different for other platform, TODO).
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

project(testthread)


################## src
# add .cpp
file(GLOB mysrc "./src/*.cpp" "./src/*.h" "./src/*.hpp")


# add include directory
include_directories(./src)


################## include
# add all source files
add_executable(testthread 
    ${mysrc}
)

################## link
