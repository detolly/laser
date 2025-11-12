#include <assert.h>
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

    int steps_yaw = 0;
    int steps_pitch = 0;

    for(unsigned i = 0; i < picture->num_points; i++) {
        project_point(&picture->projections[i], &picture->points[i]);
        if (i == 0)
            continue;

        motor_instruction_pair_t* ip = &picture->instructions[i - 1];
        make_instruction_pair(ip, &picture->projections[i], &picture->projections[i - 1]);

        picture->total_yaw += ip->yaw.steps;
        picture->total_pitch += ip->pitch.steps;

        steps_yaw += ip->yaw.steps * (ip->yaw.direction == DIRECTION_FORWARD ? 1 : -1);
        steps_pitch += ip->pitch.steps * (ip->pitch.direction == DIRECTION_FORWARD ? 1 : -1);
    }

    motor_instruction_pair_t* ip = &picture->instructions[picture->num_points - 1];
    make_instruction_pair(ip, &picture->projections[picture->num_points - 1], &picture->projections[0]);

    picture->total_yaw += ip->yaw.steps;
    picture->total_pitch += ip->pitch.steps;

    steps_yaw += ip->yaw.steps * (ip->yaw.direction == DIRECTION_FORWARD ? 1 : -1);
    steps_pitch += ip->pitch.steps * (ip->pitch.direction == DIRECTION_FORWARD ? 1 : -1);

    fprintf(stderr, "symmetrical steps: %d %d\n", steps_yaw, steps_pitch);

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
