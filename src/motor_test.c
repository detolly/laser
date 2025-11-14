#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <config.h>
#include <laser_math.h>
#include <motor.h>
#include <picture.h>

// #define NUM_POINTS 100
// static point_t points[NUM_POINTS] = {0};

#include <points/game_of_thrones.h>
#define NUM_POINTS sizeof(points) / sizeof(point_t)

int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    config_t cfg;
    cfg.distance_up = (float)atof(argv[1]);
    cfg.distance_to_wall = (float)atof(argv[2]);
    cfg.picture_size = (float)atof(argv[3]);
    cfg.steps_per_revolution_yaw = (size_t)atoi(argv[4]);
    cfg.steps_per_revolution_pitch = (size_t)atoi(argv[5]);
    cfg.motor_speed = (size_t)atoi(argv[6]);
    set_config(&cfg);

    // for(int i = 0; i < NUM_POINTS; i++) {
    //     points[i].x = cosf((TWO_PI / NUM_POINTS) * (float)i);
    //     points[i].y = sinf((TWO_PI / NUM_POINTS) * (float)i);
    //     // printf("%f,%f\n", points[i].x, points[i].y);
    // }

    motor_init();
    start_motor_thread();

    picture_t picture = {0};
    picture_from_points(&picture, points, NUM_POINTS);

    set_picture(&picture);
    start_motor();
    sleep(2);
    fputs("slept for 2\n", stderr);
    stop_motor();
    sleep(2);
    start_motor();
    sleep(2);
    stop_motor_thread();

    picture_free(&picture);
    free_grid_points();
}
