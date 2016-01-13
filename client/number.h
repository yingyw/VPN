#pragma once

#include <iostream>
#include <string.h>
#include <tuple>

#define IS_PRIME_ITERATIONS 10

namespace RSA
{

class Number
{
public:
    explicit Number(const uint8_t* ni, size_t l) 
    {
        n = (uint8_t*)malloc(l);
        memcpy(n, ni, l);
        length = l;
    }
    explicit Number(const char* ni, size_t l) 
    {
        n = (uint8_t*)malloc(l);
        memcpy(n, ni, l);
        length = l;
    }
    explicit Number(size_t l)
    {
        n = (uint8_t*)malloc(l);
        memset(n, 0, l);
        length = l;
    }
    Number(unsigned long long num)
    {
        length = 0;
        while (1ul << (8 * (++length)) <= num);
        n = (uint8_t*)malloc(length);
        for (int i = 0; i < length; i++)
        {
            n[i] = (num >> (i * 8));
        }
    }
    Number(const Number& cp)
        : Number(cp.n, cp.length)
    {
    }

    ~Number()
    {
        //std::cout << "free(" << n << ")" << std::endl;
        free(n);
    }
    uint8_t* n;
    size_t length;

    Number operator+(const Number& n2) const;

    Number operator-(const Number& n2) const;

    Number operator*(const Number& n2) const;

    bool operator>(const Number& n2) const;

    bool operator<(const Number& n2) const;

    bool operator>=(const Number& n2) const;

    bool operator<=(const Number& n2) const;

    Number operator>>(const int& s) const;

    Number operator<<(const int& s) const;

    Number operator%(const Number& n2) const;

    Number operator/(const Number& n2) const;

    bool operator==(const Number& n2) const;

    bool operator!=(const Number& n2) const;

    Number& operator=(const Number& n2);

    static Number random(size_t l);

    static Number random(const Number& max);

    bool isPrime() const;
};

std::ostream& operator<<(std::ostream& os, const Number& n);

std::tuple<Number, Number> operator|(const Number& n1, const Number& n2);

Number modpow(Number x, Number exp, const Number& m);

//std::tuple<Number, Number, Number> extendedEuclid(const Number& a, const Number& b);
Number gcd(const Number& a, const Number& b);

Number modinv(Number a, Number n);

}
