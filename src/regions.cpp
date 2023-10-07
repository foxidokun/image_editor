#include "regions.h"
#include <format>

static bool has_intersect(const Rectangle& one, const Rectangle& other) {
    bool has_common_x = false;
    has_common_x |= (other.low_x < one.low_x  && other.high_x > one.low_x);
    has_common_x |= (other.low_x < one.high_x && other.high_x > one.high_x);
    has_common_x |= (other.low_x >= one.low_x  && other.high_x <= one.high_x);
    bool has_common_y = false;
    has_common_y |= (other.low_y < one.low_y  && other.high_y > one.low_y);
    has_common_y |= (other.low_y < one.high_y && other.high_y > one.high_y);
    has_common_y |= (other.low_y >= one.low_y  && other.high_y <= one.high_y);

    return has_common_x && has_common_y;
}

Region& Region::operator+=(const Region& other) {
    Region tmp = other - *this;
    rects.insert(rects.end(), tmp.rects.begin(), tmp.rects.end());
    return *this;
}

Region& Region::operator-=(const Region& other) {
    for (uint i = 0; i < rects.size();) {
        bool increment_i = true;
        for (uint j = 0; j < other.rects.size(); ++j) {
            if (has_intersect(rects[i], other.rects[j])) {
                const Rectangle& first_rect = rects[i];
                const Rectangle& second_rect = other.rects[j];
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
                    rects[i] = added[number];
                }

                for (uint i = number; i > 0; --i) {
                    rects.push_back(added[i - 1]);
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

    return *this;
}

Region& Region::operator*=(const Region& other) {
    *this -= (*this - other);
    return *this;
}

std::ostream& operator<<(std::ostream& stream, const Region& self) {
    stream << "Region {\n";

    stream.setf(std::ios_base::fixed, std::ios_base::floatfield);
    stream.precision(2);

    for (uint i = 0; i < self.rects.size(); ++i) {
        stream << std::format("\tRect #{} {{low_x: {:.2} low_y: {:.2} high_x: {:.2} high_y: {:.2} }}\n", 
            i,
            self.rects[i].low_x,
            self.rects[i].low_y,
            self.rects[i].high_x,
            self.rects[i].high_y
            );
    }
    stream << "}\n";

    return stream;
}