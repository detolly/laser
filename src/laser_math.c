#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <laser_math.h>
#include <config.h>

typedef struct {
    point_t point;
    angles_t angles;
} grid_member_t;

static grid_member_t* calculated_grid_points = NULL;
static size_t calculated_grid_points_length = 0;

static float project_x(float x)
{
    const float pic_size = picture_size();
    return ((x + 1.f) / 2.f) * pic_size - (pic_size / 2);
}

static float project_y(float y)
{
    const float pic_size = picture_size();
    return (((y + 1.f) / 2.f) * pic_size) + pic_size;
}

static uint64_t ns_now(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + (unsigned long long)ts.tv_nsec;
}

void project_point(projection_t* proj, const point_t* point_to_project)
{
    const float projected_x = project_x(point_to_project->x);
    const float projected_y = project_y(point_to_project->y);

    proj->original_point.x = point_to_project->x;
    proj->original_point.y = point_to_project->y;

    proj->projected_point.x = projected_x;
    proj->projected_point.y = projected_y;

    float least_distance = INFINITY;
    size_t least_distance_index = calculated_grid_points_length + 1;

    assert(calculated_grid_points);

#ifdef LASER_DEBUG
    uint64_t ns_before = ns_now();
#endif

    for(size_t i = 0; i < calculated_grid_points_length; i++) {
        float dx = projected_x - calculated_grid_points[i].point.x;
        float dy = projected_y - calculated_grid_points[i].point.y;
        float distance = dx*dx + dy*dy;
        if (distance < least_distance) {
            least_distance = distance;
            least_distance_index = i;
        }
    }

#ifdef LASER_DEBUG
    uint64_t ns_after = ns_now();

    float us = (float)(ns_after - ns_before) / 1000.f;
    printf("searching calculated_grid_points took %f us.\n", us);
#endif

    assert(least_distance_index != calculated_grid_points_length + 1);
    grid_member_t* ptr = &calculated_grid_points[least_distance_index];

    proj->fixed_point.x = ptr->point.x;
    proj->fixed_point.y = ptr->point.y;

    proj->fixed_angles.yaw = ptr->angles.yaw;
    proj->fixed_angles.pitch = ptr->angles.pitch;
}

static float project_angle_yaw(float yaw) { return distance_to_wall() / tanf(yaw); }
static float project_angle_pitch(float pitch) { return tanf(pitch) * distance_to_wall(); }

#define FOV_X 180
#define FOV_Y 90

void calculate_grid_points(void)
{
#ifdef LASER_DEBUG
    uint64_t ns_before = ns_now();
#endif

    const int steps_yaw = steps_per_revolution_yaw();
    const int steps_pitch = steps_per_revolution_pitch();

    if (calculated_grid_points != NULL)
        free(calculated_grid_points);

    size_t i = 0;
    size_t capacity = (unsigned long)steps_yaw;

    calculated_grid_points = (grid_member_t*)malloc(sizeof(grid_member_t)*capacity);
    assert(calculated_grid_points);

    #define push(p) do {                                                                                    \
        if ((size_t)i >= capacity) {                                                                        \
            calculated_grid_points = realloc(calculated_grid_points, capacity*sizeof(grid_member_t)*2);     \
            assert(calculated_grid_points);                                                                 \
            capacity *= 2;                                                                                  \
        }                                                                                                   \
        calculated_grid_points[i++] = p;                                                                    \
    } while(0)

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
            push(p); // lol
        }
    }

    calculated_grid_points_length = (size_t)i;
    set_must_recalculate_pictures(0);

#ifdef LASER_DEBUG
    uint64_t ns_after = ns_now();

    float us = (float)(ns_after - ns_before) / 1000.f;
    printf("calculating grid took %f us.\n", us);
#endif
}

void free_grid_points(void)
{
    free(calculated_grid_points);
    calculated_grid_points = NULL;
}

void make_instruction_pair(motor_instruction_pair_t* instruction_pair, projection_t* p1, projection_t* p2)
{
    const float d_yaw = p1->fixed_angles.yaw - p2->fixed_angles.yaw;
    const float d_pitch = p1->fixed_angles.pitch - p2->fixed_angles.pitch;

    const float steps_yaw = fabsf(d_yaw) / (TWO_PI / (float)steps_per_revolution_yaw());;
    const float steps_pitch = fabsf(d_pitch) / (TWO_PI / (float)steps_per_revolution_pitch());

#ifdef LASER_DEBUG
    printf("steps: %f %f %d %d\n", steps_yaw, steps_pitch, (int)roundf(steps_yaw), (int)roundf(steps_pitch));
#endif

    instruction_pair->yaw.steps = (int)roundf(steps_yaw);
    instruction_pair->yaw.direction = d_yaw < 0.f ? DIRECTION_FORWARD : DIRECTION_BACKWARD;

    instruction_pair->pitch.steps = (int)roundf(steps_pitch);
    instruction_pair->pitch.direction = d_yaw < 0.f ? DIRECTION_FORWARD : DIRECTION_BACKWARD;
}
