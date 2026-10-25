
#include "LRUCache.hpp"

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

struct Operation {
    bool is_get;
    int key;
    int value;
};

int main() {
    constexpr std::size_t capacity = 10'000;
    constexpr std::size_t num_operations = 1'000'000;
    constexpr int key_range = 20'000;

    // Generate a reproducible workload before timing.
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> key_dist(0, key_range - 1);

    std::vector<Operation> operations;
    operations.reserve(num_operations);

    for (std::size_t i = 0; i < num_operations; ++i) {
        bool is_get = (i % 5 != 0); // 80% gets, 20% puts

        operations.push_back({
            is_get,
            key_dist(rng),
            static_cast<int>(i)
        });
    }

    LRUCache cache(capacity);

    // Pre-fill the cache before timing.
    for (int key = 0; key < static_cast<int>(capacity); ++key) {
        cache.put(key, key * 10);
    }

    std::uint64_t checksum = 0;
    std::size_t get_count = 0;
    std::size_t hits = 0;

    const auto start = std::chrono::steady_clock::now();

    for (const auto& op : operations) {
        if (op.is_get) {
            ++get_count;

            auto result = cache.get(op.key);

            if (result.has_value()) {
                ++hits;
                checksum += static_cast<std::uint64_t>(*result);
            }
        } else {
            cache.put(op.key, op.value);
        }
    }

    const auto end = std::chrono::steady_clock::now();

    const double elapsed_seconds =
        std::chrono::duration<double>(end - start).count();

    const double operations_per_second =
        static_cast<double>(num_operations) / elapsed_seconds;

    const double hit_rate =
        100.0 * static_cast<double>(hits) / get_count;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "LRU Cache Benchmark\n";
    std::cout << "-------------------\n";
    std::cout << "Capacity: " << capacity << '\n';
    std::cout << "Operations: " << num_operations << '\n';
    std::cout << "Workload: 80% get, 20% put\n";
    std::cout << "Elapsed time: "
              << elapsed_seconds * 1000.0 << " ms\n";
    std::cout << "Throughput: "
              << operations_per_second << " operations/sec\n";
    std::cout << "Get hit rate: " << hit_rate << "%\n";
    std::cout << "Checksum: " << checksum << '\n';

    return 0;
}
