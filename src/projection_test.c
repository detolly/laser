#include <stdio.h>

#include <config.h>
#include <laser_math.h>
#include <picture.h>

// #include <math.h>
// #define NUM_POINTS 100
// static point_t points[NUM_POINTS] = {0};

#include <points/game_of_thrones.h>
#define NUM_POINTS sizeof(points) / sizeof(point_t)

int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    // for(int i = 0; i < NUM_POINTS; i++) {
    //     points[i].x = cosf((TWO_PI / NUM_POINTS) * (float)i);
    //     points[i].y = sinf((TWO_PI / NUM_POINTS) * (float)i);
    //     // printf("%f,%f\n", points[i].x, points[i].y);
    // }

    calculate_grid_points();

    picture_t picture = {0};
    picture_from_points(&picture, points, NUM_POINTS);

    fprintf(stderr, "total rotations: %f %f\n", picture.total_yaw_angle / TWO_PI, picture.total_pitch_angle / TWO_PI);

    for(unsigned i = 0; i < picture.num_points; i++) {
        point_t* p = &picture.projections[i].grid_point;
        point_t* p2 = &picture.projections[i].projected_point;
        angles_t* a = &picture.projections[i].grid_angles;
        motor_instruction_pair_t* m = &picture.instructions[i];

        printf("%f,%f,%f,%f,%u,%u", p->x, p->y, a->yaw, a->pitch, m->yaw.steps, m->pitch.steps);
        printf(",%f,%f,%f", p2->x, 0.0f, p2->y);
        puts("");
    }

    picture_free(&picture);
    free_grid_points();
}
