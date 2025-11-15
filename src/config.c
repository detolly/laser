#include <config.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <laser_math.h>
#include <motor.h>

config_t g_config = {
    .steps_per_revolution_yaw = 15000,
    .steps_per_revolution_pitch = 15000,
    .motor_speed = 750,

    .picture_size = 2.f,
    .distance_to_wall = 5.f,
    .distance_up = 1.f,
};

const config_t* config() { return &g_config; }

void set_config(const config_t* new_config)
{
    char must_recalculate = (g_config.distance_up != new_config->distance_up ||
                             g_config.distance_to_wall != new_config->distance_to_wall ||
                             g_config.picture_size != new_config->picture_size ||
                             g_config.steps_per_revolution_pitch != new_config->steps_per_revolution_pitch ||
                             g_config.steps_per_revolution_yaw != new_config->steps_per_revolution_yaw);

    g_config = *new_config;
    if (!must_recalculate)
        return;

    const char should_restart = motor_is_running();
    if (should_restart)
        stop_motor();

    calculate_grid_points();

    // FIXME: recalculate pictures

    if (should_restart)
        start_motor();
}

void set_config_from_argv(const int argc, const char* argv[])
{
    assert(argc == 7);

    config_t cfg;
    cfg.distance_up = (float)atof(argv[1]);
    cfg.distance_to_wall = (float)atof(argv[2]);
    cfg.picture_size = (float)atof(argv[3]);
    cfg.steps_per_revolution_yaw = (size_t)atoi(argv[4]);
    cfg.steps_per_revolution_pitch = (size_t)atoi(argv[5]);
    cfg.motor_speed = (size_t)atoi(argv[6]);
 
    fprintf(stderr, "Using parameters: \n");
    fprintf(stderr, "\tdistance_up: %f m\n", cfg.distance_up);
    fprintf(stderr, "\tdistance_to_wall: %f m\n", cfg.distance_to_wall);
    fprintf(stderr, "\tpicture_size: %f m\n", cfg.picture_size);
    fprintf(stderr, "\tsteps_per_revolution_yaw: %lu steps\n", cfg.steps_per_revolution_yaw);
    fprintf(stderr, "\tsteps_per_revolution_pitch: %lu steps\n", cfg.steps_per_revolution_pitch);
    fprintf(stderr, "\tmotor_speed: %lu rpm\n", cfg.motor_speed);

    set_config(&cfg);
}

