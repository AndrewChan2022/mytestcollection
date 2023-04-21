
# introduction

build a shared lib and test it

## cpp

for windows platform, need add followed before function declare:
```cpp
__declspec(dllexport)
```

Here we define macro:

```cpp
#ifdef _WIN32
    #define MYMULTI_FUNC_DECL __declspec(dllexport)
#else
    #define MYLIB_FUNC_DECL
#endif

MYMULTI_FUNC_DECL int mymultiply(int a, int b);

```

## cmake

to link and copy dll
```cmake

target_link_libraries(${APP_NAME} PRIVATE mymulti)

add_custom_command(TARGET ${APP_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    $<TARGET_FILE:mymulti> #TARGET_RUNTIME_DLLS
    $<TARGET_FILE_DIR:${APP_NAME}> 
)
```

for macos and linux, no need:
```cmake
target_link_libraries(${APP_NAME} PRIVATE mymulti)
```



# build

## options

    // release during cmake
    -DCMAKE_BUILD_TYPE=Release
    
    // release during build
    --config Release

## build script

    ./build.sh

## general build and test

    rm -rf build
    cmake . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --config Release -j 16
    build/test/Release/testmylib.exe

## linux

    rm -rf build
    mkdir build && cd build

    cmake ..
    make -j 16
    test/Debug/testmylib.exe

## macos

    rm -rf build
    mkdir build && cd build

    // with XCode
    cmake .. -GXcode
    open project
    
    // with make
    cmake ..
    make -j 16
    test/Debug/testmylib.exe

## windows

    rm -rf build
    mkdir build && cd build

    // with visual studio
    cmake ..
    open .sln with visual studio
    set test as start project
    
    // with cmake
    cmake ..
    cmake --build . -j 16
    test/Debug/testmylib.exe

## prerequisite

# Acknowledgements
