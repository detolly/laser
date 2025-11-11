#pragma once

#include <stddef.h>

#include <laser_math.h>

typedef enum {
    DIRECTION_FORWARD,
    DIRECTION_BACKWARD,
} direction_t;

typedef struct {
    int steps;
    direction_t direction;
} motor_instruction_t;

typedef struct {
    motor_instruction_t yaw;
    motor_instruction_t pitch;
} motor_instruction_pair_t;

typedef struct {
    point_t* points;
    projection_t* projections;
    motor_instruction_pair_t* instructions;
    size_t num_points;
    int total_yaw;
    int total_pitch;
    char initialized;
} picture_t;

void picture_free(picture_t* picture);
void picture_from_file(picture_t* picture, const char* filename);
void picture_from_points(picture_t* picture, point_t* points, size_t num_points);
