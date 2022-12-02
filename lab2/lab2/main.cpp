#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

unsigned long long putere(int a, int b) {
    unsigned long long res = 1;

    while (--b) {
        res *= a;
    }

    return res;
}

int main() {
    int a = 5, b = 5, c = 5;

    unsigned long long a_la_b_la_c = putere(a, putere(b, c)); // asa calculezi a la puterea b la puterea c

    std::cout << a_la_b_la_c << '\n';
}
