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


namespace Shadowmap {


typedef  unsigned char  UCH;


/**
 * RGB unsigned char image.
 */
struct Image {
    int w, h;
    UCH* data;

    Image(int width, int height) {
        w = width;
        h = height;

        data = new UCH[w * h * 3];
    }

    /**
     * Free data.
     */
    ~Image() {
        delete[] data;
    }

    /**
     * Get pixel and channel value.
     */
    UCH get(int x, int y, int chn) {
        return data[3*(y*w + x) + chn];
    }

    /**
     * Set pixel and channel value.
     */
    void set(int x, int y, int chn, UCH value) {
        data[3*(y*w + x) + chn] = value;
    }

    /**
     * Write image to file.
     * Use scripts/convert.py to convert to other formats.
     */
    void write(std::ofstream& fp) {
        fp.write((char*)&w, sizeof(int));
        fp.write((char*)&h, sizeof(int));
        fp.write((char*)data, w*h*3);
    }
};


/**
 * Grayscale double image.
 * No automatic deallocation. Use map.free() to free memory.
 */
struct ShadowMap {
    int w, h;
    double* data;

    ShadowMap(int width, int height) {
        w = width;
        h = height;

        data = new double[w * h];
    }

    /**
     * Free data.
     */
    void free() {
        delete[] data;
    }

    /**
     * Get pixel value.
     */
    double get(int x, int y) {
        return data[y*w + x];
    }

    /**
     * Set pixel value.
     */
    void set(int x, int y, double value) {
        data[y*w + x] = value;
    }
};


}  // namespace Shadowmap
