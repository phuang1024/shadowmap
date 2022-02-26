//
//  Shadowmap
//  Shadow map rendering engine.
//  Copyright  Patrick Huang  2022
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

#include "linalg.hpp"


namespace Shadowmap {


inline double min(double a, double b, double c) {
    return std::min(a, std::min(b, c));
}

inline double max(double a, double b, double c) {
    return std::max(a, std::max(b, c));
}

inline double distance(double dx, double dy, double dz) {
    return sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));
}

inline double distance(double dx, double dy) {
    return sqrt(pow(dx, 2) + pow(dy, 2));
}

inline double distance(Vec3& v1, Vec3& v2) {
    return sqrt(pow(v1.x-v2.x, 2) + pow(v1.y-v2.y, 2) + pow(v1.z-v2.z, 2));
}

/**
 * Clamp value to range.
 */
inline int bounds(int v, int min, int max) {
    return std::min(std::max(v, min), max);
}

inline double dbounds(double v, double min, double max) {
    return std::min(std::max(v, min), max);
}

/**
 * True if positive.
 * False if 0 or negative.
 */
inline bool sign(double v) {
    return v >= 0;
}

/**
 * Random from 0 to 1
 */
inline double randd() {
    return (rand() % (int)1e9) / 1e9;
}

/**
 * Milliseconds since epoch.
 */
inline int time() {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    int elapse = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    return elapse;
}


/**
 * Information about an intersection between a ray and a face.
 */
struct Intersect {
    double dist;  // distance from ray origin to intersection
    Vec3 normal;  // normal of the face at intersection
    Vec3 pos;     // position of the intersection
    Vec3 color;   // color of the face at intersection
};


}  // namespace Shadowmap
