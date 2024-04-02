#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>

using namespace boost::multiprecision;

cpp_int smallestPrimeFactor(cpp_int n) {
    if ((n & 1) == 0) return 2;
    for (cpp_int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return i;
    }
    return n; // n is prime
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
                cpp_int smallestFactor = smallestPrimeFactor(exp);
                if (smallestFactor != exp) { // exp is composite
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

    if (smallestPrimeFactor(n) == n) {
        std::cout << n << " = " << n << " (prime)" << std::endl;
        return;
    }

    std::cout << n << " = ";
    printFactors(n);
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << "<positive integer greater than 1 >" << std::endl;
        return 1;
    }

    cpp_int number;
    try {
        number = cpp_int(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid input." << std::endl;
        return 1;
    }

    primeFactorization(number);

    return 0;
}

