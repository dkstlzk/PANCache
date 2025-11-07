#include "data/ttl_heap.hpp"
#include <iostream>
#include <thread>
using namespace std;

int main() {
    TTLHeap<string, string> ttlHeap;

    ttlHeap.insert("a", "apple", 2);
    ttlHeap.insert("b", "banana", 5);

    string val;
    if (ttlHeap.get("a", val))
        cout << "Got: " << val << "\n";

    this_thread::sleep_for(chrono::seconds(3));

    if (!ttlHeap.get("a", val))
        cout << "a expired\n";
    if (ttlHeap.get("b", val))
        cout << "b still valid: " << val << "\n";

    return 0;
}
