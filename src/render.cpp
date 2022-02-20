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


Ray::Ray(double x, double y, double z, double dx, double dy, double dz) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->dx = dx;
    this->dy = dy;
    this->dz = dz;
}

void Ray::make_unit() {
    double dist = distance(dx, dy, dz);
    dx /= dist;
    dy /= dist;
    dz /= dist;
}


/**
 * Builds a shadow map for light and stores in map.
 */
void build_map(Scene& scene, ShadowMap& map, Light& light) {
    for (int y = 0; y < scene.SHMAP_H; y++) {
        for (int x = 0; x < scene.SHMAP_W; x++) {
            double tilt = ((double)y/scene.SHMAP_H - 0.5) * PI;
            double pan = ((double)x/scene.SHMAP_W - 0.5) * PI * 2;

            double dz = -sin(tilt);
            double dy = cos(pan) * cos(tilt);
            double dx = sin(pan) * cos(tilt);

            Ray ray(light.x, light.y, light.z, dx, dy, dz);
            double dist = 1e9;
            for (int i = 0; i < (int)scene.objs.size(); i++) {
                double d = scene.objs[i].intersect(ray);
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
double read_shadow_map(Scene& scene, ShadowMap& map, double dx, double dy, double dz) {
    double tilt = atan2(-dz, distance(dx, dy));
    double pan = atan2(dx, dy);
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
    double tilt = ((double)y/img.h - 0.5) * 2*PI * fov_y;
    double pan = ((double)x/img.w - 0.5) * 2*PI * fov_x;

    // add randomness to tilt and pan
    tilt += scene.jitter * randd() * fov_y / img.h;
    pan += scene.jitter * randd() * fov_x / img.w;

    double dz = -sin(tilt);
    double dy = cos(pan) * cos(tilt);
    double dx = sin(pan) * cos(tilt);

    Ray ray(scene.cam_x, scene.cam_y, scene.cam_z, dx, dy, dz);
    ray.make_unit();
    double dist = 1e9;
    for (int i = 0; i < (int)scene.objs.size(); i++) {
        double d = scene.objs[i].intersect(ray);
        if (d < dist)
            dist = d;
    }

    // coordinates of hit
    double hx = ray.x + ray.dx*dist;
    double hy = ray.y + ray.dy*dist;
    double hz = ray.z + ray.dz*dist;

    double v = scene.scene_light;
    for (int i = 0; i < (int)scene.lights.size(); i++) {
        Light& light = scene.lights[i];
        double dx = hx - light.x;
        double dy = hy - light.y;
        double dz = hz - light.z;
        double d_map = read_shadow_map(scene, scene.shadow_maps[i], dx, dy, dz);
        double d_real = distance(dx, dy, dz);

        if (d_real > d_map+0.02)
            continue;

        double power = light.power / (d_real*d_real);
        v += power;
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
