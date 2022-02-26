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

#include <algorithm>
#include <cmath>
#include <iostream>
#include "shadowmap.hpp"


namespace Shadowmap {


/**
 * Preprocess the scene.
 * * Face._radius
 */
void preprocess(Scene& scene) {
    for (Mesh& obj: scene.objs) {
        for (Face& face: obj.faces) {
            Face copy = face;

            copy._color = obj.color;

            copy.p1 = copy.p1.add(obj.loc);
            copy.p2 = copy.p2.add(obj.loc);
            copy.p3 = copy.p3.add(obj.loc);

            copy._center = copy.p1.add(copy.p2).add(copy.p3).div(3);
            copy._radius = max(
                distance(copy.p1, copy._center),
                distance(copy.p2, copy._center),
                distance(copy.p3, copy._center)
            );

            scene._faces.push_back(copy);
        }
    }
}


/**
 * Builds a shadow map for light and stores in map.
 * @param index is for verbose
 */
void build_map(Scene& scene, ShadowMap& map, Light& light, int index = 0, bool verbose = false) {
    build_faces(scene, light.loc);

    int last_percent = -1;
    for (int y = 0; y < scene.SHMAP_H; y++) {
        for (int x = 0; x < scene.SHMAP_W; x++) {
            if (verbose) {
                int percent = (y*scene.SHMAP_W + x) * 100 / (scene.SHMAP_H*scene.SHMAP_W);
                if (percent != last_percent) {
                    std::cerr << "\rShadow map " << index << ": " << percent << "%" << std::flush;
                    last_percent = percent;
                }
            }

            double tilt = ((double)y/scene.SHMAP_H - 0.5) * PI;
            double pan = ((double)x/scene.SHMAP_W - 0.5) * PI * 2;

            Vec3 delta(sin(pan)*cos(tilt), cos(pan)*cos(tilt), -sin(tilt));
            Ray ray(light.loc, delta);
            double dist = intersect(scene._faces, ray).dist;

            map.set(x, y, dist);
        }
    }
}


void build(Scene& scene, bool verbose) {
    int start = time();

    preprocess(scene);

    for (int i = 0; i < (int)scene.lights.size(); i++) {
        scene.shadow_maps.push_back(ShadowMap(scene.SHMAP_W, scene.SHMAP_H));
        build_map(scene, scene.shadow_maps[i], scene.lights[i], i, verbose);
    }

    if (verbose) {
        double elapse = (time() - start) / 1000.0;
        std::cerr << "\rBuild finished in " << elapse << " seconds" << std::endl;
    }
}


}  // namespace Shadowmap
