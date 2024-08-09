#include <iostream>
#include <vector>
#include <cmath>
#include <boost/multiprecision/cpp_int.hpp>
#include "external/Miller_Rabin/miller_rabin.hpp"

using namespace boost::multiprecision;

cpp_int ceil_sqrt(const cpp_int& n) {
    cpp_int left = 0;
    cpp_int right = n;
    cpp_int result = 0;

    while (left <= right) {
        cpp_int mid = left + (right - left) / 2;
        if (mid * mid <= n) {
            result = mid;
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return result;
}

cpp_int gcd(const cpp_int& a, const cpp_int& b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

cpp_int pollardsRho(const cpp_int& n) {
    cpp_int x = 2, y = 2, d = 1;
    const cpp_int c = 1;  // constant in the polynomial x^2 + c

    auto f = [&](const cpp_int& x) { return (x * x + c) % n; };

    while (d == 1) {
        x = f(x);
        y = f(f(y));
        d = gcd(abs(x - y), n);
    }

    return d;
}

cpp_int findFactor(const cpp_int& n) {
    if (n % 2 == 0)
        return 2;

    if (fudmottin::millerRabinTest(n))
        return n;

    // Use Pollard's Rho for more efficient factorization
    cpp_int factor = pollardsRho(n);

    // If factor equals n, Pollard's Rho failed, fallback to brute force
    if (factor == n) {
        auto sqrt_n = ceil_sqrt(n);
        for (cpp_int factor = 3; factor <= sqrt_n; factor += 2) {
            if (n % factor == 0)
                return factor;
        }
        return n;
    }

    // Ensure the factor is prime
    if (!fudmottin::millerRabinTest(factor))
        return findFactor(factor);
    return factor;
}

void primeFactorization(cpp_int n) {
    cpp_int fac;
    do {
        fac = findFactor(n);
        cpp_int exp = 0;

        if (fac == n) {
            std::cout << n;
            return;
        }

        do {
            ++exp;
            n /= fac;
        } while ((n % fac) == 0);

        std::cout << fac;

        if (exp > 1) {
            std::cout << "^";
            if (!fudmottin::millerRabinTest(exp)) {
                std::cout << "(";
                primeFactorization(exp);
                std::cout << ")";
            } else std::cout << exp;
        }

        if (n > 2) std::cout << " * ";
        std::cout.flush();
    } while (n > 1);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <positive integer greater than 1>" << std::endl;
        return -1;
    }

    cpp_int number;
    try {
        number = cpp_int(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Error: Invalid input: " << e.what() << std::endl;
        return -1;
    }

    if (number < 2) {
        std::cerr << "Usage: " << argv[0] << "< positive integer greater than 1>" << std::endl;
        return -1;
    }

    std::cout << " = ";
    primeFactorization(number);
    std::cout << std::endl;

    return 0;
}

