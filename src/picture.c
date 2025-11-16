#include <laser/debug.h>
#include <laser/picture.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <laser/config.h>
#include <laser/math.h>

static grid_t grid = {0};

static picture_t pictures[100] = {0};
static size_t num_pictures = 0;

static void make_instruction_pair_between_points(motor_instruction_pair_t* instruction_pair,
                                                 const picture_point_t* current,
                                                 const picture_point_t* previous)
{
    const int dix = (int)current->indices.x - (int)previous->indices.x;
    const int diy = (int)current->indices.y - (int)previous->indices.y;

    assert(dix < 32768);
    assert(diy < 32768);

    instruction_pair->yaw.steps = (uint16_t)abs(dix);
    instruction_pair->pitch.steps = (uint16_t)abs(diy);

    instruction_pair->yaw.direction = dix > 0 ? DIRECTION_FORWARD : DIRECTION_BACKWARD;
    instruction_pair->pitch.direction = diy > 0 ? DIRECTION_FORWARD : DIRECTION_BACKWARD;
}

static void make_instructions_and_accumulate_stats(motor_instruction_pair_t* ip,
                                                   const picture_point_t* current,
                                                   const picture_point_t* previous,
                                                   picture_t* picture,
                                                   int* symmetric_steps_yaw,
                                                   int* symmetric_steps_pitch)
{
    make_instruction_pair_between_points(ip, current, previous);

    picture->total_yaw_angle += fabsf(current->grid_angles.yaw - previous->grid_angles.yaw);
    picture->total_pitch_angle += fabsf(current->grid_angles.pitch - previous->grid_angles.pitch);

    picture->total_yaw_steps += ip->yaw.steps;
    picture->total_pitch_steps += ip->pitch.steps;

    *symmetric_steps_yaw += (int)ip->yaw.steps * (ip->yaw.direction == DIRECTION_FORWARD ? 1 : -1);
    *symmetric_steps_pitch += (int)ip->pitch.steps * (ip->pitch.direction == DIRECTION_FORWARD ? 1 : -1);
}

static void find_grid_point_closest_to_projected_point(const grid_member_t** x,
                                                       const grid_member_t** y,
                                                       const point_t* projected_point)
{
    assert(grid.x && grid.y);

    *x = binary_search_for_coord(grid.x, grid.grid_length_x, projected_point->x);
    *y = binary_search_for_coord(grid.y, grid.grid_length_y, projected_point->y);

}

static void make_picture_point(picture_point_t* picture_point,
                               const point_t* point,
                               float picture_size,
                               float distance_up)
{
    project_point(&picture_point->projected_point, point, picture_size, distance_up);

    const grid_member_t* x = NULL;
    const grid_member_t* y = NULL;

    find_grid_point_closest_to_projected_point(&x, &y, &picture_point->projected_point);

    picture_point->grid_point.x = x->coord;
    picture_point->grid_point.y = y->coord;

    picture_point->grid_angles.yaw = x->angle;
    picture_point->grid_angles.pitch = y->angle;

    picture_point->indices.x = x->index;
    picture_point->indices.y = y->index;
}

static void picture_calculate(picture_t* picture)
{
    if (!picture->picture_points)
        picture->picture_points = (picture_point_t*)malloc(sizeof(picture_point_t) * picture->num_points);

    if (!picture->instructions)
        picture->instructions = (motor_instruction_pair_t*)malloc(sizeof(motor_instruction_pair_t) * (picture->num_points));

    picture->total_pitch_angle = 0.0f;
    picture->total_yaw_angle = 0.0f;

    int steps_yaw = 0;
    int steps_pitch = 0;

    const config_t* cfg = config();

    float picture_size = cfg->picture_size;
    float distance_up = cfg->distance_up;

    for(unsigned i = 0; i < picture->num_points; i++) {
        make_picture_point(&picture->picture_points[i], &picture->points[i], picture_size, distance_up);
        if (i == 0)
            continue;

        make_instructions_and_accumulate_stats(&picture->instructions[i - 1],
                                               &picture->picture_points[i],
                                               &picture->picture_points[i - 1],
                                               picture,
                                               &steps_yaw,
                                               &steps_pitch);
    }

    make_instructions_and_accumulate_stats(&picture->instructions[picture->num_points - 1],
                                           &picture->picture_points[0],
                                           &picture->picture_points[picture->num_points - 1],
                                           picture,
                                           &steps_yaw,
                                           &steps_pitch);

    DEBUG("symmetrical steps: %d %d\n", steps_yaw, steps_pitch);
    DEBUG("total steps %d %d\n", picture->total_yaw_steps, picture->total_pitch_steps);

    assert(steps_yaw == 0);
    assert(steps_pitch == 0);
}

void picture_free(picture_t* picture)
{
    if (picture->points) {
        free(picture->points);
        picture->points = NULL;
    }
    if (picture->picture_points) {
        free(picture->picture_points);
        picture->picture_points = NULL;
    }
    if (picture->instructions) {
        free(picture->instructions);
        picture->instructions = NULL;
    }
}

void picture_from_points(picture_t* picture,
                         const point_t* points,
                         size_t num_points)
{
    picture->points = (point_t*)malloc(sizeof(point_t)*num_points);
    picture->num_points = num_points;

    memcpy(picture->points, points, sizeof(point_t) * num_points);
    picture_calculate(picture);
}

// managed pictures

picture_t* managed_picture_from_points(const point_t* points,
                                       size_t num_points)
{
    picture_t* pic = &pictures[num_pictures++];
    picture_from_points(pic, points, num_points);
    return pic;
}

void free_managed_pictures()
{
    for(size_t i = 0; i < num_pictures; i++)
        picture_free(&pictures[i]);

    grid_free(&grid);
}

void recalculate_managed_pictures()
{
#ifdef LASER_DEBUG
    const uint64_t ns_before = ns_now();
#endif

    const config_t* cfg = config();
    grid_make(&grid,
              cfg->distance_up,
              cfg->distance_to_wall,
              cfg->picture_size,
              cfg->steps_per_revolution_yaw,
              cfg->steps_per_revolution_pitch);

    for(size_t i = 0; i < num_pictures; i++)
        picture_calculate(&pictures[i]);

#ifdef LASER_DEBUG
    uint64_t ns_after = ns_now();

    float us = (float)(ns_after - ns_before) / 1000.f;
    DEBUG("recalculating managed pictures took %f us.\n", us);
#endif
}
