#include <picture.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <config.h>
#include <laser_math.h>

static void make_instruction_pair_between_points(motor_instruction_pair_t* instruction_pair,
                                                 const projection_t* current,
                                                 const projection_t* previous)
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

static void process_points(motor_instruction_pair_t* ip,
                           const projection_t* current,
                           const projection_t* previous,
                           picture_t* picture,
                           int* steps_yaw,
                           int* steps_pitch)
{
    make_instruction_pair_between_points(ip, current, previous);

    picture->total_yaw_angle += fabsf(current->grid_angles.yaw - previous->grid_angles.yaw);
    picture->total_pitch_angle += fabsf(current->grid_angles.pitch - previous->grid_angles.pitch);

    picture->total_yaw += ip->yaw.steps;
    picture->total_pitch += ip->pitch.steps;

    *steps_yaw += (int)ip->yaw.steps * (ip->yaw.direction == DIRECTION_FORWARD ? 1 : -1);
    *steps_pitch += (int)ip->pitch.steps * (ip->pitch.direction == DIRECTION_FORWARD ? 1 : -1);
}

static void picture_calculate(picture_t* picture)
{
    if (!picture->projections)
        picture->projections = (projection_t*)malloc(sizeof(projection_t) * picture->num_points);

    if (!picture->instructions)
        picture->instructions = (motor_instruction_pair_t*)malloc(sizeof(motor_instruction_pair_t) * (picture->num_points));

    picture->total_pitch = 0.0f;
    picture->total_yaw = 0.0f;

    int steps_yaw = 0;
    int steps_pitch = 0;

    for(unsigned i = 0; i < picture->num_points; i++) {
        project_point(&picture->projections[i], &picture->points[i]);
        if (i == 0)
            continue;

        process_points(&picture->instructions[i - 1],
                       &picture->projections[i],
                       &picture->projections[i - 1],
                       picture,
                       &steps_yaw,
                       &steps_pitch);
    }

    process_points(&picture->instructions[picture->num_points - 1],
                   &picture->projections[0],
                   &picture->projections[picture->num_points - 1],
                   picture,
                   &steps_yaw,
                   &steps_pitch);

#ifdef LASER_DEBUG
    fprintf(stderr, "symmetrical steps: %d %d\n", steps_yaw, steps_pitch);
    fprintf(stderr, "total steps %d %d\n", picture->total_yaw, picture->total_pitch);
#endif

    assert(steps_yaw == 0);
    assert(steps_pitch == 0);
}

void picture_free(picture_t* picture)
{
    if (picture->points) {
        free(picture->points);
        picture->points = NULL;
    }
    if (picture->projections) {
        free(picture->projections);
        picture->projections = NULL;
    }
    if (picture->instructions) {
        free(picture->instructions);
        picture->instructions = NULL;
    }
}

void picture_from_file(picture_t* picture, const char* filename)
{
    //...
}

void picture_from_points(picture_t* picture, const point_t* points, size_t num_points)
{
    picture->points = (point_t*)malloc(sizeof(point_t)*num_points);
    picture->num_points = num_points;

    memcpy(picture->points, points, sizeof(point_t) * num_points);
    picture_calculate(picture);
}
