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

#include <fstream>
#include <vector>


namespace Shadowmap {
    typedef  unsigned char  UCH;

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

        double cam_x, cam_y, cam_z;  // camera position, always facing +y
        double fov;   // FOV in degrees of X of camera.
        double scene_light;  // scene light intensity

        bool _built;

        Scene();

        Scene(double cam_x, double cam_y, double cam_z, double fov);

        /**
         * Frees shadow maps.
         */
        ~Scene();

        /**
         * Build scene.
         * Call before rendering.
         */
        void build(bool verbose = false);

        /**
         * Builds a shadow map.
         */
        void build_map(ShadowMap& img, Light& light);

        /**
         * Renders an image and stores in img.
         */
        void render(Image& img, int samples, bool verbose = false);

        /**
         * Renders a pixel and stores in img.
         */
        double render_px(Image& img, int x, int y);

        /**
         * Read a pixel of the shadow map given the XYZ point.
         * Delta values are relative to the light.
         * Delta values are unit vector.
         */
        double read_shadow_map(ShadowMap& map, double dx, double dy, double dz);
    };
}
