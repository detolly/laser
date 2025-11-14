#pragma once

#include <stddef.h>

typedef struct {
    size_t steps_per_revolution_yaw;
    size_t steps_per_revolution_pitch;
    size_t motor_speed;

    float picture_size;
    float distance_to_wall;
    float distance_up;
} config_t;

const config_t* config();
void set_config(const config_t* new_config);
