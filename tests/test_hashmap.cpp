#include "data/hashmap.hpp"
#include <iostream>
#include <cassert>
using namespace std;

int main(){
    HashMap<string,int> map;

    map.insert("apple",10);
    map.insert("banana", 20);
    assert( map.get("apple").value() ==10 );
    assert( map.get("banana").value()== 20 );

    map.insert("apple", 50);
    assert( map.get("apple").value() == 50 );

    assert( map.contains("banana") );
    assert( !map.contains("grape") );

    assert( map.erase("banana") );
    assert( !map.erase("banana") );
    assert( !map.contains("banana") );

    for(int i=0; i<100; i++)
        map.insert("key"+ to_string(i), i);

    assert(map.size() >=100);

    cout<<"All HashMap tests passed!\n"<<endl;
    return 0;
}
