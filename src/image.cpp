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

#include "shadowmap.hpp"


namespace Shadowmap {


Image::Image(int width, int height) {
    w = width;
    h = height;

    data = new UCH[w * h * 3];
}

Image::~Image() {
    delete[] data;
}

UCH Image::get(int x, int y, int chn) {
    return data[3*(y*w + x) + chn];
}

void Image::set(int x, int y, int chn, UCH value) {
    data[3*(y*w + x) + chn] = value;
}

void Image::write(std::ofstream& fp) {
    fp.write((char*)&w, sizeof(int));
    fp.write((char*)&h, sizeof(int));
    fp.write((char*)data, w*h*3);
}


ShadowMap::ShadowMap(int width, int height) {
    w = width;
    h = height;

    data = new double[w * h];
}

void ShadowMap::free() {
    delete[] data;
}

double ShadowMap::get(int x, int y) {
    return data[y*w + x];
}

void ShadowMap::set(int x, int y, double value) {
    data[y*w + x] = value;
}


}  // namespace Shadowmap
