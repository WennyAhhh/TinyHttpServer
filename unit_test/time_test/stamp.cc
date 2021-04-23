#include <iostream>
#include <memory>
#include "stamp.h"

using namespace std;

class Foo
{
public:
    void get_context()
    {
        cout << "xiao mi" << endl;
    }
    ~Foo()
    {
        cout << "~Foo" << endl;
    }
};

int main()
{
    const std::shared_ptr<Foo> foo(make_shared<Foo>());
    Stamp<Foo> s(foo, std::bind(&Foo::get_context, foo));
    return 0;
}
