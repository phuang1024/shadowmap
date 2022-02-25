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

#include <iostream>
#include "shadowmap.hpp"

using Shadowmap::Vec3;


int main() {
    Shadowmap::Scene scene(0, -9, 4, 0, 0.3, 70);
    scene.bg = 20;

    scene.objs.push_back(Shadowmap::Mesh(Vec3(0, 0, 2), "monkey.stl"));
    scene.objs.push_back(Shadowmap::Mesh(Vec3(0, 0, 0), "plane.stl"));

    scene.add_light(4, -5, 6, 6000);
    scene.add_light(-5, -3, 3, 1000);

    std::ofstream fp("scene1.img");
    Shadowmap::Image img(1280, 720);

    Shadowmap::build(scene, true);
    Shadowmap::render(scene, img, 1, true);

    img.write(fp);
}
