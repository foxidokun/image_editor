#pragma once

#include <cstdlib>
#include <new>
#include <vector>
#include <cassert>
#include <iostream>

#define DISABLE_DYNARRAY_FOR_SAFETY 1


#if DISABLE_DYNARRAY_FOR_SAFETY
template<typename T>
using dynarray = std::vector<T>;
#else


template<typename T>
struct _DynArrayIterator 
{
    using iterator_category = std::random_access_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;

    _DynArrayIterator(pointer ptr) : m_ptr(ptr) {}

    reference operator*() const { return *m_ptr; }
    pointer operator->() { return m_ptr; }
    _DynArrayIterator& operator++() { m_ptr++; return *this; }  
    _DynArrayIterator operator++(int) { _DynArrayIterator tmp = *this; ++(*this); return tmp; }
    _DynArrayIterator& operator--() { m_ptr--; return *this; }  
    _DynArrayIterator operator--(int) { _DynArrayIterator tmp = *this; --(*this); return tmp; }

    _DynArrayIterator& operator+=(difference_type n) {
        difference_type m = n;
        if (m >= 0) while (m--) ++(*this);
        else while (m++) --(*this);
        return *this;
    }

    _DynArrayIterator& operator-=(difference_type n) {
        return *this += -n; 
    }

    _DynArrayIterator operator+(difference_type n) {
        _DynArrayIterator temp = *this;
        return temp += n; 
    }

    _DynArrayIterator operator-(difference_type n) {
        _DynArrayIterator temp = *this;
        return temp -= n; 
    }

    reference operator[](difference_type n) {
        return *(*this + n); 
    }

    friend bool operator== (const _DynArrayIterator& a, const _DynArrayIterator& b) { return a.m_ptr == b.m_ptr; };
    friend bool operator<  (const _DynArrayIterator& a, const _DynArrayIterator& b) { return a.m_ptr <  b.m_ptr; };
    friend bool operator>  (const _DynArrayIterator& a, const _DynArrayIterator& b) { return a.m_ptr >  b.m_ptr; };
    friend bool operator<= (const _DynArrayIterator& a, const _DynArrayIterator& b) { return a.m_ptr <= b.m_ptr; };
    friend bool operator>= (const _DynArrayIterator& a, const _DynArrayIterator& b) { return a.m_ptr >= b.m_ptr; };
    friend _DynArrayIterator operator+(const difference_type& a, const _DynArrayIterator& b) { return b + a; };
    friend difference_type operator-(const _DynArrayIterator& a, const _DynArrayIterator& b) { return a.m_ptr - b.m_ptr; };
    friend bool operator!= (const _DynArrayIterator& a, const _DynArrayIterator& b) { return a.m_ptr != b.m_ptr; };  

private:
    pointer m_ptr;
};

template<typename T>
class dynarray {
private:
    T* _data;
    size_t _capacity;
    size_t _size     = 0;

    void resize(size_t new_size);
    void resize_if_needed();

public:
    using Iterator = _DynArrayIterator<T>;

    dynarray(): _data(reinterpret_cast<T*>(malloc(sizeof(T)))), _capacity(1) {};
    
    explicit dynarray(size_t capacity, const T& value = T()):
        _data(reinterpret_cast<T*>(malloc((capacity + 1) * sizeof(T)))),
        _capacity(capacity + 1),
        _size(capacity)
        {
            if (!_data) {
                throw std::bad_alloc();
            }

            for (uint i = 0; i < capacity; ++i) {
                _data[i] = value;
            }
        }

    dynarray(const dynarray& other): _capacity(other._capacity), _size(other._size) {
        _data = reinterpret_cast<T*>(malloc(_capacity * sizeof(T)));

        for (uint i = 0; i < _size; ++i) {
            new(_data + i) T(other._data[i]); 
        }
    }

    dynarray(std::initializer_list<T> list) {
        for (const auto& x: list) {
            push_back(x);
        }
    }

    dynarray& operator=(const dynarray& other) {
        free(_data);

        _size = other._size;
        _capacity = other._capacity;
        _data = reinterpret_cast<T*>(malloc(_capacity * sizeof(T)));
        for (uint i = 0; i < _size; ++i) {
            new(_data + i) T(other._data[i]); 
        }

        return *this;
    }

    size_t capacity() const noexcept { return _capacity; }
    size_t size()     const noexcept { return _size; }

    void push_back(const T& elem);
    void push_back(T&& elem);

    Iterator begin() const { return Iterator(&_data[0]); }
    Iterator end() const { return Iterator(&_data[_size]); }

    void insert(Iterator pos, Iterator other_beg, Iterator other_end);
    void erase(Iterator pos);

    T& operator[](size_t index) noexcept {
        assert(index < _size);

        return _data[index];
    }

    const T& operator[](size_t index) const noexcept {
        assert(index < _size);

        return _data[index];
    }

    void pop_back() noexcept {
        if (_size > 0) {
            _data[_size - 1].~T();
            --_size;
        }
    }
};

template<class T>
void dynarray<T>::resize_if_needed() {
    if (_size + 1 >= _capacity) {
        resize((_capacity > 1) ? 2 * _capacity : 4);
    } else if (_size > 4 && _size + 1 < _capacity / 2) {
        resize(_capacity / 2);
    }
}

template<class T>
void dynarray<T>::resize(size_t new_size) {
    assert (new_size >= _size);

    T* new_data = reinterpret_cast<T*>(malloc(new_size * sizeof(T)));

    for (size_t i = 0; i < _size; ++i) {
        new(new_data + i) T(std::move(_data[i]));
    }

    free(_data);

    _capacity = new_size;
    _data = new_data;
}

template<class T>
void dynarray<T>::push_back(const T& elem) {
    resize_if_needed();

    new(_data + _size) T(elem);

    _size++;
}

template<class T>
void dynarray<T>::push_back(T&& elem) {
    resize_if_needed();

    new(_data + _size) T(std::move(elem));

    _size++;
}

template<class T>
void dynarray<T>::insert(Iterator pos, Iterator other_beg, Iterator other_end) {
    if (pos != end()) {
        std::cerr << "Insert not to the end is not supported yet\n";
        abort();
    }

    while (other_beg != other_end) {
        push_back(*other_beg);
        ++other_beg;
    }
}

template<class T>
void dynarray<T>::erase(Iterator _pos) {
    size_t pos = _pos - begin();
    --_size;

    for (uint i = pos; i < _size; ++i) {
        _data[i] = _data[i+1];
    }

}

#endif