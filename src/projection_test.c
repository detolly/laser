#include <laser/debug.h>

#include <stdio.h>

#include <laser/config.h>
#include <laser/math.h>
#include <laser/picture.h>

// #include <math.h>
// #define NUM_POINTS 100
// static point_t points[NUM_POINTS] = {0};

#include <laser/points/game_of_thrones.h>
#define NUM_POINTS sizeof(points) / sizeof(point_t)
#define GENERATED

int main(int argc, const char* argv[])
{
    set_config_from_argv(argc, argv);

#ifndef GENERATED
    for(int i = 0; i < NUM_POINTS; i++) {
        points[i].x = cosf((TWO_PI / NUM_POINTS) * (float)i);
        points[i].y = sinf((TWO_PI / NUM_POINTS) * (float)i);
    }
#endif

    picture_t* picture = managed_picture_from_points(points, NUM_POINTS);

    DEBUG("total rotations: %f %f\n", picture->total_yaw_angle / TWO_PI, picture->total_pitch_angle / TWO_PI);

    for(unsigned i = 0; i < picture->num_points; i++) {
        point_t* p = &picture->picture_points[i].grid_point;
        point_t* p2 = &picture->picture_points[i].projected_point;
        angles_t* a = &picture->picture_points[i].grid_angles;
        motor_instruction_pair_t* m = &picture->instructions[i];

        printf("%f,%f,%f,%f,%u,%u", p->x, p->y, a->yaw, a->pitch, m->yaw.steps, m->pitch.steps);
        printf(",%f,%f,%f", p2->x, 0.0f, p2->y);
        puts("");
    }

    free_managed_pictures();
}
