#include <iostream>
#include "../Lib/include/Func_1/prime_count_eratosthenes.hpp"
#include "../Lib/include/Func_2/e_formule.hpp"

int main() {
    int command;
    while (true) {
        std::cout << "Enter command (1 for PrimeCount, 2 for E, 0 to exit): ";
        std::cin >> command;

        if (command == 0) {
            break;
        } else if (command == 1) {
            int A, B;
            std::cout << "Enter A and B: ";
            std::cin >> A >> B;
            std::cout << "Prime count between " << A << " and " << B << " is " << PrimeCount(A, B) << std::endl;
        } else if (command == 2) {
            int x;
            std::cout << "Enter x: ";
            std::cin >> x;
            std::cout << "E(" << x << ") = " << E(x) << std::endl;
        } else {
            std::cout << "Unknown command" << std::endl;
        }
    }
    return 0;
}