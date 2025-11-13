#pragma once

#include <stddef.h>

#define RAD_TO_DEG (180.f / 3.1415926535f)
#define TWO_PI (2.f * 3.1415926535f)

typedef struct {
    float x;
    float y;
} point_t;

typedef struct {
    float yaw;   // x
    float pitch; // y
} angles_t;

typedef enum {
    DIRECTION_FORWARD,
    DIRECTION_BACKWARD,
} direction_t;

typedef struct {
    size_t steps;
    direction_t direction;
} motor_instruction_t;

typedef struct {
    motor_instruction_t yaw;
    motor_instruction_t pitch;
} motor_instruction_pair_t;

typedef struct {
    size_t x;
    size_t y;
} indices_t;

typedef struct {
    point_t projected_point;
    point_t grid_point;
    angles_t grid_angles;
    indices_t indices;
} projection_t;

void project_point(projection_t* point,
                   const point_t* point_to_project);

void calculate_grid_points(void);
void free_grid_points(void);

