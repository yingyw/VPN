#include "suite.h"
#include <iostream>
#include <string>
#include "../rsa.h"

using namespace std;

#define rsaText "RSA"

bool testAddition()
{
    RSA::Number n1("\x12", 1);
    RSA::Number n2("\x42", 1);
    RSA::Number n3("\xf0", 1);
    RSA::Number n4("\x10\x10\x10", 3);

    RSA::Number r1 = n1 + n2;
    ASSERTI(r1.length, 1, "");
    ASSERTI(r1.n[0], 0x12 + 0x42, "");
    RSA::Number r2 = n2 + n3;
    ASSERTI(r2.length, 2, "");
    ASSERTI(r2.n[0], (0xf0 + 0x42) % 256, "");
    ASSERTI(r2.n[1], (0xf0 + 0x42) / 256, "");
    RSA::Number r3 = n3 + n4;
    ASSERTI(r3.length, 3, "");
    ASSERTI(r3.n[0], 0x00, "");
    ASSERTI(r3.n[1], 0x11, "");
    ASSERTI(r3.n[2], 0x10, "");
    return true;
}

bool testMult()
{
    unsigned long long tests[7][2] = {{1, 85629}, {0, 56235}, {0, 0}, {52344, 41243}, {312312, 4232}, {12, 5123453434}, {213, 423423}};
    for (int i = 0; i < 7; i++) {
        RSA::Number n0 = tests[i][0];
        RSA::Number n1 = tests[i][1];
        RSA::Number expect = tests[i][0] * tests[i][1];
        RSA::Number act = n0 * n1;
        ASSERT(act, expect, "multiplying");
    }
    return true;
}

bool testInEq()
{
    unsigned long long tests[8][2] = {{1, 85629}, {0, 56235}, {0, 0}, {52344, 41243}, {312312, 4232}, {12, 5123453434}, {213, 423423}, {257, 256*2}};
    for (int i = 0; i < 8; i++) {
        RSA::Number n0 = tests[i][0];
        RSA::Number n1 = tests[i][1];
        ASSERT(n0 < n1, tests[i][0] < tests[i][1], "ineq");
        ASSERT(n0 > n1, tests[i][0] > tests[i][1], "ineq");
    }
    return true;
}

bool testSubt()
{
    unsigned long long tests[8][2] = {{142, 85629}, {0, 56235}, {0, 0}, {52344, 52344}, {4232, 312312}, {12, 5123453434}, {213, 423423}, {255, 256 * 256}};
    for (int i = 0; i < 8; i++) {
        RSA::Number n0 = tests[i][0];
        RSA::Number n1 = tests[i][1];
        ASSERTX(n1 - n0, tests[i][1] - tests[i][0], "subt");
    }
    return true;
}


bool testShift()
{
    long long n = 612472352342341;
    RSA::Number nN = n;
    for (int i = 0; i < 27; i++) {
        RSA::Number sh = nN >> i;
        ASSERTX(sh, n >> i, "shiftDown");
    }
    RSA::Number n2 = 0;
    ASSERTX(n2 >> 10, 0, "shiftDown");

    n = 13;
    RSA::Number nN2 = n;
    for (int i = 0; i < 27; i++) {
        RSA::Number sh = nN2 << i;
        ASSERTX(sh, n << i, "shiftUp");
    }
    ASSERTX(n2 << 4, 0, "shiftUp");
    ASSERTX(n2 << 8, 0, "shiftUp");
    ASSERTX(n2 << 10, 0, "shiftUp");
    return true;
}

bool testEquality()
{
    RSA::Number n1 = (unsigned long long)0;
    RSA::Number n2 = (unsigned long long)0;
    RSA::Number n3 = (unsigned long long)5123;
    RSA::Number n4 = (unsigned long long)5123;
    RSA::Number n5 = (unsigned long long)634;
    ASSERT(n1 == n2, true, "0 == 0");
    ASSERT(n3 == n4, true, "5123 == 5123");
    ASSERT(n1 == n5, false, "0 == 634");
    ASSERT(n3 == n5, false, "5123 == 634");
    return true;
}

bool testMod()
{
    unsigned long long tests[8][2] = {{142, 85629}, {1, 56235}, {2, 8}, {52344, 52344}, {4232, 312312}, {12, 5123453434}, {213, 423423}, {255, 256 * 256}};
    for (int i = 0; i < 8; i++) {
        RSA::Number n0 = tests[i][0];
        RSA::Number n1 = tests[i][1];
        ASSERTX(n1 % n0, tests[i][1] % tests[i][0], "mod");
    }
    return true;
}

bool testDiv()
{
    unsigned long long tests[8][2] = {{142, 85629}, {1, 56235}, {2, 8}, {52344, 52344}, {4232, 312312}, {12, 5123453434}, {213, 423423}, {255, 256 * 256}};
    for (int i = 0; i < 8; i++) {
        RSA::Number n0 = tests[i][0];
        RSA::Number n1 = tests[i][1];
        ASSERTX(n1 / n0, tests[i][1] / tests[i][0], "div");
    }
    return true;
}

bool testModPow()
{
    RSA::Number a = 10;
    RSA::Number ex = 16;
    RSA::Number m = 11;
    RSA::Number mp = RSA::modpow(a, ex, m);
    // cout << mp << endl;
    ASSERT(ex, 16, "modpow const");
    ASSERT(a, 10, "modpow const");

    unsigned long long tests[100][4] = {{530271, 745635, 793893, 513288}, {30108, 773446, 477141, 200187}, {316743, 591101, 627711, 484722}, {565358, 197516, 313566, 310198}, {6318, 715880, 579162, 442560}, {542989, 604227, 85124, 59145}, {401462, 113373, 427678, 123322}, {29254, 409933, 657580, 486424}, {951098, 154998, 593062, 183006}, {255022, 933223, 993186, 613654}, {557965, 573414, 322740, 245125}, {545418, 697069, 628405, 397143}, {68753, 877597, 454818, 263129}, {247016, 299821, 431725, 123991}, {875687, 112485, 363396, 301883}, {168965, 614842, 534968, 500585}, {639970, 841430, 830090, 52650}, {367434, 166987, 500403, 34896}, {830201, 655874, 772626, 535309}, {219051, 722961, 67914, 27783}, {145160, 348806, 36569, 2188}, {887594, 267692, 217061, 129912}, {602025, 569464, 369602, 295031}, {565515, 439154, 371316, 264009}, {886602, 298097, 472712, 176032}, {857631, 207, 146474, 142891}, {953155, 332, 400173, 5983}, {901179, 643654, 600237, 101709}, {943545, 944142, 122306, 22043}, {955081, 187616, 32816, 19713}, {920022, 679024, 550971, 271116}, {267770, 492265, 529771, 10350}, {134239, 532916, 615036, 593269}, {486750, 425210, 556093, 258142}, {77094, 368398, 876998, 206906}, {972838, 56904, 479910, 241486}, {604787, 151777, 210487, 53702}, {315806, 74173, 187467, 117503}, {97848, 915204, 16399, 2837}, {121186, 969216, 690495, 113971}, {499037, 391227, 541439, 479315}, {861585, 711695, 55008, 26289}, {53808, 606538, 884142, 260604}, {275206, 876938, 320892, 315760}, {519692, 201207, 278862, 92924}, {888820, 140850, 376702, 232418}, {670636, 810492, 263302, 212668}, {283831, 889608, 83499, 56629}, {818582, 503570, 332754, 14788}, {785919, 986821, 159490, 55299}, {587797, 258299, 189204, 114577}, {270736, 87950, 346657, 188601}, {263927, 772585, 324830, 249217}, {842239, 421661, 90015, 39364}, {911089, 730516, 939922, 208969}, {519213, 756126, 483429, 313107}, {322704, 155945, 186992, 81008}, {366006, 252049, 50359, 44040}, {87236, 98744, 678789, 68320}, {752475, 216409, 483269, 181761}, {186837, 562100, 566470, 222271}, {397237, 583071, 54074, 49791}, {109644, 766447, 391406, 197608}, {495539, 342219, 668480, 363659}, {956501, 953820, 332269, 17346}, {753456, 419532, 230135, 15736}, {104247, 993577, 936462, 782847}, {37693, 450400, 983384, 331745}, {344706, 690319, 32454, 11718}, {812122, 509423, 747868, 510812}, {572217, 419962, 450502, 83911}, {532278, 934448, 937054, 693288}, {72003, 105527, 661479, 239814}, {8495, 931498, 360002, 47219}, {411928, 150740, 10649, 4130}, {155367, 37396, 363786, 118275}, {835173, 806319, 334044, 296541}, {598394, 91289, 369825, 93029}, {761469, 48111, 659130, 257409}, {198372, 415499, 679613, 548256}, {528748, 949036, 914072, 160768}, {825634, 791793, 565202, 421852}, {226256, 922596, 951638, 367366}, {455287, 62858, 771846, 379849}, {871487, 191274, 207987, 6343}, {295307, 940851, 362729, 190077}, {545476, 907050, 26078, 6774}, {191973, 135644, 194058, 92475}, {877664, 746349, 824749, 686917}, {104465, 770255, 489172, 363249}, {516526, 921037, 945995, 544661}, {681759, 812981, 858780, 836199}, {882678, 489668, 483567, 206256}, {545163, 418636, 346577, 14270}, {750623, 183476, 974130, 718471}, {190941, 806952, 322219, 280743}, {75892, 990594, 961847, 419896}, {849019, 276616, 296650, 245581}, {622733, 545557, 190366, 97791}, {896381, 13020, 235064, 150545}};
    for (int i = 0; i < 100; i++) {
        RSA::Number n0 = tests[i][0];
        RSA::Number n1 = tests[i][1];
        RSA::Number n2 = tests[i][2];
        ASSERTX(RSA::modpow(n0, n1, n2), tests[i][3], "modpow");
    }
    return true;
}

bool testEuclid()
{
    RSA::Number a = 32452346;
    RSA::Number b = 435612;
    ASSERTX((RSA::gcd(a, b)), 2, "euclid");

    a = 41234;
    b = 41234;
    ASSERTX((RSA::gcd(a, b)), 41234, "euclid");

    for (int i = 0; i < 100; i++) {
        RSA::Number n1 = RSA::Number::random((size_t)16);
        RSA::Number n2 = RSA::Number::random((size_t)16);

        if (n2 > n1) {
            RSA::Number temp = n2;
            n2 = n1;
            n1 = temp;
        }
        if (gcd(n1, n2) == 1) {
            RSA::Number mInv = RSA::modinv(n2, n1);
            ASSERTX(((n2 * mInv) % n1), 1, "modinv");
            //cout << ((n2 * mInv) % n1) << endl;
        } else {
            //cout << "-" << endl;
        }
    }
    return true;
}

bool testRSA()
{
    // RSA::Number n = 119493765989936279ul;
    // for (int i = 0; i < 10; i++)
    //     n = RSA::Number::random(4);
    // cout << n << " " << n.isPrime() << endl;
    // RSA::Key key = RSA::generateKey(16, 65537);
    // RSA::Key key;// = RSA::generateKey(16, 65537);
    // key.n = 3233;
    // key.e = 17;
    // key.d = 2753;
    // RSA::Number msg = 65;
    // RSA::Number code = RSA::encrypt(key, msg);
    // RSA::Number orMsg = RSA::decrypt(key, code);
    // cout << code << " " << orMsg << endl;
    for (int i = 0; i < 50; i++)
    {
        RSA::Key key = RSA::generateKey(16, 65537);
        for (int j = 0; j < 4; j++) {
            RSA::Number msg = RSA::Number::random(8);
            RSA::Number code = RSA::encrypt(key, msg);
            ASSERT(RSA::decrypt(key, code), msg, "rsa");
        }
    }
    return true;
}

bool rsaUnit()
{
    // return testSubt();
    // return testAddition() && testMult() && testInEq() && testEquality() && testShift() && 
    //     testSubt() && testMod() && testDiv() && testModPow() && testEuclid() && testRSA();

    cout << RSA::generatePrime(256) << endl;
    
    return true;
}
