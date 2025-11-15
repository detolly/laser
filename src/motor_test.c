#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <config.h>
#include <laser_math.h>
#include <motor.h>
#include <picture.h>

#define NUM_POINTS 100
#include <math.h>
static point_t points[NUM_POINTS] = {0};

// #include <points/game_of_thrones.h>
// #define NUM_POINTS sizeof(points) / sizeof(point_t)
// #define GENERATED

int main(int argc, const char* argv[])
{
    assert(argc == 6);

    fprintf(stderr, "Using parameters: \n");
    fprintf(stderr, "\tdistance_up: %s\n", argv[1]);
    fprintf(stderr, "\tdistance_to_wall: %s\n", argv[2]);
    fprintf(stderr, "\tpicture_size: %s\n", argv[3]);
    fprintf(stderr, "\tsteps_per_revolution_yaw: %s\n", argv[4]);
    fprintf(stderr, "\tsteps_per_revolution_pitch: %s\n", argv[5]);
    fprintf(stderr, "\tmotor_speed: %s\n", argv[6]);

    config_t cfg;
    cfg.distance_up = (float)atof(argv[1]);
    cfg.distance_to_wall = (float)atof(argv[2]);
    cfg.picture_size = (float)atof(argv[3]);
    cfg.steps_per_revolution_yaw = (size_t)atoi(argv[4]);
    cfg.steps_per_revolution_pitch = (size_t)atoi(argv[5]);
    cfg.motor_speed = (size_t)atoi(argv[6]);
    set_config(&cfg);

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
    stop_motor();
    stop_motor_thread();

    picture_free(&picture);
    free_grid_points();
}
