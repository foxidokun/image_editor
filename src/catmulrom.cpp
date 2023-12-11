#include "plugin.h"
#include "vector.h"
#include "catmulrom.h"
#include <cmath>

static double calc_next_t(Vector p1, Vector p2, double t) {
    return pow((p2 - p1).length(), 0.5) + t;
}

static double linear_interpol(double a, double b, double t) {
    return a + t * (b - a);
}

Vector cutmullrom_3(double t, plugin::Vec2 p0, plugin::Vec2 p1, plugin::Vec2 p2) {
    double t0 = 0;
    double t1 = calc_next_t(p0, p1, t0);
    double t2 = calc_next_t(p1, p2, t1);

    t = linear_interpol(t1, t2, t);

    plugin::Vec2 a1 = (t1 - t) / (t1 - t0) * p0 + (t - t0) / (t1 - t0) * p1;
    plugin::Vec2 a2 = (t2 - t) / (t2 - t1) * p1 + (t - t1) / (t2 - t1) * p2;

    return (t2 - t) / (t2 - t0) * a1 + (t - t0)/ (t2 - t0) * a2;
}

Vector cutmullrom_2(double t, plugin::Vec2 p0, plugin::Vec2 p1) {
    double t0 = 0;
    double t1 = calc_next_t(p0, p1, t0);

    t = linear_interpol(t0, t1, t);
    return (t1 - t) / (t1 - t0) * p0 + (t - t0) / (t1 - t0) * p1;
}

Vector cutmullrom(double t, plugin::Vec2 p0, plugin::Vec2 p1, plugin::Vec2 p2, plugin::Vec2 p3) {
    double t0 = 0;
    double t1 = calc_next_t(p0, p1, t0);
    double t2 = calc_next_t(p1, p2, t1);
    double t3 = calc_next_t(p2, p3, t2);

    t = linear_interpol(t1, t2, t);

    plugin::Vec2 a1 = (t1 - t) / (t1 - t0) * p0 + (t - t0) / (t1 - t0) * p1;
    plugin::Vec2 a2 = (t2 - t) / (t2 - t1) * p1 + (t - t1) / (t2 - t1) * p2;
    plugin::Vec2 a3 = (t3 - t) / (t3 - t2) * p2 + (t - t2) / (t3 - t2) * p3;

    plugin::Vec2 b1 = (t2 - t) / (t2 - t0) * a1 + (t - t0)/ (t2 - t0) * a2;
    plugin::Vec2 b2 = (t3 - t) / (t3 - t1) * a2 + (t - t1)/ (t3 - t1) * a3;

    return (t2 - t) / (t2 - t1) * b1 + (t - t1) / (t2 - t1) * b2;
}