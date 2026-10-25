#include<bits/stdc++.h>
#include "LRUCache.hpp"

int main() {

    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, capacity;
    std::cin >> n >> capacity;

    LRUCache cache(capacity);

    for (int i = 0; i < n; ++i) {
        std::string command;
        std::cin >> command;

        if (command == "put") {
            int key, value;
            std::cin >> key >> value;
            cache.put(key, value);
        } else if (command == "get") {
            int key;
            std::cin >> key;
            auto result = cache.get(key);
            if (result.has_value()) {
                std::cout << result.value() << "\n";
            } else {
                std::cout << "null\n";
            }
        } else if (command == "erase") {
            int key;
            std::cin >> key;
            bool erased = cache.erase(key);
            std::cout << (erased ? "true" : "false") << "\n";
        }
    }

    return 0;
}