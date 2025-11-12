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
    point_t point;
    angles_t angles;
} grid_member_t;

static grid_member_t* grid_points = NULL;

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

static grid_member_t* find_grid_point_closest_to_grid_point(const point_t* projected_point)
{
    assert(grid_points);

#ifdef LASER_DEBUG
    const uint64_t ns_before = ns_now();
#endif

    const float x = projected_point->x;
    const float y = projected_point->y;

    int current_x = 0;
    int current_y = 0;

    int left = 0;
    int right = (int)grid_length_x - 1;
    grid_member_t* current_grid_member = NULL;

    while(left <= right) {
        // x * grid_y + y + stop_x * grid_y
        current_x = left + (right - left) / 2;
        current_grid_member = &grid_points[current_x * (int)grid_length_y];
        if (current_grid_member->point.x < x)
            left = current_x + 1;
        else if (current_grid_member->point.x > x)
            right = current_x - 1;
        else break;
    }

    left = 0;
    right = (int)grid_length_y - 1;
    while(left <= right) {
        // x * grid_y + y + stop_x * grid_y
        current_y = left + (right - left) / 2;
        current_grid_member = &grid_points[current_x * (int)grid_length_y + current_y];
        if (current_grid_member->point.y < y)
            left = current_y + 1;
        else if (current_grid_member->point.y > y)
            right = current_y - 1;
        else break;
    }

#ifdef LASER_DEBUG
    uint64_t ns_after = ns_now();

    float us = (float)(ns_after - ns_before) / 1000.f;
    fprintf(stderr, "searching calculated_grid_points took %f us.\n", us);
    // fprintf(stderr, "grid index: %lu\n", least_distance_index);
    // fprintf(stderr, "grid x: %f y: %f\n", grid_point->point.x, grid_point->point.y);
#endif

    return current_grid_member;
}

void project_point(projection_t* proj, const point_t* point_to_project)
{
    const float projected_x = project_x(point_to_project->x);
    const float projected_y = project_y(point_to_project->y);

    proj->projected_point.x = projected_x;
    proj->projected_point.y = projected_y;

    grid_member_t* grid_point = find_grid_point_closest_to_grid_point(&proj->projected_point);

    proj->fixed_point.x = grid_point->point.x;
    proj->fixed_point.y = grid_point->point.y;

    proj->fixed_angles.yaw = grid_point->angles.yaw;
    proj->fixed_angles.pitch = grid_point->angles.pitch;
}

static float project_angle_yaw(float yaw) { return tanf(yaw) * distance_to_wall(); }
static float project_angle_pitch(float pitch) { return tanf(pitch) * distance_to_wall(); }

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
    const size_t start_y = (size_t)(wanted_y_start_angle / angle_step_pitch) + 1;

    const float wanted_y_stop_angle = atanf((picture_size() + distance_up()) / distance_to_wall());
    const size_t stop_y = (size_t)(wanted_y_stop_angle / angle_step_pitch) + 1;

    const float wanted_x_stop_angle = atanf((picture_size() / 2) / distance_to_wall());
    const size_t stop_x = (size_t)(wanted_x_stop_angle / angle_step_yaw) + 1;

    if (grid_points != NULL)
        free(grid_points);

    grid_length_x = stop_x * 2;
    grid_length_y = stop_y - start_y;

    const size_t num_grid_items = grid_length_y * grid_length_x;
    grid_points = (grid_member_t*)malloc(sizeof(grid_member_t) * num_grid_items);
    assert(grid_points);

    for(size_t x = 0; x < stop_x; x++) {
        const float yaw = angle_step_yaw * (float)x;
        const float projected_x = project_angle_yaw(yaw);

        for(size_t y = 0; y < stop_y - start_y; y++) {
            const float pitch = angle_step_pitch * (float)(y + start_y);
            const float projected_y = project_angle_pitch(pitch);

            grid_member_t p = {
                .point = {
                    .x = projected_x,
                    .y = projected_y
                },
                .angles = {
                    .yaw = yaw,
                    .pitch = pitch
                }
            };

            grid_points[x * grid_length_y + stop_x * grid_length_y + y] = p;

            // since observer is at x=0 it will be symmetric.
            p.point.x = -projected_x;
            p.angles.yaw = -yaw;

            grid_points[(grid_length_x * grid_length_y / 2 - 1) - (x * grid_length_y + y)] = p;
        }
    }

    set_must_recalculate_pictures(0);

#ifdef LASER_DEBUG
    uint64_t ns_after = ns_now();

    float us = (float)(ns_after - ns_before) / 1000.f;
    fprintf(stderr, "calculating grid took %f us.\n", us);
    // for(size_t i = 0; i < grid_length_x * grid_length_y; i++) {
    //     printf("%f %f\n", g_grid_points[i].point.x, g_grid_points[i].point.y);
    // }
#endif
}

void free_grid_points(void)
{
    free(grid_points);
    grid_points = NULL;
}

void make_instruction_pair(motor_instruction_pair_t* instruction_pair,
                           const projection_t* p1,
                           const projection_t* p2)
{
    const float d_yaw = p2->fixed_angles.yaw - p1->fixed_angles.yaw;
    const float d_pitch = p2->fixed_angles.pitch - p1->fixed_angles.pitch;

    const float steps_yaw = fabsf(d_yaw) * (float)steps_per_revolution_yaw() / TWO_PI;
    const float steps_pitch = fabsf(d_pitch) * (float)steps_per_revolution_pitch() / TWO_PI;

#ifdef LASER_DEBUG
    // fprintf(stderr, "steps angles: %f %f %f %f\n", p2->fixed_angles.yaw, p2->fixed_angles.pitch, p1->fixed_angles.yaw, p1->fixed_angles.pitch);
    // fprintf(stderr, "steps: %f %f  %d %d\n", steps_yaw, steps_pitch, (int)roundf(steps_yaw), (int)roundf(steps_pitch));
#endif

    instruction_pair->yaw.steps = (int)roundf(steps_yaw);
    instruction_pair->yaw.direction = d_yaw > 0.f ? DIRECTION_FORWARD : DIRECTION_BACKWARD;

    instruction_pair->pitch.steps = (int)roundf(steps_pitch);
    instruction_pair->pitch.direction = d_pitch > 0.f ? DIRECTION_FORWARD : DIRECTION_BACKWARD;
}
