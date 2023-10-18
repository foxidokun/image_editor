#pragma once

#include <random>
#include <SFML/Graphics.hpp>
#include "config.h"

// ---------------------------------------------------------------------------------------------------------------------
// Consts
// ---------------------------------------------------------------------------------------------------------------------

const double infinity = std::numeric_limits<double>::infinity();


namespace global_resources {
    extern const sf::Font    font;

    extern const sf::Texture close_button;
    extern const sf::Texture brush;
    extern const sf::Texture alien;
    extern const sf::Texture polyline;
    extern const sf::Texture rectangle;
    extern const sf::Texture ellipse;
    extern const sf::Texture pick;
    extern const sf::Texture fill;
}

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

static inline double radians_to_degrees(double radians) {
    return radians / std::numbers::pi * 180.0;
}