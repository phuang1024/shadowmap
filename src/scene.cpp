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


Light::Light(double x, double y, double z, double power, const Vec3& color) {
    loc = Vec3(x, y, z);
    this->power = power;
    this->color = color;
}

Light::Light(const Vec3& loc, double power, const Vec3& color) {
    this->loc = loc;
    this->power = power;
    this->color = color;
}


Scene::Scene() {
    cam_loc = Vec3(0, 0, 0);
    fov = 60;
    _init();
}

Scene::Scene(double cam_x, double cam_y, double cam_z, double pan, double tilt, double fov) {
    cam_loc = Vec3(cam_x, cam_y, cam_z);
    this->cam_pan = pan;
    this->cam_tilt = tilt;
    this->fov = fov;

    _init();
}

Scene::~Scene() {
    for (ShadowMap& map: shadow_maps)
        map.free();
}

void Scene::_init() {
    SHMAP_W = 1024;
    SHMAP_H = 1024;
}

void Scene::add_light(double x, double y, double z, double power, const Vec3& color) {
    lights.push_back(Light(x, y, z, power, color));
}


}  // namespace Shadowmap
