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

#pragma once

#include <fstream>
#include <vector>


namespace Shadowmap {


typedef  unsigned char  UCH;

constexpr double PI = 3.14159;

/**
 * Absolute distance.
 */
double distance(double dx, double dy);

/**
 * Absolute distance.
 */
double distance(double dx, double dy, double dz);

/**
 * Clamp value to range.
 */
int bounds(int v, int min, int max);

/**
 * Random from 0 to 1
 */
double randd();


/**
 * RGB unsigned char image.
 */
struct Image {
    int w, h;
    UCH* data;

    /**
     * Initialize with width and height.
     */
    Image(int width, int height);

    /**
     * Free data.
     */
    ~Image();

    /**
     * Get pixel and channel value.
     */
    UCH get(int x, int y, int chn);

    /**
     * Set pixel and channel value.
     */
    void set(int x, int y, int chn, UCH value);

    /**
     * Write image to file.
     * Use scripts/convert.py to convert to other formats.
     */
    void write(std::ofstream& fp);
};

/**
 * Grayscale double image.
 * No automatic deallocation. Use map.free() to free memory.
 */
struct ShadowMap {
    int w, h;
    double* data;

    /**
     * Initialize with width and height.
     */
    ShadowMap(int width, int height);

    /**
     * Free data.
     */
    void free();

    /**
     * Get pixel value.
     */
    double get(int x, int y);

    /**
     * Set pixel value.
     */
    void set(int x, int y, double value);
};


/**
 * Vector with starting point.
 */
struct Ray {
    double x, y, z;     // starting point
    double dx, dy, dz;  // direction

    Ray(double x, double y, double z, double dx, double dy, double dz);

    /**
     * Change dx, dy, dz to a unit vector.
     */
    void make_unit();

    /**
     * Magnitude of direction vector.
     */
    double magnitude();
};


/**
 * Sphere object.
 */
struct Sphere {
    double x, y, z;  // center
    double rad;      // radius

    Sphere(double x, double y, double z, double rad);

    /**
     * Intersection with a ray.
     * If no intersection, returns arbitrarily large number.
     * Else, smallest distance to the intersection.
     */
    double intersect(Ray& ray);
};

/**
 * Point light source.
 */
struct Light {
    double x, y, z;
    double power;

    Light(double x, double y, double z, double power);
};


/**
 * Collection of things to render.
 * Also camera parameters.
 */
struct Scene {
    std::vector<Sphere> objs;
    std::vector<Light> lights;
    std::vector<ShadowMap> shadow_maps;
    int SHMAP_W, SHMAP_H;

    double cam_x, cam_y, cam_z;  // camera position, always facing +y
    double fov;   // FOV in degrees of X of camera.
    double scene_light;  // scene light intensity
    double jitter;  // randomness in sampling

    bool _built;

    Scene();

    Scene(double cam_x, double cam_y, double cam_z, double fov);

    /**
     * Frees shadow maps.
     */
    ~Scene();

    /**
     * Initialize default values. Called from all constructors.
     */
    void _init();

    /**
     * Add a sphere to the scene.
     */
    void add_sphere(double x, double y, double z, double rad);

    /**
     * Add a light to the scene.
     */
    void add_light(double x, double y, double z, double power);
};


/**
 * Build scene.
 * Call before rendering.
 */
void build(Scene& scene, bool verbose = false);

/**
 * Renders an image and stores in img.
 */
void render(Scene& scene, Image& img, int samples, bool verbose = false);


}  // namespace Shadowmap
