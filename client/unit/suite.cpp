#include "suite.h"
#include "rsa.cpp"
#include <iostream>

using namespace std;

int main()
{
    std::srand(0);
    std::vector<TestUnit> suiteTests;
    suiteTests.push_back({rsaUnit, rsaText});

    int passed = 0;
    for (auto test : suiteTests)
    {
        bool success = test.fn();
        if (success)
            passed++;
        else 
        {
            cout << "--------------------" << endl;
            cout << "Running test: " << test.name << endl;
            cout << "Test failed" << endl;
            cout << "--------------------" << endl;
        }
    }
    if (passed == suiteTests.size())
        cout << "All test passed!" << endl;
    else
        cout << passed << "/" << suiteTests.size() << " test passed." << endl;
    return 0;
}
