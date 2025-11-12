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

#include <laser_math.h>
#include <config.h>

typedef struct {
    point_t point;
    angles_t angles;
} grid_member_t;

static grid_member_t* g_grid_points = NULL;
static size_t grid_points_length = 0;

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

void project_point(projection_t* proj, const point_t* point_to_project)
{
    const float projected_x = project_x(point_to_project->x);
    const float projected_y = project_y(point_to_project->y);

    proj->projected_point.x = projected_x;
    proj->projected_point.y = projected_y;

    float least_distance = INFINITY;
    size_t least_distance_index = grid_points_length + 1;

    assert(g_grid_points);

#ifdef LASER_DEBUG
    const uint64_t ns_before = ns_now();
#endif

    for(size_t i = 0; i < grid_points_length; i++) {
        const float dx = projected_x - g_grid_points[i].point.x;
        const float dy = projected_y - g_grid_points[i].point.y;
        const float distance = dx*dx + dy*dy;
        if (distance < least_distance) {
#ifdef LASER_DEBUG
            // fprintf(stderr, "new least distance: previous: %f now: %f %lu\n", least_distance, distance, least_distance_index);
#endif
            least_distance = distance;
            least_distance_index = i;
        }
    }

    assert(least_distance_index != grid_points_length + 1);
    grid_member_t* grid_point = &g_grid_points[least_distance_index];

    proj->fixed_point.x = grid_point->point.x;
    proj->fixed_point.y = grid_point->point.y;

    proj->fixed_angles.yaw = grid_point->angles.yaw;
    proj->fixed_angles.pitch = grid_point->angles.pitch;

#ifdef LASER_DEBUG
    uint64_t ns_after = ns_now();

    float us = (float)(ns_after - ns_before) / 1000.f;
    fprintf(stderr, "searching calculated_grid_points took %f us.\n", us);
    // fprintf(stderr, "grid index: %lu\n", least_distance_index);
    // fprintf(stderr, "grid x: %f y: %f\n", grid_point->point.x, grid_point->point.y);
#endif
}

static float project_angle_yaw(float yaw) { return distance_to_wall() / tanf(yaw); }
static float project_angle_pitch(float pitch) { return tanf(pitch) * distance_to_wall(); }

void calculate_grid_points(void)
{
#ifdef LASER_DEBUG
    uint64_t ns_before = ns_now();
#endif

    const int steps_yaw = steps_per_revolution_yaw();
    const int steps_pitch = steps_per_revolution_pitch();

    if (g_grid_points != NULL)
        free(g_grid_points);

    size_t i = 0;
    size_t capacity = (unsigned long)steps_yaw;

    g_grid_points = (grid_member_t*)malloc(sizeof(grid_member_t)*capacity);
    assert(g_grid_points);

    #define push(p) do {                                                                \
        if ((size_t)i >= capacity) {                                                    \
            g_grid_points = realloc(g_grid_points, capacity*sizeof(grid_member_t)*2);   \
            assert(g_grid_points);                                                      \
            capacity *= 2;                                                              \
        }                                                                               \
        g_grid_points[i++] = p;                                                         \
    } while(0)

#define FOV_X 90
#define FOV_Y 90

    assert(steps_pitch % (360 / FOV_X) == 0);
    assert(steps_yaw % (360 / FOV_Y) == 0);

    const float angle_step_yaw = TWO_PI / (float)steps_yaw;
    const float angle_step_pitch = TWO_PI / (float)steps_pitch;

    for(int x = 0; x < (steps_yaw / (360 / FOV_X)); x++) {
        const float yaw = angle_step_yaw * (float)x;
        const float projected_x = project_angle_yaw(yaw);
        if (projected_x > (picture_size() / 2) || projected_x < -(picture_size() / 2))
            continue;

        for(int y = 0; y < (steps_pitch / (360 / FOV_Y)); y++) {
            const float pitch = angle_step_pitch * (float)y;
            const float projected_y = project_angle_pitch(pitch);
            if (projected_y > (distance_up() + picture_size()) || projected_y < distance_up())
                continue;

// #ifdef LASER_DEBUG
//             fprintf(stderr, "Adding to grid: %f %f\n", projected_x, projected_y);
// #endif
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
            push(p);
            p.point.x = -projected_x;
            p.angles.yaw = -yaw;
            push(p);
        }
    }

    grid_points_length = (size_t)i;
    set_must_recalculate_pictures(0);

#ifdef LASER_DEBUG
    uint64_t ns_after = ns_now();

    float us = (float)(ns_after - ns_before) / 1000.f;
    fprintf(stderr, "calculating grid took %f us.\n", us);

#if 0
    for(unsigned i = 0; i < grid_points_length; i++) {
        fprintf(stderr, "grid: %f %f\n", g_grid_points[i].point.x, g_grid_points[i].point.y);
    }
#endif
#endif
}

void free_grid_points(void)
{
    free(g_grid_points);
    g_grid_points = NULL;
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
#if 0
    fprintf(stderr, "steps: %f %f %d %d\n", steps_yaw, steps_pitch, (int)roundf(steps_yaw), (int)roundf(steps_pitch));
#endif
#endif

    instruction_pair->yaw.steps = (int)roundf(steps_yaw);
    instruction_pair->yaw.direction = d_yaw > 0.f ? DIRECTION_FORWARD : DIRECTION_BACKWARD;

    instruction_pair->pitch.steps = (int)roundf(steps_pitch);
    instruction_pair->pitch.direction = d_pitch > 0.f ? DIRECTION_FORWARD : DIRECTION_BACKWARD;
}
