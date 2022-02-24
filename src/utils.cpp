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
#include "shadowmap.hpp"


namespace Shadowmap {


double distance(double dx, double dy, double dz) {
    return sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));
}

double distance(double dx, double dy) {
    return sqrt(pow(dx, 2) + pow(dy, 2));
}

int bounds(int v, int min, int max) {
    return std::min(std::max(v, min), max);
}

bool sign(double v) {
    return v >= 0;
}

double randd() {
    return (rand() % (int)1e9) / 1e9;
}


}  // namespace Shadowmap
