#include "regions.h"
#include <format>

static bool has_intersect(const Rectangle& one, const Rectangle& other);

enum class common_edge {
    NO_COMMON,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM
};

static common_edge has_common_edge(const Rectangle& one, const Rectangle& other);

static inline bool eq(double lhs, double rhs) { return fabs(lhs - rhs) < 0.1; } 

Region& Region::operator+=(const Region& other) {
    Region tmp = other - *this;
    _rects.insert(_rects.end(), tmp._rects.begin(), tmp._rects.end());

    optimize();

    return *this;
}

void Region::optimize() {
    for (uint i = 0; i < _rects.size();) {
        bool increment_i = true;
        for (uint j = 0; j < i; ++j) {
            auto common = has_common_edge(_rects[i], _rects[j]);
            if (common != common_edge::NO_COMMON) {
                switch (common) {
                    case (common_edge::BOTTOM):
                        _rects[i].low_y = _rects[j].low_y;
                        break;
                    case (common_edge::TOP):
                        _rects[i].high_y = _rects[j].high_y;
                        break;
                    case (common_edge::LEFT):
                        _rects[i].low_x = _rects[j].low_x;
                        break;
                    case (common_edge::RIGHT):
                        _rects[i].high_x = _rects[j].high_x;
                        break;
                }

                _rects.erase(_rects.begin() + j);

                i = 0;
                j = 0;
                increment_i = false;
                break;
            }
        }

        if (increment_i) {
            ++i;
        }
    }
}

Region& Region::operator-=(const Region& other) {
    for (uint i = 0; i < _rects.size();) {
        bool increment_i = true;
        for (uint j = 0; j < other._rects.size(); ++j) {
            if (has_intersect(_rects[i], other._rects[j])) {
                const Rectangle& first_rect = _rects[i];
                const Rectangle& second_rect = other._rects[j];
                Rectangle added[4] = {};
                uint number = 0;

                if (first_rect.high_y > second_rect.high_y) {
                    added[number].high_y = first_rect.high_y;
                    added[number].low_y  = second_rect.high_y;
                    added[number].low_x  = std::max(first_rect.low_x, second_rect.low_x);
                    added[number].high_x = std::min(first_rect.high_x, second_rect.high_x);
                    number++;
                }

                if (first_rect.low_y < second_rect.low_y) {
                    added[number].high_y = second_rect.low_y;
                    added[number].low_y  = first_rect.low_y;
                    added[number].low_x  = std::max(first_rect.low_x, second_rect.low_x);
                    added[number].high_x = std::min(first_rect.high_x, second_rect.high_x);
                    number++;
                }

                if (first_rect.low_x < second_rect.low_x) {
                    added[number].high_y = first_rect.high_y;
                    added[number].low_y  = first_rect.low_y;
                    added[number].low_x  = first_rect.low_x;
                    added[number].high_x = second_rect.low_x;
                    number++;
                
                }
                if (first_rect.high_x > second_rect.high_x) {
                    added[number].high_y = first_rect.high_y;
                    added[number].low_y  = first_rect.low_y;
                    added[number].low_x  = second_rect.high_x;
                    added[number].high_x = first_rect.high_x;
                    number++;
                }

                if (number > 0) {
                    number--;
                    _rects[i] = added[number];
                } else {
                    _rects.erase(_rects.begin() + i);
                }

                for (uint i = number; i > 0; --i) {
                    _rects.push_back(added[i - 1]);
                }

                i = 0;
                j = 0;
                increment_i = false;
                break;
            }
        }

        if (increment_i) {
            ++i;
        }
    }

    optimize();

    return *this;
}

Region& Region::operator*=(const Region& other) {
    *this -= (*this - other);

    optimize();

    return *this;
}

std::ostream& operator<<(std::ostream& stream, const Region& self) {
    stream << "Region {\n";

    stream.setf(std::ios_base::fixed, std::ios_base::floatfield);
    // stream.precision(10);

    for (uint i = 0; i < self._rects.size(); ++i) {
        stream << std::format("\tRect #{} {{low_x: {} low_y: {} high_x: {} high_y: {} }}\n", 
           i,
           self._rects[i].low_x,
           self._rects[i].low_y,
           self._rects[i].high_x,
           self._rects[i].high_y
           );
    }
    stream << "}\n";

    return stream;
}

void Region::shift(const Vector &vec) {
    for (uint i = 0; i < _rects.size(); ++i) {
        _rects[i].low_x  += vec.x;
        _rects[i].high_x += vec.x;
        _rects[i].low_y  += vec.y;
        _rects[i].high_y += vec.y;
    }
}


// ---------------------------------------------------------------------------------------------------------------------
// Static 
// ---------------------------------------------------------------------------------------------------------------------

static bool has_intersect(const Rectangle& one, const Rectangle& other) {
    bool has_common_x = false;
    has_common_x |= (other.low_x < one.low_x  && other.high_x > one.low_x);
    has_common_x |= (other.low_x < one.high_x && other.high_x > one.high_x);
    has_common_x |= (other.low_x >= one.low_x && other.high_x <= one.high_x);
    bool has_common_y = false;
    has_common_y |= (other.low_y < one.low_y  && other.high_y > one.low_y);
    has_common_y |= (other.low_y < one.high_y && other.high_y > one.high_y);
    has_common_y |= (other.low_y >= one.low_y && other.high_y <= one.high_y);

    return has_common_x && has_common_y;
}

static common_edge has_common_edge(const Rectangle& one, const Rectangle& other) {
    if (eq(one.high_x, other.low_x)) {
        if (eq(one.low_y, other.low_y) && eq(one.high_y, other.high_y)) {
            return common_edge::RIGHT;
        }
    }

    if (eq(one.low_x, other.high_x)) {
        if (eq(one.low_y, other.low_y) && eq(one.high_y, other.high_y)) {
            return common_edge::LEFT;
        }
    }

    if (eq(one.high_y, other.low_y)) {
        if (eq(one.low_x, other.low_x) && eq(one.high_x, other.high_x)) {
            return common_edge::TOP;
        }
    }

    if (eq(one.low_y, other.high_y)) {
        if (eq(one.low_x, other.low_x) && eq(one.high_x, other.high_x)) {
            return common_edge::BOTTOM;
        }
    }

    return common_edge::NO_COMMON;
}