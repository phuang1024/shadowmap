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


Light::Light(double x, double y, double z, double power) {
    loc = Vec3(x, y, z);
    this->power = power;
}

Light::Light(const Vec3& loc, double power) {
    this->loc = loc;
    this->power = power;
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
    if (_built) {
        for (ShadowMap& map: shadow_maps)
            map.free();
    }
}

void Scene::_init() {
    _built = false;
    SHMAP_W = 4096;
    SHMAP_H = 2048;
}

void Scene::add_light(double x, double y, double z, double power) {
    lights.push_back(Light(x, y, z, power));
}


}  // namespace Shadowmap
