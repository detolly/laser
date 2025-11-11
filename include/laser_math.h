#pragma once

typedef enum {
    DIRECTION_POS,
    DIRECCTION_NEG
} direction_t;

typedef struct {
    float x;
    float y;
} point_t;

typedef struct {
    float value;
    direction_t direction;
} angle_t;

typedef struct {
    angle_t yaw;   // x
    angle_t pitch; // y
} angles_t;

void angles_between_projected_points(angles_t* angles,
                                     const point_t* previous,
                                     const point_t* next);

void project_point(point_t* point,
                   const point_t* point_to_project);
