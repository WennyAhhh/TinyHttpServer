#include <iostream>
#include "buffer.h"

using namespace std;

int main()
{
    Buffer buff;
    const string str("Set the minimum version of CMake that can be used");
    cout << str << "  " << endl;
    buff.append(str);
    const string str2 = buff.RetrieveAsString(4);
    buff.shrink();
    cout << str2 << "  " << endl;
    const string addstr("  to find xxxx");
    buff.append(addstr);
    buff.prepend("xx", 2);
    const string str3 = buff.RetrieveAllAsString();
    cout << str3 << endl;
    Buffer swapbuff;
    swapbuff.append("先有圣人后有天， 我惠美如画中仙");
    // const string str4 = swapbuff.RetrieveAllAsString();
    // cout << str4 << endl;
    buff.swap(swapbuff);
    // swapbuff.swap(buff);
    const string str5 = buff.RetrieveAllAsString();
    cout << str5 << endl;
    return 0;
}