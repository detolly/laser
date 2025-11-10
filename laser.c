#include <stdio.h>

#include <laser_math.h>

#define RAD_TO_DEG (180.f / 3.1415926535f)

const static point_t points[] = {
    { -1.f, -1.f },
    { 1.f, 1.f },
};

int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    for(unsigned i = 1; i < sizeof(points)/sizeof(point_t); i++) {
        const point_t projected_point_before = project(points[i-1]);
        const point_t projected_point= project(points[i]);
        const angles_t angles = angles_between_projected_vectors(projected_point_before, projected_point);

        printf("(%f, %f), (%f, %f)\n", projected_point_before.x, projected_point_before.y,
                                       projected_point.x, projected_point.y);

        printf("yaw: %f, pitch: %f\n", angles.yaw * RAD_TO_DEG, angles.pitch * RAD_TO_DEG);
    }
}
