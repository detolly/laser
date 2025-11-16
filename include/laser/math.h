#pragma once

#include <assert.h>
#include <math.h>
#include <stddef.h>

#define RAD_TO_DEG (180.f / 3.1415926535f)
#define TWO_PI (2.f * 3.1415926535f)

typedef struct {
    float x;
    float y;
} point_t;

typedef struct {
    float yaw;
    float pitch;
} angles_t;

typedef struct {
    size_t x;
    size_t y;
} indices_t;

static inline float project_x(float x,
                              float picture_size)
{
    const float projected_x = ((x + 1.f) / 2.f) * picture_size - (picture_size / 2);

    assert(x >= -1.f && x <= 1.f);
    assert(projected_x <= picture_size / 2 && projected_x >= -(picture_size / 2));

    return projected_x;
}

static inline float project_y(float y,
                              float picture_size,
                              float distance_up)
{
    const float projected_y = (((y + 1.f) / 2.f) * picture_size) + distance_up;

    assert(y >= -1.f && y <= 1.f);
    assert(projected_y >= distance_up && projected_y <= distance_up + picture_size);

    return projected_y;
}

static inline void project_point(point_t* proj,
                                 const point_t* point_to_project,
                                 float picture_size,
                                 float distance_up)
{
    proj->x = project_x(point_to_project->x, picture_size);
    proj->y = project_y(point_to_project->y, picture_size, distance_up);
}

static inline float project_angle_yaw(const float yaw, const float distance_to_wall)
{
    return tanf(yaw) * distance_to_wall;
}

static inline float project_angle_pitch(const float pitch, const float distance_to_wall)
{
    return tanf(pitch) * distance_to_wall;
}

static inline float ideal_pitch_angle(float distance_up,
                                      float distance_to_wall)
{
    return atanf(distance_up / distance_to_wall);
}

static inline float ideal_yaw_angle(float picture_size,
                                    float distance_to_wall)
{
    return atanf((picture_size / 2.f) / distance_to_wall);
}

