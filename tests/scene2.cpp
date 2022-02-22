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
#include <iostream>
#include <string>
#include "shadowmap.hpp"

constexpr double PI = Shadowmap::PI;


int main() {
    Shadowmap::Scene scene(0, -15, 3, 0, 0.1, 80);
    scene.bg = 20;
    scene.add_sphere(0, 0, 0, 2);
    scene.add_sphere(0, 0, 3, 1);
    scene.add_sphere(0, 0, -1002, 1000);   // this sphere is like the ground
    scene.add_light(4, -3, 6, 6000);

    Shadowmap::build(scene, true);

    int frame = 0;
    for (double z = 0; z < 2*PI; z += 0.07) {
        std::cerr << "frame " << frame << std::endl;

        std::string fname = "scene2_frame" + std::to_string(frame) + ".img";
        std::ofstream fp(fname);
        Shadowmap::Image img(1280, 720);

        scene.cam_x = 15 * sin(z);
        scene.cam_y = -15 * cos(z);
        scene.cam_pan = -z;
        Shadowmap::render(scene, img, 8);

        img.write(fp);
        frame++;
    }
}
