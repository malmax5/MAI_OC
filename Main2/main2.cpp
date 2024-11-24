#include <iostream>
#include <dlfcn.h>

typedef int (*PrimeCountFunc)(int, int);
typedef float (*EFunc)(int);

int main() {
    void* handle_prime_count = dlopen("./libprime_count_eratosthenes.so", RTLD_LAZY);
    void* handle_e_formule = dlopen("./libe_formule.so", RTLD_LAZY);

    if (!handle_prime_count || !handle_e_formule) {
        std::cerr << "Cannot open libraries: " << dlerror() << std::endl;
        return 1;
    }

    PrimeCountFunc PrimeCount = (PrimeCountFunc)dlsym(handle_prime_count, "PrimeCount");
    EFunc E = (EFunc)dlsym(handle_e_formule, "E");

    if (!PrimeCount || !E) {
        std::cerr << "Cannot load symbols: " << dlerror() << std::endl;
        dlclose(handle_prime_count);
        dlclose(handle_e_formule);
        return 1;
    }

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

    dlclose(handle_prime_count);
    dlclose(handle_e_formule);
    return 0;
}