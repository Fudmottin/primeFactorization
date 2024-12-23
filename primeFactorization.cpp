#include <iostream>
#include <cmath>
#include <future>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <algorithm>
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
    if (b == 0) return a;
    return gcd(b, a % b);
}

// https://en.wikipedia.org/wiki/Pollard's_rho_algorithm
cpp_int pollardsRho(const cpp_int& n, int c = 1) {
    cpp_int x = 2, y = 2, d = 1;
    auto f = [c, n](cpp_int x) { return (x * x + c) % n; };

    while (d == 1) {
        x = f(x);
        y = f(f(y));
        d = gcd(abs(x - y), n);
    }

    return (d == n) ? cpp_int(0) : d;
}

cpp_int pollardsRhoParallel(const cpp_int& n) {
    constexpr int primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
                              73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151,
                              157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233,
                              239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317,
                              331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419,
                              421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503,
                              509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607,
                              613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701,
                              709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811,
                              821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911,
                              919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997, 1009, 1013, 1019,
                              1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069, 1087, 1091, 1093, 1097,
                              1103, 1109, 1117, 1123, 1129, 1151, 1153, 1163, 1171, 1181, 1187, 1193, 1201,
                              1213, 1217, 1223, 1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289, 1291,
                              1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373, 1381, 1399, 1409,
                              1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453, 1459, 1471, 1481, 1483, 1487,
                              1489, 1493, 1499, 1511, 1523, 1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579,
                              1583, 1597, 1601, 1607, 1609, 1613, 1619, 1621, 1627, 1637, 1657, 1663, 1667,
                              1669, 1693, 1697, 1699, 1709, 1721, 1723, 1733, 1741, 1747, 1753, 1759, 1777,
                              1783, 1787, 1789, 1801, 1811, 1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877,
                              1879, 1889, 1901, 1907, 1913, 1931, 1933, 1949, 1951, 1973, 1979, 1987, 1993,
                              1997, 1999, 2003};

    int numThreads = std::thread::hardware_concurrency();
    std::atomic<bool> factorFound(false);
    cpp_int result = 0;
    std::mutex resultMutex;

    auto task = [&](int threadIndex) {
        int cIndex = threadIndex;  // Start each thread with a unique c value
        while (!factorFound.load()) {
            if (cIndex >= static_cast<int>(sizeof(primes) / sizeof(primes[0]))) break;
            int c = primes[cIndex++];
            cpp_int factor = pollardsRho(n, c);
            if (factor > 1 && factor < n) {
                {
                    std::lock_guard<std::mutex> lock(resultMutex);
                    if (!factorFound) {
                        result = factor;
                        factorFound = true;
                    }
                }
                break;
            }
        }
    };

    // Launch threads
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(task, i);
    }

    // Wait for threads to complete
    for (auto& t : threads) {
        t.join();
    }

    return result;
}

cpp_int bruteFactor (const cpp_int& n) {
    auto sqrtn = ceil_sqrt(n);

    for (cpp_int i = 3; i < sqrtn; i ++= 2)
        if (n % i == 0)
            return i;

    return n;
}

cpp_int findFactor(const cpp_int& n) {
    if (n % 2 == 0) return 2;

    if (fudmottin::millerRabinTest(n)) return n;

    cpp_int div;
    if (n > 1'000'000'000'000'000'000)
        div = pollardsRhoParallel(n);
    else
        div = pollardsRho(n);

    if (div == 0 || div == n) {
        // Fall back to brute force or report failure
        std::cerr << " brute ";
        div = bruteFactor(n);
    }

    return div;
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

