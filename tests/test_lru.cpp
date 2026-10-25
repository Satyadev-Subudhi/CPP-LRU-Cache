
#include <cassert>
#include <iostream>
#include <stdexcept>

#include "LRUCache.hpp"

void test_basic_operations() {
    LRUCache cache(2);

    assert(cache.size() == 0);
    assert(!cache.get(99).has_value());

    cache.put(1, 100);
    cache.put(2, 200);

    assert(cache.get(1) == 100);
    assert(cache.size() == 2);
}

void test_eviction() {
    LRUCache cache(2);

    cache.put(1, 100);
    cache.put(2, 200);

    cache.get(1);       // 1 becomes MRU
    cache.put(3, 300);  // Should evict 2

    assert(!cache.get(2).has_value());
    assert(cache.get(1) == 100);
    assert(cache.get(3) == 300);
}

void test_update() {
    LRUCache cache(2);

    cache.put(1, 100);
    cache.put(2, 200);
    cache.put(1, 999);

    assert(cache.get(1) == 999);
    assert(cache.size() == 2);

    cache.put(3, 300);
    assert(!cache.get(2).has_value());
}

void test_erase() {
    LRUCache cache(2);

    cache.put(1, 100);
    cache.put(2, 200);

    assert(cache.erase(1) == true);
    assert(!cache.get(1).has_value());
    assert(cache.size() == 1);

    assert(cache.erase(3) == false); // Non-existent key
    cache.put(3,300);

    assert(cache.size() == 2);
    assert(cache.get(3) == 300);
}

void test_capacity_one() {
    LRUCache cache(1);

    cache.put(1, 100);
    assert(cache.get(1) == 100);

    cache.put(2, 200); // Should evict 1
    assert(!cache.get(1).has_value());
    assert(cache.get(2) == 200);
}

void test_invalid_capacity() {
    bool exceptionThrown = false;
    try {
        LRUCache cache(0);
    }
    catch (const std::invalid_argument&) {
        exceptionThrown = true;
    }

    assert(exceptionThrown);
}

void test_repeated_evictions() {
    LRUCache cache(3);

    for (int i = 0; i < 10000; ++i) {
        cache.put(i, i * 10);
        assert(cache.size() <= 3);
    }

    assert(cache.size() == 3);

    // Only the last three keys should remain
    assert(!cache.get(9996).has_value());
    assert(cache.get(9997) == 99970);
    assert(cache.get(9998) == 99980);
    assert(cache.get(9999) == 99990);
}

int main() {
    test_basic_operations();
    test_eviction();
    test_update();
    test_erase();
    test_capacity_one();
    test_invalid_capacity();
    test_repeated_evictions();

    std::cout << "All tests passed!\n";
}
