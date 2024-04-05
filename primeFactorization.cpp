#include <iostream>
#include <random>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/random.hpp>

using namespace boost::multiprecision;

const int MILLERRABINITERATIONS = 40;

cpp_int powMod(cpp_int base, cpp_int exp, const cpp_int& mod) {
    cpp_int result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}

bool millerRabinTest(const cpp_int& n) {
    if (n <= 1 || n == 4) return false;
    if (n <= 3) return true;

    cpp_int d = n - 1;
    while ((d & 1) == 0) {
        d >>= 1;
    }

    boost::random::mt19937_64 rng(std::random_device{}());
    for (int i = 0; i < MILLERRABINITERATIONS; i++) {
        cpp_int a = boost::random::uniform_int_distribution<cpp_int>(2, n - 2)(rng);
        cpp_int x = powMod(a, d, n);
        if (x == 1 || x == n - 1) continue;

        bool isPrime = false;
        for (cpp_int r = 1; r < d; r *= 2) {
            x = powMod(x, 2, n);
            if (x == 1) return false;
            if (x == n - 1) {
                isPrime = true;
                break;
            }
        }

        if (!isPrime) return false;
    }

    return true;
}

void printFactors(cpp_int n, bool isRoot = true) {
    cpp_int factor = 2;
    bool firstFactor = true;
    while (n > 1 && factor * factor <= n) {
        int exp = 0;
        while (n % factor == 0) {
            ++exp;
            n /= factor;
        }
        if (exp > 0) {
            if (!firstFactor) std::cout << " * ";
            firstFactor = false;
            
            std::cout << factor;
            if (exp > 1) {
                std::cout << "^";
                if (!millerRabinTest(exp)) { // exp is composite
                    std::cout << "(";
                    printFactors(exp, false); // Recursive call for exponent factorization
                    std::cout << ")";
                } else {
                    std::cout << exp; // exp is prime
                }
            }
            std::cout.flush(); // Ensure the output is printed immediately
        }
        factor = (factor == 2) ? cpp_int(3) : factor + 2;
    }
    if (n > 1) {
        if (!firstFactor) std::cout << " * ";
        std::cout << n;
        std::cout.flush();
    }
}

void primeFactorization(cpp_int n) {
    if (n <= 1) {
        std::cout << "No prime factorization for " << n << std::endl;
        return;
    }

    if (millerRabinTest(n)) {
        std::cout << n << " = " << n << " (prime)" << std::endl;
        return;
    }

    std::cout << n << " = ";
    printFactors(n);
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <positive integer greater than 1>" << std::endl;
        return 1;
    }

    cpp_int number;
    try {
        number = cpp_int(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid input: " << e.what() << std::endl;
        return 1;
    }

    if (number < 2) {
        std::cerr << "Usage: " << argv[0] << "< positive integer greater than 1>" << std::endl;
        return 1;
    }

    primeFactorization(number);

    return 0;
}

