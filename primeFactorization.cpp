#include <iostream>
#include <cmath>
#include <boost/multiprecision/cpp_int.hpp>
#include "external/Miller_Rabin/miller_rabin.hpp"

using boost::multiprecision::cpp_int;

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

// https://en.wikipedia.org/wiki/Pollard's_rho_algorithm
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

cpp_int bruteFactor (const cpp_int& n) {
    auto sqrtn = ceil_sqrt(n);

    for (cpp_int i = 3; i < sqrtn; i ++= 2)
        if (n % i == 0)
            return i;

    return n;
}

cpp_int findFactor(const cpp_int& n) {
    if (n % 2 == 0)
        return 2;

    if (fudmottin::millerRabinTest(n))
        return n;

    auto div = pollardsRho(n);

    if (div == n)
        return div;
    else
        return bruteFactor(div);
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

