#pragma once

#include "plugin.h"
#include "vector.h"

Vector cutmullrom_3(double t, plugin::Vec2 p0, plugin::Vec2 p1, plugin::Vec2 p2);
Vector cutmullrom_2(double t, plugin::Vec2 p0, plugin::Vec2 p1);
Vector cutmullrom(double t, plugin::Vec2 p0, plugin::Vec2 p1, plugin::Vec2 p2, plugin::Vec2 p3);