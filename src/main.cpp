#include "../include/data/hashmap.hpp"
#include <iostream>

int main(){
    HashMap<string, int> cache;
    cache.insert("alpha", 1);
    cache.insert("beta", 2);
    cache.insert("gamma", 3);

    if (auto val= cache.get("beta"))
        cout << "beta = " << *val << "\n";
    else
        cout << "beta not found\n";

    cache.erase("beta");
    cout << "After erase, contains beta? " 
              << (cache.contains("beta") ? "Yes": "No") << "\n";

    cout << "Size: " << cache.size() << endl;
    return 0;
}
