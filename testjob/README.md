

# build

## prerequisite

## options

## build script

    ./build.sh

## general build and test

    rm -rf build
    cmake . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --config Release -j 16
    build/test/Release/testmylib.exe

## windows build and test c++

    rm -rf build
    mkdir build && cd build

    // with visual studio
    cmake ..
    open .sln with visual studio
    set test as start project
    
    // with cmake
    cmake ..
    cmake --build . -j 16
    test/Debug/testikrigretarget.exe

## macos

    rm -rf build
    mkdir build && cd build

    // with XCode
    cmake .. -GXcode
    open project
    
    // with make
    cmake ..
    make -j 16
    test/Debug/testikrigretarget.exe

# Acknowledgements
