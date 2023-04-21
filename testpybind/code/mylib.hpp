//
//  mylib.hpp
//
//
//  Created by kai chen on 3/24/23.
//

#pragma once

#ifdef _WIN32
    #define MYLIB_FUNC_DECL __declspec(dllexport)
#else
    #define MYLIB_FUNC_DECL
#endif

#include <vector>
#include <string>
struct MyItem {
    int a;
    int b;
    MyItem() = default;
    MyItem(int a_, int b_) : a(a_), b(b_){}
    std::string to_string() const {
        int totallen = 1024 * 1024;
        std::vector<char> s(totallen);
        char* buf = s.data();
        int len = 0;

        len += sprintf_s(buf + len, totallen - len, "<MyItem:a %d  b:%d>\n", a, b);
        std::string ss(buf);
        return ss;
    }
};

struct MyClass {
    std::vector<int> contents;
    std::vector<MyItem> items;

    std::string to_string() const {
        const MyClass& a = *this;
        int totallen = 1024 * 1024;
        std::vector<char> s(totallen);
        char* buf = s.data();
        int len = 0;

        len += sprintf_s(buf + len, totallen - len, "<MyClass:\n");
        len += sprintf_s(buf + len, totallen - len, "contents:\n");
        for(size_t i = 0; i < a.contents.size(); i++) {
            len += sprintf_s(buf + len, totallen - len, "    %zd:%d\n", i, a.contents[i]);
        }
        len += sprintf_s(buf + len, totallen - len, "items:\n");
        for(size_t i = 0; i < a.items.size(); i++) {
            len += sprintf_s(buf + len, totallen - len, "    %zd:%s\n", i, a.items[i].to_string().c_str());
        }
        len += sprintf_s(buf + len, totallen - len, ">\n");
        std::string ss(buf);
        return ss;
    }
};

MYLIB_FUNC_DECL int mylibadd(int a, int b);
MYLIB_FUNC_DECL void print_vector(const std::vector<int>& v);
