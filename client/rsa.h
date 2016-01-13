#pragma once

#include "number.h"

namespace RSA
{
class Key
{
public:
    Key();
    Number n, d, e;
};

// keyLength in bytes (divisible by 2)
Key generateKey(size_t keyLength, Number e);

// length l in bytes
Number generatePrime(size_t l);

Number encrypt(Key key, Number msg);

Number decrypt(Key key, Number code);

}


