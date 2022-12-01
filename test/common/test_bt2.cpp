#include <iostream>
#include <memory>
#include <cstring>
using namespace std;

#include "dynamic_bitset.hpp"

int main()
{

    dynamic_bitset bt(4, true);
    cout << "===========================" << endl;
    cout << "bt:  " << bt.to_string() << endl;
    int len;
    char* p = bt.serialize(&len);
    dynamic_bitset bt5(p);
    cout << "bt5: " << bt5.to_string() << endl;

    delete [] p;

    
    
    
    
    return 0;    
}
