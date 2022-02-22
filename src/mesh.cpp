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


Face::Face(const Vec3& p1, const Vec3& p2, const Vec3& p3, const Vec3& normal) {
    this->p1 = p1;
    this->p2 = p2;
    this->p3 = p3;
    this->normal = normal;
}


Mesh::Mesh() {
}

Mesh::Mesh(const Vec3& loc) {
    this->loc = loc;
}

Mesh::Mesh(const Vec3& loc, const std::string& filename) {
    this->loc = loc;
    std::ifstream fp(filename);
    read_stl(fp);
}

Mesh::Mesh(const std::string& filename) {
    std::ifstream fp(filename);
    read_stl(fp);
}

void Mesh::read_stl(std::ifstream& fp) {
    union uint32 {
        int i;
        char c[4];
    };
    union float32 {
        float f;
        char c[4];
    };

    fp.seekg(80, std::ios::cur);  // skip header
    uint32 count;
    fp.read(count.c, 4);

    for (int i = 0; i < count.i; i++) {
        float32 data[12];   // normal(3), pt1(3), pt2(3), pt3(3)
        for (int j = 0; j < 12; j++)
            fp.read(data[j].c, 4);

        Vec3 normal(data[0].f, data[1].f, data[2].f);
        Vec3 p1(data[3].f, data[4].f, data[5].f);
        Vec3 p2(data[6].f, data[7].f, data[8].f);
        Vec3 p3(data[9].f, data[10].f, data[11].f);
        faces.push_back(Face(p1, p2, p3, normal));

        fp.seekg(2, std::ios::cur);  // skip attribute byte count
    }
}


}  // namespace Shadowmap
