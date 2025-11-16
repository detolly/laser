#pragma once

#include <stddef.h>

typedef struct {
    size_t index;
    float coord;
    float angle;
} grid_member_t;

typedef struct {
    grid_member_t* x;
    grid_member_t* y;
    size_t grid_length_x;
    size_t grid_length_y;
} grid_t;

const grid_member_t* binary_search_for_coord(const grid_member_t* member_array,
                                             size_t member_array_size,
                                             float coord);

void grid_make(grid_t* grid,
               float distance_up,
               float distance_to_wall,
               float picture_size,
               size_t steps_per_revolution_yaw,
               size_t steps_per_revolution_pitch);

void grid_free(grid_t* grid);
