#pragma once

#define PICTURE_SIZE 1.f // size of picture in meters
#define DISTANCE_TO_WALL 5.f // distance in z to wall in meters
#define DISTANCE_UP 5.f // distance in y from origin to bottom of canvas

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
