// testcpppython.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <iostream>
#include <Python.h>
using namespace std;
int main(int argc, char const* argv[])
{
    Py_Initialize();

    std::string s = R"(
import pythoncom, pywintypes


pythoncom.CoInitialize()

clsid = pywintypes.IID("{177F0C4A-1CD3-4DE7-A32C-71DBBB9FA36D}")
iid = pywintypes.IID("{42843719-DB4C-46C2-8E7C-64F1816EFD5B}")
obj = pythoncom.CoCreateInstance(clsid, None, 1, iid)
print(obj)
    )";
    PyRun_SimpleString(s.c_str());

    Py_Finalize();
    return 0;
}
