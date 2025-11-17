#pragma once

#include <stddef.h>

#define MICROSECONDS_PER_SECOND 1000000

#define YAW_DIRECTION_GPIO 13
#define YAW_PULSE_GPIO 6

#define PITCH_DIRECTION_GPIO 26
#define PITCH_PULSE_GPIO 19

#define DIRECTION_SLEEP_TIME 5
#define BETWEEN_PULSE_SLEEP_TIME_US 3 // 200k Hz, 2.5 us per pulse

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
void set_config_from_argv(const int argc, const char* argv[]);
