#pragma once

#include "common.h"
#include <cmath>

#define VECTOR_DIM 2

class Vector {
public:
    // ############
    // Fields
    // ############
    double x = 0.0;
    double y = 0.0;
#if VECTOR_DIM > 2
    double z = 0.0;
#endif

    // ############
    // (Con/De)structors
    // ############
    Vector() = default;

#if VECTOR_DIM > 2
    Vector(double x, double y, double z):
        x(x),
        y(y),
        z(z)
        {}

    Vector(const double point[3]):
        x(point[0]),
        y(point[1]),
        z(point[2])
        {}
#else 
    Vector(double x, double y):
        x(x),
        y(y)
        {}

    Vector(const double point[2]):
        x(point[0]),
        y(point[1])
        {}
#endif

    static Vector random() {
        return Vector(
            random_double(),
            random_double()
            #if VECTOR_DIM > 2
            ,random_double()
            #endif
        );
    }

    static Vector random(double min, double max) {
    #if VECTOR_DIM > 2
        return Vector(random_double(min, max), random_double(min, max), random_double(min, max));
    #else
        return Vector(random_double(min, max), random_double(min, max));
    #endif
    }

    // ############
    // Operators
    // ############

    Vector operator*(const double& scalar) const {
#if VECTOR_DIM > 2
        return Vector(x * scalar, y * scalar, z * scalar);
#else
        return Vector(x * scalar, y * scalar);
#endif
    }

    Vector operator/(const double& scalar) const {
#if VECTOR_DIM > 2
        return Vector(x / scalar, y / scalar, z / scalar);
#else
        return Vector(x / scalar, y / scalar);
#endif
    }

    Vector& operator*=(const double& scalar) {
        *this = *this * scalar;
        return *this;
    }

    Vector& operator/=(const double& scalar) {
        *this = *this / scalar;
        return *this;
    }

    Vector operator-() const {
#if VECTOR_DIM > 2
        Vector res(-x, -y, -z);
#else
        Vector res(-x, -y);
#endif
        return res;
    }

    Vector& operator+=(const Vector& vec) {
        x += vec.x;
        y += vec.y;
#if VECTOR_DIM > 2
        z += vec.z;
#endif

        return *this;
    }

    Vector& operator-=(const Vector& vec) {
        x -= vec.x;
        y -= vec.y;
#if VECTOR_DIM > 2
        z -= vec.z;
#endif

        return *this;
    }

    // ###############
    // General purpose methods
    // ##############

    double length() const {
        return sqrt(length_square());
    }

    double length_square() const {
        return x * x + y * y 
        #if VECTOR_DIM > 2
            + z * z
        #endif
            ;
    }

    bool near_zero() const {
        return (fabs(x) < NEAR_ZERO_VEC_1DLEN) && (fabs(y) < NEAR_ZERO_VEC_1DLEN)
            #if VECTOR_DIM > 2
            && (fabs(z) < NEAR_ZERO_VEC_1DLEN)
            #endif
            ;
    }

    Vector norm() const;
};

Vector operator*(const double& scalar, const Vector& vec);
Vector operator*(const Vector& lhs, const Vector& vec);
Vector operator+(const Vector& lhs, const Vector& rhs);
Vector operator-(const Vector& lhs, const Vector& rhs);

double dot(const Vector& lhs, const Vector& rhs);

#if VECTOR_DIM > 2
Vector cross(const Vector& lhs, const Vector& rhs);
#endif

std::ostream& operator<<(std::ostream& out, const Vector& vec);
