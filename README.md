# LRU Cache in C++17

A small LRU (Least Recently Used) cache implementation in C++17. This
project explores how a hash map and a doubly linked list work together
to support efficient cache operations, with unit tests and a repeatable
benchmark.

## Features

-   Custom doubly linked list with head and tail sentinel nodes.
-   `std::unordered_map` for mapping keys to list nodes.
-   `get(key)` retrieves a value and marks a successful access as most
    recently used.
-   `put(key, value)` inserts a key or updates an existing value.
-   Automatic eviction of the least recently used entry when capacity is
    exceeded.
-   `erase(key)` removes an entry and reports whether the key existed.
-   Unit tests and an optional AddressSanitizer build.
-   Benchmark against a reference implementation using `std::list`.

## How it works

The cache uses two data structures:

-   **Hash map:** maps each key to its corresponding list node, allowing
    average-case constant-time lookup.
-   **Doubly linked list:** tracks access order. The front contains the
    most recently used (MRU) entry, and the back contains the least
    recently used (LRU) entry.

Operations:

1.  On a successful `get`, the entry moves to the front.
2.  On `put` for an existing key, the value is updated and the entry
    moves to the front.
3.  On `put` for a new key, a node is added to the front. If capacity is
    exceeded, the node at the back is evicted and its key is removed
    from the hash map.
4.  On `erase`, the node is detached from the list and its key is
    removed from the hash map.

The hash map stores pointers to nodes in the list. The cache and list
manage the node lifetime. Copying and moving an `LRUCache` are disabled
to avoid accidental pointer-ownership problems.

## Complexity

| Operation | Average time complexity |
|---|---|
| `get(key)` | `O(1)` |
| `put(key, value)` | `O(1)` |
| `erase(key)` | `O(1)` |

**Space complexity:** `O(capacity)`

## Project structure

``` text
.
├── .devcontainer/
├── include/
│   ├── DoublyLinkedList.hpp
│   └── LRUCache.hpp
├── src/
│   └── main.cpp
├── tests/
│   └── test_lru.cpp
├── benchmark/
│   └── benchmark.cpp
├── CMakeLists.txt
└── README.md
```

## Requirements

-   A C++17-compatible compiler
-   CMake
-   Bash or a similar terminal environment

The commands below are suitable for a Linux environment such as GitHub
Codespaces.

## Build and run

Configure and build a Release version:

``` bash
cmake -S . -B build-release \
  -DCMAKE_BUILD_TYPE=Release \
  -DENABLE_ASAN=OFF

cmake --build build-release --parallel
```

## Run the example program

The program reads the command count and cache capacity first, followed by one command per line. Supported commands include `put <key> <value>` and `get <key>`.

Run this example from the repository root:

```bash
printf '6 2\nput 1 100\nput 2 200\nget 1\nput 3 300\nget 2\nget 3\n' | ./build-release/lru_cache
```

Expected output:

```text
100
null
300
```

In this example, the cache capacity is 2. Calling `get 1` makes key 1 the most recently used entry. When key 3 is inserted, key 2 is evicted because it is the least recently used entry.

Therefore, `get 2` returns `null`, while `get 3` returns `300`.

## Run tests

``` bash
ctest --test-dir build-release --output-on-failure
```

## Run the benchmark

``` bash
./build-release/lru_benchmark
```

The benchmark compares the custom cache with a reference implementation
based on `std::list` and `std::unordered_map`.

Benchmark setup:

-   Cache capacity: `10,000`
-   Operations per round: `1,000,000`
-   Workload: 80% `get`, 20% `put`
-   Keys are generated from a fixed random seed so the workload is
    reproducible.
-   Both implementations receive the same pre-generated operations and
    initial cache contents.
-   The execution order alternates across 10 rounds.
-   The benchmark reports median elapsed times and checks that the
    implementations produce matching hit counts and checksums.

### Example measurement

Across three separate benchmark executions in GitHub Codespaces, the
median of the reported 10-round medians was:

  Implementation            Median elapsed time
  ----------------------- ---------------------
  Custom cache                         34.49 ms
  `std::list` reference                38.62 ms

This corresponds to approximately 10.69% lower elapsed time for the
custom implementation in those runs. Treat this as an environment- and
workload-specific observation, not a claim that a custom linked list is
always faster than `std::list`. Benchmark results can vary with CPU
load, compiler, and environment.

## Optional: AddressSanitizer

To build and run the tests with AddressSanitizer enabled:

``` bash
cmake -S . -B build-asan \
  -DCMAKE_BUILD_TYPE=Debug \
  -DENABLE_ASAN=ON

cmake --build build-asan --parallel

ctest --test-dir build-asan --output-on-failure
```

AddressSanitizer can help detect common memory errors during testing. It
does not prove that a program is free of all memory or logic bugs.

## Scope

This project focuses on the core LRU cache data structures, correctness,
memory ownership, testing, and benchmarking. It is a single-threaded
implementation and does not provide thread safety.
