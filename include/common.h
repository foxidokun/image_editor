#pragma once

#include <random>
#include "config.h"

// ---------------------------------------------------------------------------------------------------------------------
// Consts
// ---------------------------------------------------------------------------------------------------------------------

const double infinity = std::numeric_limits<double>::infinity();

// ---------------------------------------------------------------------------------------------------------------------
// Random functions
// ---------------------------------------------------------------------------------------------------------------------

// Random real in [0,1).
static inline double random_double() {
#if !ALWAYS_SAME_RANDOM
    std::random_device rd;
    static std::mt19937 generator(rd());
#else
    static std::mt19937 generator;
#endif

    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator);
}

// Random real in [min,max).
static inline double random_double(double min, double max) {
    return min + (max - min) * random_double();
}

// ---------------------------------------------------------------------------------------------------------------------
// Convert functions
// ---------------------------------------------------------------------------------------------------------------------

static inline double degrees_to_radians(double degrees) {
    return degrees * std::numbers::pi / 180.0;
}