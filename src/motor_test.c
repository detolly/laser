#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <config.h>
#include <laser_math.h>
#include <motor.h>
#include <picture.h>

// #define NUM_POINTS 100
// #include <math.h>
// static point_t points[NUM_POINTS] = {0};

#include <points/game_of_thrones.h>
#define NUM_POINTS sizeof(points) / sizeof(point_t)
#define GENERATED

int main(int argc, const char* argv[])
{
    set_config_from_argv(argc, argv);

#ifndef GENERATED
    for(int i = 0; i < NUM_POINTS; i++) {
        points[i].x = cosf((TWO_PI / NUM_POINTS) * (float)i);
        points[i].y = sinf((TWO_PI / NUM_POINTS) * (float)i);
        printf("%f,%f\n", points[i].x, points[i].y);
    }
#endif

    motor_init();
    start_motor_thread();

    picture_t picture = {0};
    picture_from_points(&picture, points, NUM_POINTS);

    set_picture(&picture);
    start_motor();
    sleep(10);
    stop_motor_thread();

    picture_free(&picture);
    free_grid_points();
}
