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
 * Intersect sphere with a ray.
 * If no intersection, returns arbitrarily large number.
 * Else, smallest distance to the intersection.
 */
double intersect(Sphere& sph, Ray& ray) {
    Vec3 consts = ray.pt.sub(sph.loc);

    double a = pow(ray.dir.magnitude(), 2);
    double b = 2 * (ray.dir.x*consts.x + ray.dir.y*consts.y + ray.dir.z*consts.z);
    double c = pow(consts.magnitude(), 2) - pow(sph.rad, 2);

    double discr = pow(b, 2) - 4*a*c;
    if (discr < 0)
        return 1e9;

    double t1 = (-b + sqrt(discr)) / (2*a);
    double t2 = (-b - sqrt(discr)) / (2*a);
    double d1 = (t1 < 0) ? 1e9 : ray.dir.mul(t1).magnitude();
    double d2 = (t2 < 0) ? 1e9 : ray.dir.mul(t2).magnitude();

    return std::min(d1, d2);
}

/**
 * Builds a shadow map for light and stores in map.
 */
void build_map(Scene& scene, ShadowMap& map, Light& light) {
    for (int y = 0; y < scene.SHMAP_H; y++) {
        for (int x = 0; x < scene.SHMAP_W; x++) {
            double tilt = ((double)y/scene.SHMAP_H - 0.5) * PI;
            double pan = ((double)x/scene.SHMAP_W - 0.5) * PI * 2;

            Vec3 delta(sin(pan)*cos(tilt), cos(pan)*cos(tilt), -sin(tilt));
            Ray ray(light.loc.x, light.loc.y, light.loc.z, delta.x, delta.y, delta.z);

            double dist = 1e9;
            for (int i = 0; i < (int)scene.objs.size(); i++) {
                double d = intersect(scene.objs[i], ray);
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
        if (verbose)
            std::cerr << "\rBuilding shadow maps: " << i+1 << "/" << scene.lights.size() << std::flush;

        scene.shadow_maps.push_back(ShadowMap(scene.SHMAP_W, scene.SHMAP_H));
        build_map(scene, scene.shadow_maps[i], scene.lights[i]);
    }

    if (verbose)
        std::cerr << std::endl;
}


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
double render_px(Scene& scene, Image& img, int x, int y) {
    double fov_x = scene.fov / 360;
    double fov_y = fov_x * img.h / img.w;
    double tilt = ((double)y/img.h - 0.5) * 2*PI * fov_y + scene.cam_tilt;
    double pan = ((double)x/img.w - 0.5) * 2*PI * fov_x + scene.cam_pan;

    // add randomness to tilt and pan
    tilt += 8 * randd() * fov_y / img.h;
    pan += 8 * randd() * fov_x / img.w;

    // find closest object in current pixel
    Vec3 delta(sin(pan)*cos(tilt), cos(pan)*cos(tilt), -sin(tilt));
    Ray ray(scene.cam_loc.x, scene.cam_loc.y, scene.cam_loc.z, delta.x, delta.y, delta.z);
    ray.dir = ray.dir.unit();

    double dist = 1e9;
    int obj_ind = -1;
    for (int i = 0; i < (int)scene.objs.size(); i++) {
        double d = intersect(scene.objs[i], ray);
        if (d < dist) {
            dist = d;
            obj_ind = i;
        }
    }
    if (obj_ind == -1)
        return scene.bg;

    // coordinates of hit
    Vec3 hit = ray.pt.add(ray.dir.mul(dist));

    // normal vector, only dx, dy, dz of ray are used
    Sphere& obj = scene.objs[obj_ind];
    Vec3 normal = hit.sub(obj.loc).unit();

    // compute lighting
    double v = scene.bg;
    for (int i = 0; i < (int)scene.lights.size(); i++) {
        // see if this light hits the object
        Light& light = scene.lights[i];
        Vec3 delta = hit.sub(light.loc);
        double d_map = read_shadow_map(scene, scene.shadow_maps[i], delta);
        double d_real = delta.magnitude();
        if (d_real-d_map > 0.3)
            continue;

        // inverse square falloff
        double fac_dist = 1 / pow(d_real, 2);

        // dim by dot of normal and light vector
        Vec3 light_ray = light.loc.sub(hit).unit();
        double fac_norm = light_ray.dot(normal);
        fac_norm = std::max(fac_norm, 0.0);

        v += light.power * fac_dist * fac_norm;
    }

    if (v > 255)
        v = 255;
    return v;
}

void render(Scene& scene, Image& img, int samples, bool verbose) {
    if (!scene._built)
        build(scene, verbose);

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

            double sum = 0;
            for (int i = 0; i < samples; i++)
                sum += render_px(scene, img, x, y);
            sum /= samples;

            for (int chn = 0; chn < 3; chn++)
                img.set(x, y, chn, sum);
        }
    }

    if (verbose)
        std::cerr << std::endl;
}


}  // namespace Shadowmap
