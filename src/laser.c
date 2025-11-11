#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <laser_math.h>

#define RAD_TO_DEG (180.f / 3.1415926535f)
#define TWO_PI (2 * 3.1415926535f)

int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    #define NUM_POINTS 100

    point_t points[NUM_POINTS];
    // point_t* points = (point_t*)calloc(NUM_POINTS, sizeof(point_t));

    for(int i = 0; i < NUM_POINTS; i++) {
        points[i].x = cosf((TWO_PI / NUM_POINTS) * (float)i);
        points[i].y = sinf((TWO_PI / NUM_POINTS) * (float)i);
    }

    for(unsigned i = 1; i < NUM_POINTS; i++) {
        point_t projected_point_before;
        project_point(&projected_point_before, &points[i-1]);

        point_t projected_point;
        project_point(&projected_point, &points[i]);

        angles_t angles;
        angles_between_projected_points(&angles, &projected_point_before, &projected_point);

        printf("(%f, %f), (%f, %f)\n", projected_point_before.x, projected_point_before.y,
                                       projected_point.x, projected_point.y);

        printf("yaw: %f, pitch: %f\n", angles.yaw.value * RAD_TO_DEG, angles.pitch.value * RAD_TO_DEG);
    }

    // free(points);
}
