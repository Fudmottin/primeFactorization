# Prime Factorization Program

This C++ program performs prime factorization for positive integers greater than
1 . It factors the input number into its prime factors and their exponents,
showing the prime factorization in the form of a product of prime numbers raised
to their respective powers.

## Compilation

The program can be compiled using the following command:

```sh
c++ -std=c++20 -O3 -o primeFactorization primeFactorization.cpp
```

## Usage

To factorize a positive integer greater than 1, run the compiled executable with the integer as a command-line argument. For example:

```sh
./primeFactorization 123456789
```

This will output the prime factorization of 123456789:

```
123456789 = 3^2 * 3607 * 3803
```

## Dependencies

- [Boost C++ Libraries](https://www.boost.org/) (specifically `boost/multiprecision/cpp_int.hpp`)

## License

This program is licensed under the terms of the MIT license. See the LICENSE file for more information.

