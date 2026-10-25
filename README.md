
# C++ LRU Cache

A Least Recently Used (LRU) Cache implemented from scratch in C++17 using a custom doubly linked list and an unordered map.

## Overview

The cache stores key-value pairs with a fixed capacity. When the cache is full, it automatically evicts the least recently used entry.

## Design

- **Hash map:** Maps keys to node pointers for average O(1) lookups.
- **Doubly linked list:** Maintains entries in usage order, with the most recently used entry at the front and the least recently used entry at the back.
- **Memory management:** Uses explicit node ownership and RAII to reduce the risk of memory errors.

## Features

- `get(key)`: Retrieves a value and marks the entry as recently used.
- `put(key, value)`: Inserts or updates an entry.
- `erase(key)`: Removes an entry by key.
- Automatic eviction when capacity is exceeded.
- Input validation for zero capacity.
- Unit tests for core operations and edge cases.

## Complexity

| Operation | Average Time Complexity |
|---|---|
| `get` | O(1) |
| `put` | O(1) |
| `erase` | O(1) |

These average-case bounds rely on the hash map's operations taking average O(1) time.

## Build and Run

Requires CMake and a C++17-compatible compiler.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Run the application:

- Linux: `./build/lru_cache`
- Windows PowerShell: `.\build\lru_cache.exe`

## Run Tests

```bash
ctest --test-dir build --output-on-failure
```

## Project Goals

This project focuses on more than implementing an algorithm. It explores pointer ownership, memory management, exception safety, testing, and performance measurement in C++.
