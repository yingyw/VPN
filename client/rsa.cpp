
#include <iostream>
using namespace std;

#include "rsa.h"


namespace RSA
{
Key::Key()
    : n(0ul),
      e(0ul),
      d(0ul) {}

// keyLength in bytes (divisible by 2)
Key generateKey(size_t keyLength, Number e)
{
    Key ret;
    ret.e = e;
    Number p = generatePrime(keyLength / 2), 
            q = generatePrime(keyLength / 2),
            phi = (p - 1) * (q - 1);
    ret.n =  p * q;
    ret.d = modinv(e, phi);
    // cout << p << endl;
    // cout << q << endl;
    // cout << ret.n << endl;
    // cout << phi << endl;
    // cout << ret.d << endl;
    return ret;
}

// length l in bytes
Number generatePrime(size_t l)
{
    Number res = Number::random(l);
    while (!res.isPrime())
        res = Number::random(l);
    return res;
}

Number encrypt(Key key, Number msg) {
    return modpow(msg, key.e, key.n);
}

Number decrypt(Key key, Number code) {
    return modpow(code, key.d, key.n);
}

}

