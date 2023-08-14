
debug with python source and extension source

invoke python code from c++

so cannot debug python code

but can debug python source code and extension source code

1. install python with python debug symbol and debug lib
2. download python code and put some place
3. create vs c++ project, set include path and lib path of installed python
4. c++ call python to execute python code, then phthon code call c++ lib
    Py_Initialize();
    PyRun_SimpleString(s.c_str());
5. open python extension lib code and set break point
6. open python code and set break point

