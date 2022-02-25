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
#include "shadowmap.hpp"


namespace Shadowmap {


/**
 * Signed volume.
 * From https://stackoverflow.com/q/42740765/
 */
double signed_volume(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d) {
    return b.sub(a).cross(c.sub(a)).dot(d.sub(a)) / 6.0;
}

/**
 * Formula from https://stackoverflow.com/q/42740765/
 */
Intersect intersect(Mesh& mesh, Ray& ray) {
    Intersect ret;
    ret.dist = 1e9;

    for (int i = 0; i < (int)mesh.faces.size(); i++) {
        Face& f = mesh.faces[i];

        Vec3 q1 = ray.pt.sub(ray.dir.mul(1e4));
        Vec3 q2 = ray.pt.add(ray.dir.mul(1e4));

        bool a = sign(signed_volume(q1, f.p1, f.p2, f.p3));
        bool b = sign(signed_volume(q2, f.p1, f.p2, f.p3));
        bool c = sign(signed_volume(q1, q2, f.p1, f.p2));
        bool d = sign(signed_volume(q1, q2, f.p2, f.p3));
        bool e = sign(signed_volume(q1, q2, f.p3, f.p1));

        if ((a != b) && (c == d) && (d == e)) {  // there is intersection
            Vec3 n = f.p2.sub(f.p1).cross(f.p3.sub(f.p1));
            double t = -1 * q1.sub(f.p1).dot(n) / q2.sub(q1).dot(n);
            Vec3 pt = q1.add(q2.sub(q1).mul(t));
            double dist = pt.sub(ray.pt).magnitude();

            if (dist < ret.dist) {
                ret.dist = dist;
                ret.pos = pt;
                ret.normal = f.normal;
            }
        }
    }

    return ret;
}

/**
 * Builds a shadow map for light and stores in map.
 * @param index is for verbose
 */
void build_map(Scene& scene, ShadowMap& map, Light& light, int index = 0, bool verbose = false) {
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

            double dist = 1e9;
            for (int i = 0; i < (int)scene.objs.size(); i++) {
                double d = intersect(scene.objs[i], ray).dist;
                if (d < dist)
                    dist = d;
            }
            map.set(x, y, dist);
        }
    }
}

void build(Scene& scene, bool verbose) {
    if (scene._built)
        return;
    scene._built = true;

    for (int i = 0; i < (int)scene.lights.size(); i++) {
        scene.shadow_maps.push_back(ShadowMap(scene.SHMAP_W, scene.SHMAP_H));
        build_map(scene, scene.shadow_maps[i], scene.lights[i], i, verbose);
    }

    if (verbose)
        std::cerr << std::endl;
}


}  // namespace Shadowmap
