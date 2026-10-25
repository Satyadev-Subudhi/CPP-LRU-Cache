
#include "LRUCache.hpp"

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

#include <list>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <algorithm>

struct Operation {
    bool is_get;
    int key;
    int value;
};

class StandardLRUCache {
public:
    explicit StandardLRUCache(std::size_t capacity)
        : capacity_(capacity) {
        if (capacity_ == 0) {
            throw std::invalid_argument(
                "Capacity must be greater than zero"
            );
        }
    }

    std::optional<int> get(int key) {
        auto it = lookup_.find(key);

        if (it == lookup_.end()) {
            return std::nullopt;
        }

        // Move the accessed entry to the front.
        items_.splice(items_.begin(), items_, it->second);

        return it->second->second;
    }

    void put(int key, int value) {
        auto it = lookup_.find(key);

        // If the key already exists, update it.
        if (it != lookup_.end()) {
            it->second->second = value;
            items_.splice(items_.begin(), items_, it->second);
            return;
        }

        // Insert a new entry at the front.
        items_.emplace_front(key, value);
        lookup_[key] = items_.begin();

        // Evict the least recently used entry if necessary.
        if (items_.size() > capacity_) {
            int lru_key = items_.back().first;
            lookup_.erase(lru_key);
            items_.pop_back();
        }
    }

private:
    using EntryList = std::list<std::pair<int, int>>;

    std::size_t capacity_;
    EntryList items_;
    std::unordered_map<int, EntryList::iterator> lookup_;
};


struct BenchmarkResult {
    double elapsed_ms;
    double operations_per_second;
    double hit_rate;
    std::size_t hits;
    std::size_t get_count;
    std::uint64_t checksum;
};

template <typename Cache>
BenchmarkResult run_benchmark(
    Cache& cache,
    const std::vector<Operation>& operations,
    std::size_t capacity
) {
    // Start both implementations with the same initial contents.
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

    return {
        elapsed_seconds * 1000.0,
        static_cast<double>(operations.size()) / elapsed_seconds,
        100.0 * static_cast<double>(hits) / get_count,
        hits,
        get_count,
        checksum
    };
}


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

    // LRUCache custom_cache(capacity);
    
    constexpr int rounds = 10;

    std::vector<double> custom_times;
    std::vector<double> standard_times;

    BenchmarkResult custom_result{};
    BenchmarkResult standard_result{};

    std::cout << "LRU Cache Benchmark\n";
    std::cout << "Capacity: " << capacity << '\n';
    std::cout << "Operations: " << operations.size() << '\n';
    std::cout << "Rounds: " << rounds << '\n';

    for (int round = 0; round < rounds; ++round) {
        if (round % 2 == 0) {
            // Even rounds: custom cache runs first.
            LRUCache custom_cache(capacity);
            custom_result =
                run_benchmark(custom_cache, operations, capacity);

            StandardLRUCache standard_cache(capacity);
            standard_result =
                run_benchmark(standard_cache, operations, capacity);
        } else {
            // Odd rounds: standard cache runs first.
            StandardLRUCache standard_cache(capacity);
            standard_result =
                run_benchmark(standard_cache, operations, capacity);

            LRUCache custom_cache(capacity);
            custom_result =
                run_benchmark(custom_cache, operations, capacity);
        }

        // Verify that both implementations behave consistently.
        if (custom_result.hits != standard_result.hits ||
            custom_result.checksum != standard_result.checksum) {
            std::cerr << "ERROR: Cache results do not match!\n";
            return 1;
        }

        custom_times.push_back(custom_result.elapsed_ms);
        standard_times.push_back(standard_result.elapsed_ms);

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Round " << round + 1
                  << " | Custom: " << custom_result.elapsed_ms
                  << " ms | std::list: "
                  << standard_result.elapsed_ms << " ms\n";
    }

    // Sort the measurements and calculate their median.
    auto median_ms = [](std::vector<double> times) {
        std::sort(times.begin(), times.end());

        const std::size_t middle = times.size() / 2;

        if (times.size() % 2 != 0) {
            return times[middle];
        }

        return (times[middle - 1] + times[middle]) / 2.0;
    };

    const double custom_median = median_ms(custom_times);
    const double standard_median = median_ms(standard_times);

    const double improvement =
        100.0 * (standard_median - custom_median)
        / standard_median;

    std::cout << "\nSummary\n";
    std::cout << "-------------------\n";
    std::cout << "Custom median: "
              << custom_median << " ms\n";
    std::cout << "std::list median: "
              << standard_median << " ms\n";
    std::cout << "Custom cache elapsed-time reduction: "
              << improvement << "%\n";

    std::cout << "\nBoth implementations produced matching results.\n";

    return 0;

}
