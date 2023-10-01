#pragma once

#include <cstdlib>
#include <new>
#include <vector>
#include <cassert>
#include <iostream>

#define DISABLE_DYNARRAY_FOR_SAFETY 0


#if DISABLE_DYNARRAY_FOR_SAFETY
template<typename T>
using dynarray = std::vector<T>;
#else

template<typename T>
class dynarray {
private:
    T* _data         = nullptr;
    size_t _capacity = 0;
    size_t _size     = 0;

    void resize(size_t new_size);
    void resize_if_needed();

public:
    dynarray() = default;
    
    explicit dynarray(size_t capacity, const T& value = T()):
        _data(reinterpret_cast<T*>(malloc(capacity * sizeof(T)))),
        _capacity(capacity),
        _size(capacity)
        {
            if (!_data) {
                throw std::bad_alloc();
            }

            for (uint i = 0; i < capacity; ++i) {
                _data[i] = value;
            }
        }

    size_t capacity() const noexcept { return _capacity; }
    size_t size()     const noexcept { return _size; }

    void push_back(const T& elem);
    void push_back(T&& elem);

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
    if (_size == _capacity) {
        resize((_capacity > 1) ? 2 * _capacity : 4);
    } else if (_size > 4 && _size < _capacity / 2) {
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

#endif