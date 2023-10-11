#pragma once

#include "dynarray.h"
#include <iostream>
#include "vector.h"

struct Rectangle {
    double low_x;
    double low_y;
    double high_x;
    double high_y;
};

class Region {
private:
    dynarray<Rectangle> _rects;

public:
    Region() = default;

    void add_rectangle(Rectangle rec) {
        _rects.push_back(rec);
    }
    
    void optimize();

    Region& operator+=(const Region& other);
    Region& operator-=(const Region& other);
    Region& operator*=(const Region& other);

    void shift(const Vector &vec);

    const dynarray<Rectangle>& rects() const {return _rects;}

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

static std::ostream& operator<<(std::ostream& stream, const Rectangle& self) {
    stream << "{{low_x: {"  << self.low_x  << "} low_y: {"  << self.low_y;
    stream << "} high_x: {" << self.high_x << "} high_y: {" << self.high_y <<"} }}";

    return stream;
}
