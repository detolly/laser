#pragma once

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
    int steps;
    direction_t direction;
} motor_instruction_t;

typedef struct {
    motor_instruction_t yaw;
    motor_instruction_t pitch;
} motor_instruction_pair_t;

typedef struct {
    point_t projected_point;
    point_t fixed_point;
    angles_t fixed_angles;
} projection_t;

void project_point(projection_t* point,
                   const point_t* point_to_project);

void make_instruction_pair(motor_instruction_pair_t* instruction_pair,
                           const projection_t* p1,
                           const projection_t* p2);

void calculate_grid_points(void);
void free_grid_points(void);

