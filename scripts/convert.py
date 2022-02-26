#
#  Shadowmap
#  Shadow map rendering engine.
#  Copyright  Patrick Huang  2022
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

"""
Converts output from Shadowmap to PNG.

python convert.py input.img output.png
"""

import sys
import struct
import numpy as np
import cv2


def main():
    rgb = len(sys.argv) <= 3

    with open(sys.argv[1], "rb") as fp:
        w, h = struct.unpack("<II", fp.read(8))
        shape = (h, w, 3) if rgb else (h, w)
        img = np.zeros(shape, dtype=np.uint8)

        for y in range(h):
            for x in range(w):
                if rgb:
                    img[y, x] = struct.unpack("<BBB", fp.read(3))
                else:
                    img[y, x] = fp.read(1)[0]

    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    cv2.imwrite(sys.argv[2], img)


main()
