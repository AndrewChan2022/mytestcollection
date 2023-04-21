//
//  mylib.cpp
//
//
//  Created by kai chen on 3/24/23.
//

#include "mylib.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>


#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(std::vector<int>);
PYBIND11_MAKE_OPAQUE(std::vector<MyItem>);

PYBIND11_MODULE(mylibexample, m) {

    py::bind_vector<std::vector<int>>(m, "VectorInt");
    py::bind_vector<std::vector<MyItem>>(m, "VectorItem");
    
    py::implicitly_convertible<py::list, std::vector<int>>();
    py::implicitly_convertible<py::list, std::vector<MyItem>>();

    py::class_<MyItem>(m, "MyItem")
        .def(py::init())
        .def(py::init<int, int>())
        .def_readwrite("a", &MyItem::a)
        .def_readwrite("b", &MyItem::b)
        .def("__repr__", &MyItem::to_string);

    py::class_<MyClass>(m, "MyClass")
        .def(py::init())
        .def_readwrite("contents", &MyClass::contents)
        .def_readwrite("items", &MyClass::items)
        .def("__repr__", [](const MyClass& a) { return a.to_string(); });

    m.def("print_vector", &print_vector);

    m.def("mylibadd", &mylibadd, R"pbdoc(
        Add two numbers

        Some other explanation about the add function.
    )pbdoc");

    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: mylib

        .. autosummary::
           :toctree: _generate

           mylibadd
    )pbdoc";

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}

