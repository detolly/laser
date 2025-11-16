#include <laser/debug.h>
#include <laser/grid.h>

#include <stddef.h>
#include <stdlib.h>

#include <laser/math.h>

void grid_make(grid_t* grid,
               float distance_up,
               float distance_to_wall,
               float picture_size,
               size_t steps_per_revolution_yaw,
               size_t steps_per_revolution_pitch)
{
    const float angle_step_yaw = TWO_PI / (float)steps_per_revolution_yaw;
    const float angle_step_pitch = TWO_PI / (float)steps_per_revolution_pitch;

    const size_t start_y = (size_t)(ideal_pitch_start_angle(distance_up, distance_to_wall) / angle_step_pitch) + 1;
    const size_t stop_y = (size_t)(ideal_pitch_stop_angle(picture_size + distance_up, distance_to_wall) / angle_step_pitch) + 1;
    const size_t start_x = (size_t)(ideal_yaw_start_angle() / angle_step_yaw);
    const size_t stop_x = (size_t)(ideal_yaw_stop_angle(picture_size, distance_to_wall) / angle_step_yaw) + 1;

    DEBUG("%lu -> %lu | %lu -> %lu\n", start_x, stop_x, start_y, stop_y);

    const size_t grid_length_x = (stop_x - start_x) * 2;
    const size_t grid_length_y = stop_y - start_y;

    grid->grid_length_x = grid_length_x;
    grid->grid_length_y = grid_length_y;

    grid->x = (grid->x != NULL) ? 
          (grid_member_t*)realloc(grid->x, sizeof(grid_member_t) * grid_length_x)
        : (grid_member_t*)malloc(sizeof(grid_member_t) * grid_length_x);

    grid->y = (grid->y != NULL) ? 
          (grid_member_t*)realloc(grid->y, sizeof(grid_member_t) * grid_length_y)
        : (grid_member_t*)malloc(sizeof(grid_member_t) * grid_length_y);

    assert(grid->x && grid->y);

    for(size_t x = 0; x < stop_x; x++) {
        const float yaw = angle_step_yaw * (float)x;
        const float projected_x = project_angle_yaw(yaw, distance_to_wall);

        const size_t positive_index = x + grid_length_x / 2;
        grid->x[positive_index].coord = projected_x;
        grid->x[positive_index].angle = yaw;
        grid->x[positive_index].index = positive_index;

        const size_t negative_index = grid_length_x / 2 - x - 1;
        grid->x[negative_index].coord = -projected_x;
        grid->x[negative_index].angle = -yaw;
        grid->x[negative_index].index = negative_index;
    }

    for(size_t y = 0; y < grid_length_y; y++) {
        const float pitch = angle_step_pitch * (float)(y + start_y);
        grid->y[y].coord = project_angle_pitch(pitch, distance_to_wall);
        grid->y[y].angle = pitch;
        grid->y[y].index = y;
    }

#ifdef LASER_DEBUG
    for(size_t x = 0; x < grid_length_x; x++) {
        const grid_member_t* member = &grid->x[x];
        DEBUG("x: %f %f\n", member->coord, member->angle);
        if (x == 0) continue;

        const grid_member_t* member_before = &grid->x[x - 1];
        assert(member->coord >= member_before->coord);
        assert(member->angle >= member_before->angle);
    }
    for(size_t y = 0; y < grid_length_y; y++) {
        const grid_member_t* member = &grid->y[y];
        DEBUG("y: %f %f\n", member->coord, member->angle);
        if (y == 0) continue;

        const grid_member_t* member_before = &grid->y[y - 1];
        assert(member->coord >= member_before->coord);
        assert(member->angle >= member_before->angle);
    }
#endif
}

const grid_member_t* binary_search_for_coord(const grid_member_t* member_arr,
                                             size_t size,
                                             float coord)
{
    int current_index = 0;
    int left = 0;
    int right = (int)size - 1;

    const grid_member_t* current_grid_member = NULL;
    while(left <= right) {
        current_index = left + (right - left) / 2;
        current_grid_member = &member_arr[current_index];

        if (current_grid_member->coord < coord)
            left = current_index + 1;
        else if (current_grid_member->coord > coord)
            right = current_index - 1;
        else break;
    }

    assert(current_grid_member != NULL);
    return current_grid_member;
}


void grid_free(grid_t* grid)
{
    if (grid->x)
        free(grid->x);

    if (grid->y)
        free(grid->y);
}
