//
//  mymulti.hpp
//
//
//  Created by kai chen on 3/24/23.
//

#pragma once

#ifdef _WIN32
    #define MYMULTI_FUNC_DECL __declspec(dllexport)
#else
    #define MYLIB_FUNC_DECL
#endif


MYMULTI_FUNC_DECL int mymultiply(int a, int b);
