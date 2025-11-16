#pragma once

#include <stddef.h>
#include <stdint.h>

#include <laser/grid.h>
#include <laser/math.h>

typedef enum {
    DIRECTION_FORWARD,
    DIRECTION_BACKWARD,
} direction_enum_t;

typedef struct __attribute__((packed)) {
    uint16_t steps : 15;
    direction_enum_t direction : 1;
} motor_instruction_t;

typedef struct {
    motor_instruction_t yaw;
    motor_instruction_t pitch;
} motor_instruction_pair_t;

typedef struct {
    point_t projected_point;
    point_t grid_point;
    angles_t grid_angles;
    indices_t indices;
} picture_point_t;

typedef struct {
    point_t* points;
    picture_point_t* picture_points;
    motor_instruction_pair_t* instructions;
    size_t num_points;
    int total_yaw_steps;
    int total_pitch_steps;
    float total_yaw_angle;
    float total_pitch_angle;
} picture_t;

void picture_free(picture_t* picture);
void picture_from_points(picture_t* picture, const point_t* points, size_t num_points);

picture_t* managed_picture_from_points(const point_t* points, size_t num_points);
void free_managed_pictures(); 
void recalculate_managed_pictures();
