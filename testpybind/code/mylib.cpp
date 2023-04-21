//
//  mylib.cpp
//
//
//  Created by kai chen on 3/24/23.
//

#include "mylib.hpp"
#include <iostream>

int mylibadd(int a, int b) {
    return a + b;
}

void print_vector(const std::vector<int> &v) {
    for (auto item : v)
        std::cout << item << "\n";
}
