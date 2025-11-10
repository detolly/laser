#include <math.h>

#include <laser_math.h>

static float pitch(point_t p)
{
    return atanf(p.y / DISTANCE_TO_WALL);
}

static float yaw(point_t p)
{
    return atan2f(DISTANCE_TO_WALL, p.x);
}

angles_t angles_between_projected_vectors(point_t previous, point_t next)
{
    const float previous_yaw = yaw(previous);
    const float next_yaw = yaw(next);

    const float previous_pitch = pitch(previous);
    const float next_pitch = pitch(next);

    const float d_yaw = next_yaw - previous_yaw;
    const float d_pitch = next_pitch - previous_pitch;

    const angles_t r = {
        next.x > previous.x ? d_yaw : d_yaw * -1.f,
        next.y > previous.y ? d_pitch : d_pitch * -1.f,
    };

    return r;
}

static float project_x(float x) { return (((x + 1.f) / 2.f) * PICTURE_SIZE) - PICTURE_SIZE / 2; }
static float project_y(float y) { return (((y + 1.f) / 2.f) * PICTURE_SIZE) - PICTURE_SIZE / 2 + DISTANCE_UP; }

point_t project(point_t p)
{
    point_t ret = {
        .x = project_x(p.x),
        .y = project_y(p.y)
    };

    return ret;
}
