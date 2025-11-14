#include <config.h>

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
    bool must_recalculate = (g_config.distance_up != new_config->distance_up ||
                             g_config.distance_to_wall != new_config->distance_to_wall ||
                             g_config.picture_size != new_config->picture_size ||
                             g_config.steps_per_revolution_pitch != new_config->steps_per_revolution_pitch ||
                             g_config.steps_per_revolution_yaw != new_config->steps_per_revolution_yaw);

    g_config = *new_config;

    if (must_recalculate) {
        stop_motor();
        calculate_grid_points();
        // FIXME: recalculate pictures
        start_motor();
    }
}
