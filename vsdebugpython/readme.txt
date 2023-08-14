
debug c++ or debug python

cannot debug both
cannot debug python source

1. create vs python project and solution, config python env
    view>other window>python env
        C:\Users\CK\AppData\Local\Programs\Python\Python310
2. create cpython extension
    add c++ project
    toolbar:
        debug
        cpu: x64
    solution properties:
        header line:
            configuration:  active debug
            platform: active x64
        general:
            configration type_: dll
            advanced>target file extension: pyd
        c++:
            general> include path: python header
                C:\Users\CK\AppData\Local\Programs\Python\Python310\include
            preprocessor>
                python_d.exe    : _DEBUG    // default, not change
            code generation>runtime:
                python_d.exe    : /MDd      // default, not change
            code generator>basic runtime check: 
                default
        linker:
            general>addition lib path:
                C:\Users\CK\AppData\Local\Programs\Python\Python310\Libs
            input> dependency:
                C:\Users\CK\AppData\Local\Programs\Python\Python310\Libs\pythond_d.lib
                // python header file will decide lib name by debug flag
3. c++ code edit
4. error workaround
    
    vs cannot force using pythond_d.exe
    
    so force using python310.lib/python310.dll not python310_d.lib/python310_d.dll

    #if defined(_MSC_VER) && defined(_DEBUG)
    #undef _DEBUG
    #include <Python.h>
    #define _DEBUG 1
    #else
    #include <Python.h>
    #endif

4. debug python
    set break point and debug

5. debug c++
    project property > debug:  enable native debug

6. mix debug

    I donot know how

7. python source code debug

    I donot know how






