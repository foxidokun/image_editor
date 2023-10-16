#pragma once
#include <list>

#define DISABLE_LIST_FOR_SECURITY 0

#if DISABLE_LIST_FOR_SECURITY
template<typename T>
using linked_list = std::list<T>;
#else 

template<typename T>
struct list_node {
    T val;
    list_node<T>* next = nullptr;
    list_node<T>* prev = nullptr;

    list_node(const T& elem): val(elem) {}
};

template<typename T>
struct ListIterator 
{
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;

    list_node<T>* cur_ptr;
    int direction; // +1 or -1

    ListIterator(list_node<T>* cur_ptr, int direction = 1): cur_ptr(cur_ptr), direction(direction) {}

    reference operator*() const { return cur_ptr->val; }
    pointer operator->() { return &cur_ptr->val; }

    ListIterator& operator++() {
        if (direction == 1) {
            cur_ptr = cur_ptr->next;
        } else {
            cur_ptr = cur_ptr->prev;
        }
        return *this;
    }  

    ListIterator operator++(int) { ListIterator tmp = *this; ++(*this); return tmp; }

    ListIterator& operator--() {
        if (direction == 1) {
            cur_ptr = cur_ptr->prev;
        } else {
            cur_ptr = cur_ptr->next;
        }
        return *this;
    }  
    ListIterator operator--(int) { ListIterator tmp = *this; --(*this); return tmp; }

    friend bool operator== (const ListIterator& a, const ListIterator& b) { return a.cur_ptr == b.cur_ptr; };
    friend bool operator!= (const ListIterator& a, const ListIterator& b) { return a.cur_ptr != b.cur_ptr; }; 
};

template<typename T>
class linked_list {
private:
    size_t _size        = 0;
    list_node<T> *first = nullptr;
    list_node<T> *last  = nullptr;
public:
    linked_list() = default;
    linked_list(const linked_list& other) = delete;
    linked_list(linked_list&& other) = default;
    linked_list& operator=(const linked_list& other) = delete;
    linked_list& operator=(linked_list&& other) = default;
    
    void push_back(const T& elem);
    size_t size() { return size; }
    ListIterator<T> begin()    const { return ListIterator<T>(first);       }
    ListIterator<T> end()      const { return ListIterator<T>(nullptr);     }
    ListIterator<T> rbegin()   const { return ListIterator<T>(last, -1);    }
    ListIterator<T> rend()     const { return ListIterator<T>(nullptr, -1); }
    ListIterator<T> erase(const ListIterator<T>& iter);
};

template<class T>
void linked_list<T>::push_back(const T& elem) {
    auto node = new list_node<T>(elem);
    node->prev = last;
    if (last) {
        last->next = node;
    }
    last = node;

    if (!first) {
        first = node;
    }

    ++_size;
}

template<class T>
ListIterator<T> linked_list<T>::erase(const ListIterator<T>& iter) {
    if (iter.cur_ptr->next) {
        iter.cur_ptr->next->prev = iter.cur_ptr->prev;
    } else {
        last = iter.cur_ptr->prev;
    }

    if (iter.cur_ptr->prev) {
        iter.cur_ptr->prev->next = iter.cur_ptr->next;
    } else {
        first = iter.cur_ptr->next;
    }

    --_size;

    ListIterator<T> next(nullptr);
    if (iter.direction == 1) {
        if (iter.cur_ptr->next) {
            next = ListIterator<T>(iter.cur_ptr->next, 1);
        } else {
            next = end();
        }
    } else {
        if (iter.cur_ptr->prev) {
            next = ListIterator<T>(iter.cur_ptr->prev, -1);
        } else {
            next = rend();
        }
    }

    delete iter.cur_ptr;

    return next;
}

#endif