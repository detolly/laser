from svgpathtools import svg2paths

import sys

paths, _ = svg2paths(sys.argv[1])
points = []
for path in paths:
    for seg in path:
        points.append(seg.point(0.5))

max_point_x = abs(max(points, key=lambda x: abs(x.real)))
max_point_y = abs(max(points, key=lambda x: abs(x.imag)))

new_points = [((abs(p.real) / max_point_x) * 2 - 1, (abs(p.imag) / max_point_y) * 2 - 1) for p in points]

# for p in new_points:
#     print(f"{p[0]:.4f},{p[1]:.4f}")

print("point_t arr[] = {")
for p in new_points:
    print(f"    {{ {p[0]:.4f}f, {p[1]:.4f}f }},")
print("};")
