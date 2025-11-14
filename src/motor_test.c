#include <stdio.h>
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

    // for(int i = 0; i < NUM_POINTS; i++) {
    //     points[i].x = cosf((TWO_PI / NUM_POINTS) * (float)i);
    //     points[i].y = sinf((TWO_PI / NUM_POINTS) * (float)i);
    //     // printf("%f,%f\n", points[i].x, points[i].y);
    // }

    start_motor_thread();
    calculate_grid_points();

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
