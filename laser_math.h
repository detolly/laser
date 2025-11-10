#pragma once

#define PICTURE_SIZE 1.f // size of picture in meters
#define DISTANCE_TO_WALL 5.f // distance in z to wall in meters
#define DISTANCE_UP 5.f // distance in y from origin to bottom of canvas

typedef struct {
    float x;
    float y;
} point;

typedef struct {
    float yaw;   // x
    float pitch; // y
} angles;

angles angles_between_projected_vectors(point previous, point next);
point project(point p);
