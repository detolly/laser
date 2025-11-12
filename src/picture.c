#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <config.h>
#include <laser_math.h>
#include <picture.h>

static void picture_calculate(picture_t* picture)
{
    if (!picture->projections)
        picture->projections = (projection_t*)malloc(sizeof(projection_t) * picture->num_points);

    if (!picture->instructions)
        picture->instructions = (motor_instruction_pair_t*)malloc(sizeof(motor_instruction_pair_t) * (picture->num_points));

    picture->total_pitch = 0.0f;
    picture->total_yaw = 0.0f;

    for(unsigned i = 0; i < picture->num_points; i++) {
        project_point(&picture->projections[i], &picture->points[i]);
        if (i == 0)
            continue;

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
