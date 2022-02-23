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

#include "shadowmap.hpp"


namespace Shadowmap {


Vec3::Vec3() {
    x = y = z = 0;
}

Vec3::Vec3(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

Vec3::Vec3(const Vec3& v) {
    x = v.x;
    y = v.y;
    z = v.z;
}

double Vec3::magnitude() const {
    return distance(x, y, z);
}

double Vec3::sum() const {
    return x + y + z;
}

double Vec3::sqsum() const {
    return x*x + y*y + z*z;
}

Vec3 Vec3::unit() const {
    double m = magnitude();
    return Vec3(x/m, y/m, z/m);
}

Vec3 Vec3::add(const Vec3& v) const {
    return Vec3(x+v.x, y+v.y, z+v.z);
}

Vec3 Vec3::sub(const Vec3& v) const {
    return Vec3(x-v.x, y-v.y, z-v.z);
}

Vec3 Vec3::mul(double s) const {
    return Vec3(x*s, y*s, z*s);
}

Vec3 Vec3::mul(const Vec3& v) const {
    return Vec3(x*v.x, y*v.y, z*v.z);
}

Vec3 Vec3::div(double s) const {
    return Vec3(x/s, y/s, z/s);
}

double Vec3::dot(const Vec3& v) const {
    return x*v.x + y*v.y + z*v.z;
}

Vec3 Vec3::cross(const Vec3& v) const {
    double x = this->y*v.z - this->z*v.y;
    double y = this->z*v.x - this->x*v.z;
    double z = this->x*v.y - this->y*v.x;

    return Vec3(x, y, z);
}


Ray::Ray(double x, double y, double z, double dx, double dy, double dz) {
    pt = Vec3(x, y, z);
    dir = Vec3(dx, dy, dz);
}

Ray::Ray(const Vec3& pt, const Vec3& dir) {
    this->pt = pt;
    this->dir = dir;
}


}  // namespace Shadowmap
