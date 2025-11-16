from svgpathtools import svg2paths

import sys

NUM_POINTS_PATH = 5

paths, _ = svg2paths(sys.argv[1])
points = []
for path in paths:
    for seg in path:
        for i in range(0,NUM_POINTS_PATH):
            points.append(seg.point(i*(1/(NUM_POINTS_PATH-1))))

max_point_x = max([p.real for p in points])
max_point_y = max([p.imag for p in points])

min_point_x = min([p.real for p in points])
min_point_y = min([p.imag for p in points])

width = max_point_x - min_point_x
height = max_point_y - min_point_y

should_scale_y = width > height

new_points = [((p.real - min_point_x) / (width if should_scale_y else height) * 2 - 1, (p.imag - min_point_y) / (width if should_scale_y else height) * 2 - 1) for p in points]

# for p in new_points:
#     print(f"{p[0]:.4f} {p[1]:.4f}")

# for testing
print("#pragma once\n")
print("#include <laser/math.h>\n")
print("static const point_t points[] = {")
for p in new_points:
    print(f"    {{ {p[0]:.4f}f, {p[1]:.4f}f }},")
print("};")
