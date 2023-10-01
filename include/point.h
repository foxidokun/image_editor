#pragma once

#include "vector.h"

#define POINT_DIM 2

class Point {
public:
    double x;
    double y;

#if POINT_DIM > 2
    double z;
#endif

    Point() = default;

#if POINT_DIM > 2
    Point(double x, double y, double z):
#else
    Point(double x, double y):
#endif
        x(x),
        y(y)
#if POINT_DIM > 2
        ,z(z)
#endif
        {};

    Point& operator+=(const Vector& vec);

    Point& operator-=(const Vector& vec);
};

static inline Point operator+(const Point& point, const Vector& vec) {
    return {point.x + vec.x,
            point.y + vec.y
        #if POINT_DIM > 2
            ,point.z + vec.z
        #endif
    };
}

static inline Point operator+(const Vector& vec, const Point& point) {
    return point + vec;
}

static inline Point operator-(const Point& point, const Vector& vec) {
    return {point.x - vec.x,
            point.y - vec.y
        #if POINT_DIM > 2
            ,point.z - vec.z
        #endif
            };
}

static inline Point operator-(const Vector& vec, const Point& point) {
    return point - vec;
}

static inline Vector operator-(const Point& end, const Point& start) {
    return Vector(  end.x - start.x,
                    end.y - start.y
                #if POINT_DIM > 2
                    ,end.z - start.z
                #endif
                    );
}

inline Point& Point::operator+=(const Vector& vec) {
    *this = *this + vec;
    return *this;
}

inline Point& Point::operator-=(const Vector& vec) {
    *this = *this - vec;
    return *this;
}