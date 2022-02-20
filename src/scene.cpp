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

#include <cmath>
#include "shadowmap.hpp"


namespace Shadowmap {


Sphere::Sphere(double x, double y, double z, double rad) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->rad = rad;
}


Light::Light(double x, double y, double z, double power) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->power = power;
}


Scene::Scene() {
    cam_x = cam_y = cam_z = 0;
    fov = 60;
    _init();
}

Scene::Scene(double cam_x, double cam_y, double cam_z, double fov) {
    this->cam_x = cam_x;
    this->cam_y = cam_y;
    this->cam_z = cam_z;
    this->fov = fov;

    _init();
}

Scene::~Scene() {
    if (_built) {
        for (ShadowMap& map: shadow_maps)
            map.free();
    }
}

void Scene::_init() {
    _built = false;
    SHMAP_W = 4096;
    SHMAP_H = 2048;
    jitter = 10;
}

void Scene::add_sphere(double x, double y, double z, double rad) {
    objs.push_back(Sphere(x, y, z, rad));
}

void Scene::add_light(double x, double y, double z, double power) {
    lights.push_back(Light(x, y, z, power));
}


}  // namespace Shadowmap
