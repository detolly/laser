#include <assert.h>
#include <unistd.h>

#include <laser/config.h>
#include <laser/math.h>
#include <laser/motor.h>
#include <laser/picture.h>

// #define NUM_POINTS 100
// #include <math.h>
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

    motor_init();
    start_motor_thread();

    picture_t* picture = managed_picture_from_points(points, NUM_POINTS);
    set_picture(picture);

    start_motor();
    sleep(10);
    stop_motor_thread();

    free_managed_pictures();
    motor_free();
}
