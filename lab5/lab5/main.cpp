#include "Polynomial.h"
#include "Multiplications.h"

int main() {
    bool done = false;

    while (!done) {
        int command;
        std::cout << "Insert command: ";
        std::cin >> command;

        switch (command)
        {
        case 0:
            done = true;
            std::cout << "Exiting...\n";
            break;

        case 1:
            {
            Polynomial pol1{ std::vector<int>{3, 0, 5, 1},  simple_sequential };
            Polynomial pol2{ std::vector<int>{1, 2, 6, 0, 3},  simple_sequential };

            std::cout << pol1 * pol2 << "\n\n";

            Polynomial pol3{ 10000,  simple_sequential };
            Polynomial pol4{ 10000,  simple_sequential };
            auto res = pol3 * pol4;
            }
            break;

        case 2:
            {
            Polynomial pol1{ std::vector<int>{3, 0, 5, 1},  karatsuba_sequential };
            Polynomial pol2{ std::vector<int>{1, 2, 6, 0, 3},  karatsuba_sequential };

            std::cout << pol1 * pol2 << "\n\n";

            Polynomial pol3{ 10000,  karatsuba_sequential };
            Polynomial pol4{ 10000,  karatsuba_sequential };
            auto res = pol3 * pol4;
            }
            break;

        case 3:
            {
            Polynomial pol1{ std::vector<int>{3, 0, 5, 1},  simple_parallel };
            Polynomial pol2{ std::vector<int>{1, 2, 6, 0, 3},  simple_parallel };

            std::cout << pol1 * pol2 << "\n\n";

            Polynomial pol3{ 10000,  simple_parallel };
            Polynomial pol4{ 10000,  simple_parallel };
            auto res = pol3 * pol4;
            }
            break;

        case 4:
            {
            Polynomial pol1{ std::vector<int>{3, 0, 5, 1},  karatsuba_parallel };
            Polynomial pol2{ std::vector<int>{1, 2, 6, 0, 3},  karatsuba_parallel };

            std::cout << pol1 * pol2 << "\n\n";

            Polynomial pol3{ 10000,  karatsuba_parallel };
            Polynomial pol4{ 10000,  karatsuba_parallel };
            auto res = pol3 * pol4;
            }
            break;

        default:
            std::cout << "Wrong command";
            break;
        }
    }
}