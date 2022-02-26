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

#include <cmath>
#include <fstream>
#include <string>
#include <vector>

#include "linalg.hpp"
#include "image.hpp"
#include "utils.hpp"


namespace Shadowmap {


constexpr double PI = 3.14159;


struct Face {
    Vec3 p1, p2, p3;
    Vec3 normal;

    Vec3 _color;  // automatically set
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
    Vec3 loc;  // location
    Vec3 color;  // rgb, 0 to 1
    std::vector<Face> faces;

    Mesh();

    Mesh(const Vec3& loc, const Vec3& color);

    Mesh(const Vec3& loc, const Vec3& color, const std::string& filename);

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
    Vec3 color;  // rgb, 0 to 1
    double power;

    Light(double x, double y, double z, double power, const Vec3& color);

    Light(const Vec3& loc, double power, const Vec3& color);
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
    Vec3 bg;  // background color, 0 to 1

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
    void add_light(double x, double y, double z, double power, const Vec3& color);
};


/**
 * From https://stackoverflow.com/q/42740765/
 */
inline double signed_volume(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d) {
    return b.sub(a).cross(c.sub(a)).dot(d.sub(a)) / 6.0;
}

/**
 * Intersect faces with a ray.
 * If no intersection, distance is arbitrarily large number.
 * Else, smallest distance.
 *
 * @param faces sorted by Face._min_dist
 * @param faces build_faces() call with respect to ray.pt
 *
 * Intersection formula from https://stackoverflow.com/q/42740765/
 */
inline Intersect intersect(std::vector<Face>& faces, Ray& ray) {
    Intersect ret;
    ret.dist = 1e9;

    for (Face& f: faces) {
        // ignore face if can't be intersected
        Vec3 delta = f._center.sub(ray.pt);
        if (ray.dir.angle(delta) > f._angle)
            continue;

        // if current dist is closer than what this face can possibly be,
        // and all faces later are farther away, then we can stop.
        if (ret.dist < f._min_dist-0.01)
            break;

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
                ret.color = f._color;
            }
        }
    }

    return ret;
}


/**
 * Build faces with respect to a point.
 * Used internally.
 */
inline void build_faces(Scene& scene, Vec3& pt) {
    for (Face& face: scene._faces) {
        face._min_dist = min(
            distance(pt, face.p1),
            distance(pt, face.p2),
            distance(pt, face.p3)
        );

        Vec3 center = face._center.sub(pt);
        face._angle = max(
            center.angle(face.p1.sub(pt)),
            center.angle(face.p2.sub(pt)),
            center.angle(face.p3.sub(pt))
        );
    }

    std::sort(scene._faces.begin(), scene._faces.end(),
        [](Face& a, Face& b){return a._min_dist < b._min_dist;}
    );
}

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
