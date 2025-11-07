#include <iostream>
using namespace std;

int main() {
    cout << "PANCache Core Build Successful\n" << endl;
    cout << "Use the following commands to test modules:" << std::endl;
    cout << "  make test_hashmap     → Test HashMap module" << std::endl;
    cout << "  make test_lru         → Test LRUCache module" << std::endl;
    cout << "  make test_ttl         → Test TTLHeap module" << std::endl;
    cout << "  make test_integration → Run full integration test" << std::endl;
    return 0;
}
