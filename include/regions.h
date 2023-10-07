#pragma once

#include "dynarray.h"
#include <iostream>

struct Rectangle {
    double low_x;
    double low_y;
    double high_x;
    double high_y;
};

class Region {
private:
    dynarray<Rectangle> rects;

public:
    Region() = default;

    void add_rectangle(Rectangle rec) {
        rects.push_back(rec);
    }

    Region& operator+=(const Region& other);
    Region& operator-=(const Region& other);
    Region& operator*=(const Region& other);

    friend std::ostream& operator<<(std::ostream& stream, const Region& self);
};

static Region operator*(const Region& self, const Region& other) {
    Region copy = self;
    copy *= other;
    return copy;
}

static Region operator+(const Region& self, const Region& other) {
    Region copy = self;
    copy += other;
    return copy;
}

static Region operator-(const Region& self, const Region& other) {
    Region copy = self;
    copy -= other;
    return copy;
}
