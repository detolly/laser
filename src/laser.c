#include <math.h>
#include <stdio.h>

// #include <gen/points.h>

#include <config.h>
#include <laser_math.h>
#include <picture.h>

#define NUM_POINTS 100
static point_t points[NUM_POINTS] = {0};

// #define NUM_POINTS sizeof(points) / sizeof(point_t)

int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    for(int i = 0; i < NUM_POINTS; i++) {
        points[i].x = cosf((TWO_PI / NUM_POINTS) * (float)i);
        points[i].y = sinf((TWO_PI / NUM_POINTS) * (float)i);
        // printf("%f,%f\n", points[i].x, points[i].y);
    }

    calculate_grid_points();

    picture_t picture = {0};
    picture_from_points(&picture, points, NUM_POINTS);

    for(unsigned i = 0; i < picture.num_points; i++) {
        point_t* p = &picture.projections[i].fixed_point;
        point_t* p2 = &picture.projections[i].projected_point;
        angles_t* a = &picture.projections[i].fixed_angles;
        motor_instruction_pair_t* m = &picture.instructions[i];

        printf("%f,%f,%f,%f,%f,%d,%d", p->x, distance_to_wall(), p->y, a->yaw, a->pitch, m->yaw.steps, m->pitch.steps);
        printf(",%f,%f,%f", p2->x, 0.0f, p2->y);
        puts("");
    }

    picture_free(&picture);
    free_grid_points();
}
