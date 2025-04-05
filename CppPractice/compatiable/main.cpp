#include "foo.h"
#include <iostream>
#include <functional>

int main() {
    auto out = std::ref(std::cout << "result from C code: " << add(1, 2));
    [&] {
        out.get() << ".\n";
    }();

    return 0;
 }