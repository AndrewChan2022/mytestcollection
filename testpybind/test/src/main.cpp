//
//  main.cpp
//  test ikrigretarget
//
//  Created by kai chen on 2/8/23.
//

#include <stdio.h>
#include "mylib.hpp"
#include "mymulti/mymulti.hpp"


int main(int argc, char *argv[]) {

    printf("test mylib mylibadd(4, 5):%d\n", mylibadd(4, 5));
    printf("test mymulti mymultiply(4, 5):%d\n", mymultiply(4, 5));

    printf("print_vector:\n");
    print_vector({1,2,3});

    MyClass o;
    o.contents.push_back(1);
    printf("%s\n", o.to_string().c_str());

    return 0;
}
