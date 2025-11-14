#include "analytics/topk.hpp"
#include <iostream>
#include <cassert>
using namespace std;

int main() {
    TopK tk;

    unordered_map<string,int> freq = {
        {"a",5}, {"b",1}, {"c",10}, {"d",3}
    };

    auto res = tk.computeTopK(freq, 2);

    assert(res.size() == 2);
    assert(res[0].first == "c");
    assert(res[1].first == "a");

    cout << "TopK tests passed!\n";
    return 0;
}