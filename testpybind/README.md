

- [Introduction](#introduction)
  * [file tree](#file-tree)
  * [python bind](#python-bind)
    + [function](#function)
    + [enum](#enum)
    + [class](#class)
    + [class inherit](#class-inherit)
  * [python bind stl container](#python-bind-stl-container)
    + [auto type convert](#auto-type-convert)
    + [opaque type](#opaque-type)
    + [mix convert and opaque type](#mix-convert-and-opaque-type)
  * [cmake for python install](#cmake-for-python-install)
    + [add python module](#add-python-module)
    + [link and copy shared dll](#link-and-copy-shared-dll)
      - [native dll](#native-dll)
      - [external dll](#external-dll)
        * [find_library method](#find-library-method)
        * [add_library method](#add-library-method)
- [build](#build)
  * [options](#options)
  * [build script](#build-script)
  * [general build and test](#general-build-and-test)
  * [linux](#linux)
  * [macos](#macos)
  * [windows](#windows)
  * [prerequisite](#prerequisite)
- [Acknowledgements](#acknowledgements)

<small><i><a href='http://ecotrust-canada.github.io/markdown-toc/'>Table of contents generated with markdown-toc</a></i></small>


# Introduction

## file tree


    // native lib
    code                    : native shared lib
    CMakeLists.txt          : cmake to build native shared lib

    // external lib
    test/thirdparty         : external shared lib

    // test code
    test/src                : test exe
    test/CMakeLists.txt     : cmake to build test exe with native lib and external lib

    // python bind
    python/pybind11         : header only lib for python bind
    python/src              : code to bind python
    python/test.py          : test python

    // install python bind
    python/CMakeLists.txt   : cmake to build .pyd and copy native/external dll
    pyproject.toml          : install python
    setup.py                : install python


    +root
    |   CMakeLists.txt
    |   pyproject.toml
    |   README.md
    |   setup.py
    |
    +---+---code
    |   |       mylib.cpp
    |   |       mylib.hpp
    |   |
    |   +---python
    |   |   |   CMakeLists.txt
    |   |   |   test.py
    |   |   |
    |   |   +---pybind11
    |   |   |
    |   |   |---src
    |   |           mylibbind.cpp
    |   |
    |   |---test
    |       |   CMakeLists.txt
    |       |
    |       +---src
    |       |       main.cpp
    |       |
    |       |---thirdparty
    |           +---include
    |           |   |---mymulti
    |           |           mymulti.hpp
    |           |
    |           |---lib
    |               +---macos
    |               |---win
    |                       mymulti.dll
    |                       mymulti.lib


## python bind

```cpp
#include "mylib.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

namespace py = pybind11;
PYBIND11_MODULE(mylibexample, m) {
    // binding code
}
```

### function

```cpp
// bind function
m.def("mylibadd", &mylibadd)   // bind function

// with keyword arg
m.def("mylibadd", &mylibadd, py::arg("i"), py::arg("j"));
using namespace pybind11::literals;
m.def("add2", &add, "i"_a, "j"_a);

// with default value
m.def("add", &add, py::arg("i") = 1, py::arg("j") = 2);        
m.def("add2", &add, "i"_a=1, "j"_a=2);

```


### enum

```cpp
py::enum_<ERootType>(m, "ERootType")
    .value("RootZ", ERootType::RootZ)
    .value("RootZMinusGroundZ", ERootType::RootZMinusGroundZ)
    .value("Ignore", ERootType::Ignore);
    //.export_values(); // to parent space
```

```python
import mylibexample

# export_values
mylibexample.RootZ

# no export_values
mylibexample.ERootType.RootZ
```

### class

```cpp
py::class_<MyClass>(m, "MyClass")

    // construct
    .def(py::init())
    .def(py::init<int>())

    // data field
    .def_readwrite("contents", &MyClass::contents)

    // property
    .def_property("name", &MyClass::getName, &MyClass::setName)

    // method
    .def_property("getName", &MyClass::getName)

    // print
    .def("__repr__", [](const MyClass& a) { return a.to_string(); });   // print
```

### class inherit

```cpp
// TODO:
```

## python bind stl container

### auto type convert

```cpp
// cpp
struct MyClass {
    std::vector<int> contents;
};

// binding
PYBIND11_MODULE(mylibexample, m) {
    py::class_<MyClass>(m, "MyClass")
        .def(py::init())
        .def_readwrite("contents", &MyClass::contents)
        .def("__repr__", [](const MyClass& a) { return a.to_string(); });
}
```

```python
# python
import mylibexample
o = mylibexample.MyClass()
o.contents = [0, 1, 2]      # pass python list to cpp and convert to std::vector
o.contents.append(3)        # no effect, because pass by value copy, not reference
```

### opaque type

```cpp
// binding
PYBIND11_MAKE_OPAQUE(std::vector<int>); // disable auto convert

PYBIND11_MODULE(mylibexample, m) {
    py::bind_vector<std::vector<int>>(m, "VectorInt");  // disable auto convert
    ...
}
```

```python
# python
import mylibexample
o = mylibexample.MyClass()
o.contents = [0, 1, 2]      # error, cannot convert python list to VectorInt
o.contents.append(3)        # correct, pass by ref
```

### mix convert and opaque type

```cpp
// binding
PYBIND11_MAKE_OPAQUE(std::vector<int>); // disable auto convert

PYBIND11_MODULE(mylibexample, m) {
    py::bind_vector<std::vector<int>>(m, "VectorInt");  // disable auto convert
    
    py::implicitly_convertible<py::list, std::vector<int>>();   // type convert
    ...
}
```

```python
# python
import mylibexample
o = mylibexample.MyClass()
o.contents = [0, 1, 2]      # correct, implicitly_convertible
o.contents.append(3)        # correct, pass by ref
```


## cmake for python install

cmake will build bind code to pyd, and copy native dll and external dll


### add python module

use pybind11_add_module instead of add_library

```cmake
add_subdirectory(pybind11)
pybind11_add_module(${PYTHON_MODULE_NAME} ${mysrc})
```


### link and copy shared dll


#### native dll
to copy native dll, only get path by $<TARGET_FILE:lib_name>

```cmake
target_link_libraries(${PYTHON_MODULE_NAME} PRIVATE mylib) 
add_custom_command(TARGET ${PYTHON_MODULE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    $<TARGET_FILE:mylib>
    $<TARGET_FILE_DIR:${PYTHON_MODULE_NAME}> 
)
```

#### external dll

to copy external dll, there are two ways:
    
    find_library
    or 
    add_library with IMPORTED

##### find_library method

```cmake
set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../test/thirdparty/lib/win)
find_library(mymulti_lib NAMES mymulti PATHS ${LIBS_DIR} NO_DEFAULT_PATH)  ## .lib
target_link_libraries(${PYTHON_MODULE_NAME} PRIVATE ${mymulti_lib}) 
add_custom_command(TARGET ${PYTHON_MODULE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${LIBS_DIR}/mymulti.dll
        $<TARGET_FILE_DIR:${PYTHON_MODULE_NAME}> 
    )
```

##### add_library method

```cmake
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

target_link_libraries(${PYTHON_MODULE_NAME} PRIVATE mymulti)
add_custom_command(TARGET ${PYTHON_MODULE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
    $<TARGET_FILE:mymulti>
    $<TARGET_FILE_DIR:${PYTHON_MODULE_NAME}>
)
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
