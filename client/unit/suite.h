#ifndef SUITE_
#define SUITE_

#include <functional>
#include <vector>
#include <string>

typedef std::function<bool()> TestFn;

struct TestUnit 
{
    TestFn fn;
    std::string name;
};

#define ASSERT(a, b, s) if (a != b) { cout << (s)<< " expected: " << (b) << " actual: " << (a) << endl; return false; }
#define ASSERTI(a, b, s) if (a != b) { cout << (s)<< " expected: " << (int)(b) << " actual: " << (int)(a) << endl; return false; }
#define ASSERTX(a, b, s) if (a != b) { cout << (s)<< " expected: " << hex << (int)(b) << " actual: " << (a) << endl; return false; }

#endif // SUITE_
