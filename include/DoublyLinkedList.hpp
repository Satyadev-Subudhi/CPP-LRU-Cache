
#pragma once

#include <cstddef>
#include <utility>

struct Node
{
    int key = 0;
    int value = 0;

    Node *prev = nullptr;
    Node *next = nullptr;

    Node() = default;

    Node(int k, int v) : key(k), value(v) {}
};

class DoublyLinkedList
{
private:
    Node head_;
    Node tail_;
    std::size_t size_ = 0;

public:
    DoublyLinkedList()
    {
        head_.next = &tail_;
        tail_.prev = &head_;
    }

    // Prevent accidental copying and moving.
    DoublyLinkedList(const DoublyLinkedList &) = delete;
    DoublyLinkedList &operator=(const DoublyLinkedList &) = delete;
    DoublyLinkedList(DoublyLinkedList &&) = delete;
    DoublyLinkedList &operator=(DoublyLinkedList &&) = delete;

    ~DoublyLinkedList()
    {
        clear();
    }

    // Insert an existing, detached node at MRU position.
    void push_front(Node *node)
    {
        Node *temp = head_.next;
        node->next = temp;
        node->prev = &head_;
        head_.next = node;
        temp->prev = node;
        size_++;
    }

    // Unlink a node without deleting it.
    void detach(Node *node)
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;

        node->prev = nullptr;
        node->next = nullptr;

        size_--;
    }

    // Move an existing node to MRU position.
    void move_to_front(Node *node)
    {
        if (head_.next == node)
            return;
        detach(node);
        push_front(node);
    }

    // Return the LRU node, or nullptr if empty.
    // Caller must delete the returned node.
    Node *pop_back()
    {
        if (empty())
            return nullptr;

        Node *node = tail_.prev;
        detach(node);
        return node;
    }

    bool empty() const
    {
        return size_ == 0;
    }

    std::size_t size() const
    {
        return size_;
    }

    void clear()
    {
        // delete all owned data nodes
        Node *curr = head_.next;

        while (curr != &tail_)
        {
            Node *nextNode = curr->next;
            delete curr;
            curr = nextNode;
        }

        head_.next = &tail_;
        tail_.prev = &head_;
        size_ = 0;
    }
};
