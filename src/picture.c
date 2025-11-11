#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <config.h>
#include <laser_math.h>
#include <picture.h>

static void make_instruction_pair(motor_instruction_pair_t* instruction_pair, projection_t* p1, projection_t* p2)
{
    const float d_yaw = p1->fixed_angles.yaw - p2->fixed_angles.yaw;
    const float d_pitch = p1->fixed_angles.pitch - p2->fixed_angles.pitch;

    instruction_pair->yaw.steps = (int)(fabsf(d_yaw) / (TWO_PI / (float)steps_per_revolution_yaw()));
    instruction_pair->yaw.direction = d_yaw < 0.f ? DIRECTION_FORWARD : DIRECTION_BACKWARD;

    instruction_pair->pitch.steps = (int)(fabsf(d_pitch) / (TWO_PI / (float)steps_per_revolution_pitch()));
    instruction_pair->pitch.direction = d_yaw < 0.f ? DIRECTION_FORWARD : DIRECTION_BACKWARD;
}

static void picture_calculate(picture_t* picture)
{
    if (picture->projections)
        free(picture->projections);

    if (picture->instructions)
        free(picture->instructions);

    picture->projections = (projection_t*)malloc(sizeof(projection_t)*picture->num_points);
    picture->instructions = (motor_instruction_pair_t*)malloc(sizeof(motor_instruction_pair_t)*(picture->num_points));

    picture->total_pitch = 0.0f;
    picture->total_yaw = 0.0f;

    for(unsigned i = 0; i < picture->num_points; i++) {
        project_point(&picture->projections[i], &picture->points[i]);
        if (i == 0) continue;

        motor_instruction_pair_t instruction_pair = {0};
        make_instruction_pair(&instruction_pair, &picture->projections[i], &picture->projections[i - 1]);

        picture->instructions[i - 1] = instruction_pair;

        picture->total_yaw += instruction_pair.yaw.steps;
        picture->total_pitch += instruction_pair.pitch.steps;
    }

    motor_instruction_pair_t instruction_pair = {0};
    make_instruction_pair(&instruction_pair, &picture->projections[picture->num_points - 1], &picture->projections[0]);
    picture->instructions[picture->num_points - 1] = instruction_pair;
    picture->total_yaw += instruction_pair.yaw.steps;
    picture->total_pitch += instruction_pair.pitch.steps;
}

void picture_free(picture_t* picture)
{
    if (!picture->initialized)
        return;

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

void picture_from_points(picture_t* picture, point_t* points, size_t num_points)
{
    picture->points = (point_t*)malloc(sizeof(point_t)*num_points);
    picture->num_points = num_points;

    memcpy(picture->points, points, sizeof(point_t) * num_points);
    picture_calculate(picture);

    picture->initialized = 1;
}
