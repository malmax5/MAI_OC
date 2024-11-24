#include "../../include/Func_1/prime_count_eratosthenes.hpp"

int PrimeCount(int A, int B) {
    if (B < 2) return 0;
    if (A < 2) A = 2;

    std::vector<bool> is_prime(B + 1, true);
    is_prime[0] = is_prime[1] = false;

    for (int i = 2; i * i <= B; ++i) {
        if (is_prime[i]) {
            for (int j = i * i; j <= B; j += i) {
                is_prime[j] = false;
            }
        }
    }

    int count = 0;
    for (int i = A; i <= B; ++i) {
        if (is_prime[i]) ++count;
    }
    return count;
}
