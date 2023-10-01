#pragma once

#include "common.h"

class Interval {
public:
    double min, max;

    Interval():
        min(+infinity),
        max(-infinity)
        {}

    Interval(double min, double max):
        min(min),
        max(max)
        {}

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    double clamp(double x) const {
        if (x < min) { return min; }
        if (x > max) { return max; }

        return x;
    }
};
