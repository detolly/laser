#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <laser_math.h>
#include <config.h>

typedef struct {
    point_t point;
    angles_t angles;
} grid_member_t;

grid_member_t* calculated_grid_points = NULL;
size_t calculated_grid_points_length = 0;

float pitch(const point_t* p)
{
    return atanf(p->y / distance_to_wall());
}

float yaw(const point_t* p)
{
    return atan2f(distance_to_wall(), p->x);
}

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

    for(size_t i = 0; i < calculated_grid_points_length; i++) {
        float dx = projected_x - calculated_grid_points[i].point.x;
        float dy = projected_y - calculated_grid_points[i].point.y;
        float distance = dx*dx + dy*dy;
        if (distance < least_distance) {
            least_distance = distance;
            least_distance_index = i;
        }
    }

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
}

void free_grid_points(void)
{
    free(calculated_grid_points);
    calculated_grid_points = NULL;
}
