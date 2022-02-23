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


int main() {
    Shadowmap::Scene scene(0, -15, 3, 0, 0.1, 80);
    scene.bg = 20;

    Shadowmap::Mesh m("monkey.stl");
    scene.objs.push_back(m);
    scene.add_light(4, -3, 6, 6000);

    std::ofstream fp("scene3.img");
    Shadowmap::Image img(1920, 1080);

    Shadowmap::build(scene, true);
    Shadowmap::render(scene, img, 8, true);

    img.write(fp);
}
