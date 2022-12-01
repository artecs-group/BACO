#include <iostream>
#include <memory>
#include <cstring>
using namespace std;

#include "dynamic_bitset.hpp"

int main()
{

    dynamic_bitset bt, bt2, bt3;
    cout << "bt: " << bt.to_string() << endl;

    bt.resize(5);
    cout << "bt: " << bt.to_string() << endl;

    bt.set(1);bt.set(2);
    cout << "bt: " << bt.to_string() << endl;

    //desplazamientos
    bt <<= 1;
    cout << "bt: " << bt.to_string() << endl;
    bt >>= 2;
    cout << "bt: " << bt.to_string() << endl;


    bt2 = bt>>1;
    cout << "bt2:" << bt2.to_string() << endl;
    bt2 = bt<<1;
    cout << "bt2:" << bt2.to_string() << endl;


    cout << "&  :" << (bt2 & bt).to_string() << endl;
    cout << "|  :" << (bt2 | bt).to_string() << endl;
    cout << "^  :" << (bt2 ^ bt).to_string() << endl;
    cout << "~bt:" << (~bt).to_string() << endl;

    bt.resize(7);
    cout << "res:" << bt.to_string() << endl;


    cout << "===================================" << endl;
    
    cout << "bt:           " << bt.to_string() << endl;
    cout << "flip:         " << (bt.flip()).to_string() << endl;
    cout << "test(3):      " << (bt.test(3)) << endl;
    cout << "any:          " << bt.any() << endl;
    cout << "count:        " << bt.count() << endl;
    cout << "find_first:   " << bt.find_first() << endl;
    cout << "find_next(2): " << bt.find_next(2) << endl;

    bt2=dynamic_bitset(8, false);
    cout << "any2: " << bt2.any() << endl;
    cout << "none2: " << bt2.none() << endl;



    cout << "===========================" << endl;
    cout << "bt: " << bt.to_string() << endl;
    
    unique_ptr<char> p = bt.serialize();
    dynamic_bitset bt4(p.get());
    cout << "bt4:" << bt4.to_string() << endl;
    p.release();


    cout << "===========================" << endl;
    bt2.set(1, true);
    bt2.set(5, true);
    cout << "bt2: " << bt2.to_string() << endl;
    
    p = bt2.serialize();
    dynamic_bitset bt5(p.get());
    cout << "bt5: " << bt5.to_string() << endl;
    p.release();

    
    
    
    
    return 0;    
}
