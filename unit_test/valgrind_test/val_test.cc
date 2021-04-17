#include <iostream>
#include <memory>

using namespace std;

class X
{
public:
    X()
    {
        cout << "x " << endl;
    }
    ~X()
    {
        cout << "~x" << endl;
    }
};

int main()
{
    X *x = new X();
    // auto x = make_unique<X>();
    return 0;
}