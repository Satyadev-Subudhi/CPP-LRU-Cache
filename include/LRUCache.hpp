
#pragma once

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <memory> 

#include "DoublyLinkedList.hpp"

class LRUCache {
private:
    std::size_t capacity_;

    DoublyLinkedList list_;

    // Key -> pointer to corresponding list node
    std::unordered_map<int, Node*> lookup_;

public:
    explicit LRUCache(std::size_t capacity) : capacity_(capacity) {
        if (capacity <= 0) {
            throw std::invalid_argument("Cache capacity must be greater than 0");
        }
    }

    // Disable copying and moving for now
    LRUCache(const LRUCache&) = delete;
    LRUCache& operator=(const LRUCache&) = delete;
    LRUCache(LRUCache&&) = delete;
    LRUCache& operator=(LRUCache&&) = delete;

    std::optional<int> get(int key) {
        auto it = lookup_.find(key);
        if (it != lookup_.end()) {
            Node* node = it->second;
            list_.move_to_front(node);
            return node->value;
        }
        //return std::nullopt if key not found
        return std::nullopt;
    }

    
    void put(int key, int value) {
        auto it = lookup_.find(key);
        
        //already exists, update value and move to front
        if (it != lookup_.end()) { 
            Node* node = it->second;
            node->value = value;
            list_.move_to_front(node);
            return;
        }

        // RAII guard until the node is owned by the list
        auto newnode = std::make_unique<Node>(key, value);

        // This may allocate and throw.
        // If it does, unique_ptr cleans up the node.
        auto [mapIt, inserted] = lookup_.emplace(key, newnode.get());

        // Link node into the list.
        list_.push_front(newnode.get());

        // List now owns the node.
        newnode.release();

        if (list_.size() > capacity_) {
            Node* evicted = list_.pop_back();
            lookup_.erase(evicted->key);
            delete evicted;
        }
    }

    bool erase(int key) {
        auto it = lookup_.find(key);
        if (it != lookup_.end()) {
            Node* node = it->second;
            list_.detach(node);
            lookup_.erase(it->first);
            delete node;
            return true;
        }
        return false;
    }

    std::size_t size() const {
        return list_.size();
    }
};
