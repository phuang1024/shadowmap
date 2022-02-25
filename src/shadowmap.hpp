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
#include <string>
#include <vector>


namespace Shadowmap {


typedef  unsigned char  UCH;

constexpr double PI = 3.14159;


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
 * 3D vector of doubles.
 */
struct Vec3 {
    double x, y, z;

    /**
     * Initialize to 0
     */
    Vec3();

    Vec3(double x, double y, double z);

    Vec3(const Vec3& v);

    double magnitude() const;

    double sum() const;

    /**
     * Sum of squares of components.
     * x**2 + y**2 + z**2
     */
    double sqsum() const;

    Vec3 unit() const;

    Vec3 add(const Vec3& v) const;

    Vec3 sub(const Vec3& v) const;

    Vec3 mul(double s) const;

    /**
     * Element wise multiplication.
     */
    Vec3 mul(const Vec3& v) const;

    Vec3 div(double s) const;

    double dot(const Vec3& v) const;

    Vec3 cross(const Vec3& v) const;

    /**
     * Angle between this and v.
     */
    double angle(const Vec3& v) const;
};

/**
 * Vector with starting point.
 */
struct Ray {
    Vec3 pt;  // starting point
    Vec3 dir;

    Ray(double x, double y, double z, double dx, double dy, double dz);

    Ray(const Vec3& pt, const Vec3& dir);
};


struct Face {
    Vec3 p1, p2, p3;
    Vec3 normal;

    Vec3 _center;  // used internally, avg(p1, p2, p3)
    double _radius;  // used internally, max(dist(p1, _center), ...)
    double _angle;  // used internally
    double _min_dist;  // used internally

    Face(const Vec3& p1, const Vec3& p2, const Vec3& p3, const Vec3& normal);
};

/**
 * Mesh object. Can read binary STL file.
 */
struct Mesh {
    Vec3 loc;    // location
    std::vector<Face> faces;

    Mesh();

    Mesh(const Vec3& loc);

    Mesh(const Vec3& loc, const std::string& filename);

    Mesh(const std::string& filename);

    /**
     * Clears faces and reads from file.
     */
    void read_stl(std::ifstream& fp);
};

/**
 * Point light source.
 */
struct Light {
    Vec3 loc;  // location
    double power;

    Light(double x, double y, double z, double power);

    Light(const Vec3& loc, double power);
};


/**
 * Collection of things to render.
 * Also camera parameters.
 */
struct Scene {
    std::vector<Mesh> objs;
    std::vector<Light> lights;
    std::vector<ShadowMap> shadow_maps;
    int SHMAP_W, SHMAP_H;

    Vec3 cam_loc;
    double cam_pan, cam_tilt;  // radians. (0, 0) faces +y
    double fov;   // FOV in degrees of X (horizontal) of camera.
    double bg;  // background light intensity

    std::vector<Face> _faces;  // used internally

    Scene();

    Scene(double cam_x, double cam_y, double cam_z, double pan, double tilt, double fov);

    /**
     * Frees shadow maps.
     */
    ~Scene();

    /**
     * Initialize default values. Called from all constructors.
     */
    void _init();

    /**
     * Add a light to the scene.
     */
    void add_light(double x, double y, double z, double power);
};


double distance(double dx, double dy);

double distance(double dx, double dy, double dz);

double distance(Vec3& v1, Vec3& v2);

/**
 * Clamp value to range.
 */
int bounds(int v, int min, int max);

/**
 * True if positive.
 * False if 0 or negative.
 */
bool sign(double v);

/**
 * Random from 0 to 1
 */
double randd();

/**
 * Milliseconds since epoch.
 */
int time();


/**
 * Information about an intersection between a ray and a face.
 */
struct Intersect {
    double dist;  // distance from ray origin to intersection
    Vec3 normal;  // normal of the face at intersection
    Vec3 pos;     // position of the intersection
};

/**
 * Intersect faces with a ray.
 * If no intersection, distance is arbitrarily large number.
 * Else, smallest distance.
 *
 * @param faces sorted by Face._min_dist
 * @param faces build_faces() call with respect to ray.pt
 */
Intersect intersect(std::vector<Face>& faces, Ray& ray);

/**
 * Build faces with respect to a point.
 * Used internally.
 */
void build_faces(Scene& scene, Vec3& pt);

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
