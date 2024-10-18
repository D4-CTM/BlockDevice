#include <iostream>

struct test {
    unsigned int size;
    unsigned int cant;
};

int main() {
    test* tp = new test();    
    test t = *tp;

    std::cout << &tp << '\n';
    std::cout << &t << '\n';

    return 0;
}