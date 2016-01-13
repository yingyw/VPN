#include "number.h"

namespace RSA
{

Number Number::operator+(const Number& n2) const
{
    //Number res(std::max(length, n.length));
    size_t minL = std::min(length, n2.length);
    size_t maxL = std::max(length, n2.length);
    uint8_t* res = (uint8_t*)malloc(maxL + 1);
    int rem = 0;
    for (size_t i = 0; i < minL; i++) 
    {
        res[i] = rem + n[i] + n2.n[i];
        rem = (rem + n[i] + n2.n[i]) >> 8;
    }
    uint8_t* nextN = n2.length > length ? n2.n : n;
    for (int i = minL; i < maxL; i++)
    {
        res[i] = rem + nextN[i];
        rem = (rem + nextN[i]) >> 8;
    }
    res[maxL] = rem;
    Number ret(res, rem == 0 ? maxL : maxL + 1);
    free(res);
    return ret;
}

Number Number::operator-(const Number& n2) const
{
    Number ret(*this);
    int carry = 0;
    for (int i = 0; i < n2.length; i++)
    {
        if (ret.n[i] >= (n2.n[i] + carry))
        {
            ret.n[i] = (ret.n[i] - n2.n[i] - carry);
            carry = 0;
        }
        else
        {
            ret.n[i] = (ret.n[i] - n2.n[i] - carry + 256);
            carry = 1;
        }
    }
    int cInd = n2.length;
    while (carry > 0)
    {
        if (ret.n[cInd] >= (carry % 256))
            ret.n[cInd] -= (carry % 256);
        else
        {
            ret.n[cInd] = ret.n[cInd] + 256 - (carry % 256);
            carry += 256;
        }
        cInd++;
        carry >>= 8;
    }
    while ((ret.length > 1) && (ret.n[ret.length - 1] == 0))
        ret.length--;
    return ret;
}

Number Number::operator*(const Number& n2) const
{
    size_t resMaxL = length + n2.length;
    uint8_t* res = (uint8_t*)malloc(resMaxL);
    memset(res, 0, resMaxL);
    for (int i = 0; i < length; i++)
    {
        int rem = 0;
        for (int j = 0; j < n2.length; j++)
        {
            int nextV = n[i] * n2.n[j] + rem + res[i + j];
            rem = nextV >> 8;
            res[i + j] = nextV % 256;
        }
        int caryInd = i + n2.length;
        while (rem > 0) 
        {
            res[caryInd] = rem % 256;
            rem = rem >> 8;
        }
    }
    int len = resMaxL;
    while ((len > 1) && (res[len - 1] == 0))
        len -= 1;
    Number ret(res, len);
    free(res);
    return ret;
}

bool Number::operator>(const Number& n2) const
{
    if (length != n2.length)
        return length > n2.length;
    for (int i = length - 1; i >= 0; i--)
    {
        if (n[i] > n2.n[i])
            return true;
        if (n[i] < n2.n[i])
            return false;
    }
    return false;
}

bool Number::operator<(const Number& n2) const
{
    if (length != n2.length)
        return length < n2.length;
    for (int i = length - 1; i >= 0; i--)
    {
        if (n[i] < n2.n[i])
            return true;
        if (n[i] > n2.n[i])
            return false;
    }
    return false;
}

bool Number::operator>=(const Number& n2) const
{
    if (length != n2.length)
        return length > n2.length;
    for (int i = length - 1; i >= 0; i--)
    {
        if (n[i] > n2.n[i])
            return true;
        if (n[i] < n2.n[i])
            return false;
    }
    return true;
}

bool Number::operator<=(const Number& n2) const
{
    if (length != n2.length)
        return length < n2.length;
    for (int i = length - 1; i >= 0; i--)
    {
        if (n[i] < n2.n[i])
            return true;
        if (n[i] > n2.n[i])
            return false;
    }
    return true;
}

Number Number::operator>>(const int& s) const
{
    int resLen = length - (s / 8);
    if ((n[length - 1] >> (s % 8)) == 0)
        resLen = resLen - 1;
    if (resLen < 0)
        return 0;
    uint8_t* res = (uint8_t*)malloc(resLen);
    for (int i = 0; i < resLen; i++)
    {
        res[i] = n[i + (s / 8)] + ((i + (s / 8) + 1 < length ? n[i + (s / 8) + 1] : 0) << 8) >> (s % 8);
    }
    Number ret(res, resLen);
    free(res);
    return ret;
}

Number Number::operator<<(const int& s) const
{
    if ((*this) == 0)
        return 0;
    int s8Ceil = s == 0 ? 0 : ((s - 1) / 8 + 1);
    size_t resLen = length + (s / 8) + ((n[length - 1] << (s % 8)) >= 256 ? 1 : 0);
    Number ret(resLen);
    for (int i = 0; i < resLen; i++)
    {
        int ind1 = i - s8Ceil;
        int ind2 = i - s8Ceil + 1;
        ret.n[i] = (((ind1 >= 0) && (ind1 < length) ? n[ind1] : 0) + 
            (((ind2 >= 0) && (ind2 < length) ? n[ind2] : 0) << 8)) >> ((s % 8) == 0 ? 0 : (8 - (s % 8)));
    }
    return ret;
}

Number Number::operator%(const Number& n2) const
{
    Number cp(*this);
    while (cp >= n2)
    {
        Number subt = n2 << (8 * (cp.length - n2.length + 1));
        while (subt > cp)
            subt = (subt >> 1);
        cp = (cp - subt);
    }
    return cp;
}

Number Number::operator/(const Number& n2) const
{
    Number cp(*this);
    Number res = 0;
    while (cp >= n2)
    {
        Number subt = n2 << (8 * (cp.length - n2.length + 1));
        int tExp = (8 * (cp.length - n2.length + 1));
        while (subt > cp) {
            subt = (subt >> 1);
            tExp--;
        }
        cp = (cp - subt);
        res = res + (Number((unsigned long long)1) << tExp);
    }
    return res;
}

bool Number::operator==(const Number& n2) const
{
    if (length != n2.length)
    {
        return false;
    }
    for (int i = 0; i < length; i++)
    {
        if (n[i] != n2.n[i])
            return false;
    }
    return true;
}

bool Number::operator!=(const Number& n2) const
{
    return !((*this) == n2);
}

Number& Number::operator=(const Number& n2)
{
    free(n);
    n = (uint8_t*)malloc(n2.length);
    length = n2.length;
    memcpy(n, n2.n, length);
}

std::ostream& operator<<(std::ostream& os, const Number& n)
{
    for (int i = n.length - 1; i >= 0; i--)
    {
        os << std::hex << (((int)n.n[i]) / 16) << (((int)n.n[i]) % 16);
    }
    return os;
}

Number modpow(Number x, Number exp, const Number& m)
{
    Number res = 1;
    while (exp > 0)
    {
        if (exp.n[0] & 1)
            res = (res * x) % m;
        exp = exp >> 1;
        x = (x * x) % m;
    }
    return res;
}

Number Number::random(size_t l)
{
    Number res(l);
    for (int i = 0; i < l - 1; i++)
        res.n[i] = std::rand() % 256;
    res.n[l - 1] = (std::rand() % 255) + 1;
    return res;
}

Number Number::random(const Number& max)
{
    Number res(max.length);
    do
    {
        for (int i = 0; i < max.length - 1; i++)
            res.n[i] = std::rand() % 256;
        res.n[max.length - 1] = std::rand() % (max.n[max.length - 1] + 1);
    } while (res >= max);
    while ((res.length > 1) && (res.n[res.length - 1] == 0))
        res.length--;
    return res;
}

bool Number::isPrime() const
{
    if (length == 1)
    {
        if ((n[0] == 0) || (n[0] == 1))
            return false;
        if ((n[0] == 2) || (n[0] == 3))
            return true;
    }
    if ((n[0] & 1) == 0)
        return false;
    int r = 0;
    Number d = (*this) - 1;
    while (!(d.n[0] & 1))
    {
        r++;
        d = (d >> 1);
    }
    for (int reps = 0; reps < IS_PRIME_ITERATIONS; reps++)
    {
        Number a = Number::random((*this) - 4) + 2;
        Number x = modpow(a, d, (*this));
        if ((x == 1) || (x == (*this) - 1)) {
            // std::cout << a << " 1is not witness for " << (*this) << std::endl;
            continue;
        }
        int i = 0;
        for (; i < r - 1; i++)
        {
            x = (x * x) % (*this);
            if (x == 1) {
                // std::cout << a << " 2is witness for " << (*this) << std::endl;
                return false;
            }
            if (x == (*this) - 1) {
                // std::cout << a << " 3is not witness for " << (*this) << std::endl;
                i = -1;
                break;
            }
        }
        if (i != -1){
            // std::cout << a << " 4is witness for " << (*this) << std::endl;
            return false;
        }
    }
    // std::cout << (*this) << " is prime" << std::endl;
    return true;
}

std::tuple<Number, Number> operator|(const Number& n1, const Number& n2)
{
    Number cp(n1);
    Number res = 0;
    while (cp >= n2)
    {
        Number subt = n2 << (8 * (cp.length - n2.length + 1));
        int tExp = (8 * (cp.length - n2.length + 1));
        while (subt > cp) {
            subt = (subt >> 1);
            tExp--;
        }
        cp = (cp - subt);
        res = res + (Number((unsigned long long)1) << tExp);
    }
    return std::tuple<Number, Number>(res, cp);
}
// def isEvid(p, a):
//     r = 0
//     d = p - 1
//     while (d % 2) == 0:
//         r = r + 1
//         d = d / 2
//     print(d * (2 ** r))
//     if powermod3(a, d, p) == 1:
//         return false
//     return not any([powermod3(a, (2 ** s) * d, p) == p-1 for s in range(r)])
// std::tuple<Number, Number, Number> extendedEuclid(const Number& a, const Number& b)
// {
//     Number s = 0, oldS = 1,
//             t = 1, oldT = 0,
//             r = b, oldR = a;
//     while (r != 0)
//     {
//         std::tuple<Number, Number> divRes = oldR | r;
//         oldR = r;
//         r = std::get<1>(divRes);
//         Number tempS = s;
//         s = oldS - std::get<0>(divRes) * s;
//         oldS = tempS;
//         Number tempT = t;
//         t = oldT - std::get<0>(divRes) * t;
//         oldT = tempT;
//     }
//     return std::tuple<Number, Number, Number>(oldR, t, s);
// }

Number gcd(const Number& a, const Number& b)
{
    Number r = b, oldR = a;
    while (r != 0)
    {
        std::tuple<Number, Number> divRes = oldR | r;
        oldR = r;
        r = std::get<1>(divRes);
    }
    return oldR;
}

Number modinv(Number a, Number n)
{
    Number t = 0, nt = 1, r = n, nr = (a % n);
    while (nr != 0)
    {
        Number q = r / nr;
        Number temp = nt;
        Number addIt = 0;
        while (addIt + t < (q * nt))
            addIt = addIt + n;
        nt = (t + addIt) - q * nt;
        t = temp;
        temp = nr;
        addIt = 0;
        while (addIt + r < (q * nr))
            addIt = addIt + n;
        nr = (r + addIt) - q * nr;
        r = temp;
    }
    return t;
}

}