#include <laser_math.h>

#ifdef LASER_DEBUG
#define _POSIX_C_SOURCE 200809L
#endif

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef LASER_DEBUG
#include <time.h>
#endif

#include <config.h>

typedef struct {
    size_t index;
    float coord;
    float angle;
} grid_member_t;

static grid_member_t* grid_members_x = NULL;
static grid_member_t* grid_members_y = NULL;

static size_t grid_length_x = 0;
static size_t grid_length_y = 0;

static float project_x(float x)
{
    assert(x >= -1.f && x <= 1.f);
    const float pic_size = picture_size();

    const float projected_x = ((x + 1.f) / 2.f) * pic_size - (pic_size / 2);
    assert(projected_x <= pic_size / 2 && projected_x >= -(pic_size / 2));

    return projected_x;
}

static float project_y(float y)
{
    assert(y >= -1.f && y <= 1.f);
    const float pic_size = picture_size();
    const float dist_up = distance_up();

    const float projected_y = (((y + 1.f) / 2.f) * pic_size) + dist_up;
    assert(projected_y >= dist_up && projected_y <= dist_up + pic_size);

    return projected_y;
}

#ifdef LASER_DEBUG
static uint64_t ns_now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (unsigned long long)ts.tv_nsec;
}
#endif

static grid_member_t* search_for_coord(grid_member_t* member_arr, size_t size, float coord)
{
    int current_index = 0;
    int left = 0;
    int right = (int)size - 1;
    grid_member_t* current_grid_member = NULL;

    while(left <= right) {
        current_index = left + (right - left) / 2;
        current_grid_member = &member_arr[current_index];

        if (current_grid_member->coord < coord)
            left = current_index + 1;
        else if (current_grid_member->coord > coord)
            right = current_index - 1;
        else break;
    }

    return current_grid_member;
}

static void find_grid_point_closest_to_projected_point(grid_member_t** x,
                                                       grid_member_t** y,
                                                       const point_t* projected_point)
{
    assert(grid_members_x && grid_members_y);

#ifdef LASER_DEBUG
    const uint64_t ns_before = ns_now();
#endif

    *x = search_for_coord(grid_members_x, grid_length_x, projected_point->x);
    *y = search_for_coord(grid_members_y, grid_length_y, projected_point->y);

#ifdef LASER_DEBUG
    uint64_t ns_after = ns_now();

    float us = (float)(ns_after - ns_before) / 1000.f;
    fprintf(stderr, "searching calculated_grid_points took %f us.\n", us);
    // fprintf(stderr, "grid index: %lu\n", least_distance_index);
    // fprintf(stderr, "grid x: %f y: %f\n", grid_point->point.x, grid_point->point.y);
#endif
}

void project_point(projection_t* proj, const point_t* point_to_project)
{
    const float projected_x = project_x(point_to_project->x);
    const float projected_y = project_y(point_to_project->y);

    proj->projected_point.x = projected_x;
    proj->projected_point.y = projected_y;

    grid_member_t* x = NULL;
    grid_member_t* y = NULL;

    find_grid_point_closest_to_projected_point(&x, &y, &proj->projected_point);

    proj->grid_point.x = x->coord;
    proj->grid_point.y = y->coord;

    proj->grid_angles.yaw = x->angle;
    proj->grid_angles.pitch = y->angle;

    proj->indices.x = x->index;
    proj->indices.y = y->index;
}

static float project_angle_yaw(const float yaw) { return tanf(yaw) * distance_to_wall(); }
static float project_angle_pitch(const float pitch) { return tanf(pitch) * distance_to_wall(); }

void calculate_grid_points(void)
{
#ifdef LASER_DEBUG
    uint64_t ns_before = ns_now();
#endif

    const int steps_yaw = steps_per_revolution_yaw();
    const int steps_pitch = steps_per_revolution_pitch();

    const float angle_step_yaw = TWO_PI / (float)steps_yaw;
    const float angle_step_pitch = TWO_PI / (float)steps_pitch;

    const float wanted_y_start_angle = atanf(distance_up() / distance_to_wall());
    const float wanted_y_stop_angle = atanf((picture_size() + distance_up()) / distance_to_wall());
    const float wanted_x_stop_angle = atanf((picture_size() / 2) / distance_to_wall());

    const size_t start_y = (size_t)(wanted_y_start_angle / angle_step_pitch) + 1;
    const size_t stop_y = (size_t)(wanted_y_stop_angle / angle_step_pitch) + 1;
    const size_t stop_x = (size_t)(wanted_x_stop_angle / angle_step_yaw) + 1;

    if (grid_members_x != NULL)
        free(grid_members_x);
    if (grid_members_y != NULL)
        free(grid_members_y);

    grid_length_x = stop_x * 2;
    grid_length_y = stop_y - start_y;

    grid_members_x = (grid_member_t*)malloc(sizeof(grid_member_t) * grid_length_x);
    grid_members_y = (grid_member_t*)malloc(sizeof(grid_member_t) * grid_length_y);
    assert(grid_members_x && grid_members_y);

    for(size_t x = 0; x < stop_x; x++) {
        const float yaw = angle_step_yaw * (float)x;
        const float projected_x = project_angle_yaw(yaw);
        grid_members_x[x + grid_length_x / 2].coord = projected_x;
        grid_members_x[x + grid_length_x / 2].angle = yaw;
        grid_members_x[x + grid_length_x / 2].index = x + grid_length_x / 2;
        grid_members_x[grid_length_x / 2 - x - 1].coord = -projected_x;
        grid_members_x[grid_length_x / 2 - x - 1].angle = -yaw;
        grid_members_x[grid_length_x / 2 - x - 1].index = grid_length_x / 2 - x - 1;
    }

    for(size_t y = 0; y < grid_length_y; y++) {
        const float pitch = angle_step_pitch * (float)y;
        grid_members_y[y].coord = project_angle_pitch(pitch);
        grid_members_y[y].angle = pitch;
        grid_members_x[y].index = y;
    }

#ifdef LASER_DEBUG
    for(size_t x = 0; x < grid_length_x; x++) {
        const grid_member_t* member = &grid_members_x[x];
        fprintf(stderr, "x: %f %f\n", member->coord, member->angle);
        if (x == 0)
            continue;

        const grid_member_t* member_before = &grid_members_x[x - 1];
        assert(member->coord >= member_before->coord);
        assert(member->angle >= member_before->angle);
    }
    for(size_t y = 0; y < grid_length_y; y++) {
        const grid_member_t* member = &grid_members_y[y];
        fprintf(stderr, "y: %f %f\n", member->coord, member->angle);
        if (y == 0)
            continue;
        const grid_member_t* member_before = &grid_members_y[y - 1];
        assert(member->coord >= member_before->coord);
        assert(member->angle >= member_before->angle);
    }
#endif

    set_must_recalculate_pictures(0);

#ifdef LASER_DEBUG
    uint64_t ns_after = ns_now();

    float us = (float)(ns_after - ns_before) / 1000.f;
    fprintf(stderr, "calculating grid took %f us.\n", us);
#endif
}

void free_grid_points(void)
{
    free(grid_members_x);
    free(grid_members_y);
    grid_members_x = NULL;
    grid_members_y = NULL;
}

