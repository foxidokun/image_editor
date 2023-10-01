#include <cmath>

#include <iostream>
#include "vector.h"

// ---------------------------------------------------------------------------------------------------------------------
// Operators
// ---------------------------------------------------------------------------------------------------------------------

Vector operator*(const double& scalar, const Vector& vec) {
    Vector copy = vec;
    copy *= scalar;
    return copy;
}

Vector operator*(const Vector& lhs, const Vector& rhs) {
    return Vector(lhs.x * rhs.x, lhs.y * rhs.y 
    #if VECTOR_DIM > 2
    , lhs.z * rhs.z
    #endif
    );
}

Vector operator+(const Vector& lhs, const Vector& rhs) {
    Vector copy = lhs;
    copy += rhs;
    return copy;
}

Vector operator-(const Vector& lhs, const Vector& rhs) {
    Vector copy = lhs;
    copy -= rhs;
    return copy;
}

std::ostream& operator<<(std::ostream& out, const Vector& vec) {
    out << "Vector {x: " << vec.x << "; y: " << vec.y << ";"
        #if VECTOR_DIM > 2
        << " z: " << vec.z 
        #endif
        << "}";
    return out;
}

// ---------------------------------------------------------------------------------------------------------------------
// General purpose methods
// ---------------------------------------------------------------------------------------------------------------------

Vector Vector::norm() const {
    double len = length();
    return *this / len;
}

double dot(const Vector& lhs, const Vector& rhs) {
    return lhs.x * rhs.x +
           lhs.y * rhs.y
        #if VECTOR_DIM > 2
           + lhs.z * rhs.z
        #endif
           ;
}

#if VECTOR_DIM > 2
Vector cross(const Vector& u, const Vector& v) {
    return Vector(u.y * v.z - u.z * v.y,
                  u.z * v.x - u.x * v.z,
                  u.x * v.y - u.y * v.x);
}
#endif

// ---------------------------------------------------------------------------------------------------------------------
// Randomization
// ---------------------------------------------------------------------------------------------------------------------

Vector random_unit_vector() {
    Vector random_vec;

    while (true) {
        random_vec = Vector::random(-1.0, +1.0);
        double len_aq = random_vec.length_square();
        if (len_aq <= 1 && len_aq >= 0.01) { // > 0.01 to protect from NaN / inf / etc
            break;
        }
    }

    random_vec = random_vec.norm();

    return random_vec;
}

// random ray, but not into surface
Vector random_reflection(const Vector& norm) {
    Vector reflected = random_unit_vector();

    if (dot(norm, reflected) > 0.0) {
        return reflected;
    } else {
        return -reflected;
    }
}

// ---------------------------------------------------------------------------------------------------------------------
// Reflecting
// ---------------------------------------------------------------------------------------------------------------------

Vector reflect(const Vector& vec, const Vector& norm) {
    return vec - 2 * dot(vec, norm) * norm;
}

/// @brief get refracted ray
/// @param ray 
/// @param norm normal to surface 
/// @param eta_rel ratio of the refractive indices
/// @return refracted ray
Vector refract(const Vector& ray, const Vector& norm, double eta_rel) {
    double cos_theta = dot(-ray, norm);
    Vector perp = eta_rel * (ray + cos_theta * norm);
    Vector parallel = -sqrt(fabs(1.0 - perp.length_square())) * norm;
    return perp + parallel;
}