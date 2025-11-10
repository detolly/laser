#pragma once

#define PICTURE_SIZE 1.f // size of picture in meters
#define DISTANCE_TO_WALL 5.f // distance in z to wall in meters
#define DISTANCE_UP 5.f // distance in y from origin to bottom of canvas

typedef struct {
    float x;
    float y;
} point_t;

typedef struct {
    float yaw;   // x
    float pitch; // y
} angles_t;

angles_t angles_between_projected_vectors(point_t previous, point_t next);
point_t project(point_t p);
