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
 * Read a pixel of the shadow map given the XYZ point.
 * Delta values are relative to the light.
 * Delta values are unit vector.
 */
double read_shadow_map(Scene& scene, ShadowMap& map, Vec3 delta) {
    double tilt = atan2(-delta.z, distance(delta.x, delta.y));
    double pan = atan2(delta.x, delta.y);
    int y = (tilt/PI + 0.5) * scene.SHMAP_H;
    int x = (pan/PI/2 + 0.5) * scene.SHMAP_W;

    x = bounds(x, 0, scene.SHMAP_W-1);
    y = bounds(y, 0, scene.SHMAP_H-1);
    return map.get(x, y);
}

/**
 * Returns the color of rendered pixel.
 */
Vec3 render_px(Scene& scene, Image& img, int x, int y) {
    double fov_x = scene.fov / 360;
    double fov_y = fov_x * img.h / img.w;
    double tilt = ((double)y/img.h - 0.5) * 2*PI * fov_y + scene.cam_tilt;
    double pan = ((double)x/img.w - 0.5) * 2*PI * fov_x + scene.cam_pan;

    // add randomness to tilt and pan
    tilt += randd() * fov_y / img.h;
    pan += randd() * fov_x / img.w;

    // find closest object in current pixel
    Vec3 delta(sin(pan)*cos(tilt), cos(pan)*cos(tilt), -sin(tilt));
    Ray ray(scene.cam_loc, delta.unit());
    Intersect inter = intersect(scene._faces, ray);
    if (inter.dist >= 1e9-10)
        return scene.bg;

    const Vec3& hit = inter.pos;
    const Vec3& normal = inter.normal;

    // compute lighting
    Vec3 v = scene.bg;
    for (int i = 0; i < (int)scene.lights.size(); i++) {
        // see if this light hits the object
        Light& light = scene.lights[i];
        Vec3 delta = hit.sub(light.loc);
        double d_map = read_shadow_map(scene, scene.shadow_maps[i], delta);
        double d_real = delta.magnitude();
        if (d_real-d_map > 0.1)
            continue;

        // inverse square falloff
        double fac_dist = 1 / pow(d_real, 2);

        // dim by dot of normal and light vector
        Vec3 light_ray = light.loc.sub(hit).unit();
        double fac_norm = light_ray.dot(normal);
        fac_norm = std::max(fac_norm, 0.0);

        double power = light.power * fac_dist * fac_norm;
        v = v.add(light.color.mul(inter.color).mul(power));
    }

    v.x = dbounds(v.x, 0, 1);
    v.y = dbounds(v.y, 0, 1);
    v.z = dbounds(v.z, 0, 1);
    return v;
}

void render(Scene& scene, Image& img, int samples, bool verbose) {
    int start = time();

    build_faces(scene, scene.cam_loc);

    int last_percent = -1;  // for verbose
    for (int y = 0; y < img.h; y++) {
        for (int x = 0; x < img.w; x++) {
            if (verbose) {
                int percent = (y*img.w + x) * 100 / (img.w*img.h);
                if (percent != last_percent) {
                    std::cerr << "\rRendering: " << percent << "%" << std::flush;
                    last_percent = percent;
                }
            }

            Vec3 sum;
            for (int i = 0; i < samples; i++)
                sum = sum.add(render_px(scene, img, x, y));
            sum = sum.div(samples).mul(255);

            img.set(x, y, 0, sum.x);
            img.set(x, y, 1, sum.y);
            img.set(x, y, 2, sum.z);
        }
    }

    if (verbose) {
        double elapse = (time() - start) / 1000.0;
        std::cerr << "\rRender finished in " << elapse << " seconds" << std::endl;
    }
}


}  // namespace Shadowmap
