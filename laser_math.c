
#include <laser_math.h>
#include <math.h>

static float pitch(point p)
{
    return atanf(p.y / DISTANCE_TO_WALL);
}

static float yaw(point p)
{
    return atan2f(DISTANCE_TO_WALL, p.x);
}

angles angles_between_projected_vectors(point previous, point next)
{
    const float previous_yaw = yaw(previous);
    const float next_yaw = yaw(next);

    const float previous_pitch = pitch(previous);
    const float next_pitch = pitch(next);

    const float d_yaw = next_yaw - previous_yaw;
    const float d_pitch = next_pitch - previous_pitch;

    const angles r = {
        next.x > previous.x ? d_yaw : d_yaw * -1.f,
        next.y > previous.y ? d_pitch : d_pitch * -1.f,
    };

    return r;
}

static float project_x(float x) { return ((x + 2.f) / 2) * PICTURE_SIZE; }
static float project_y(float y) { return ((y + 2.f) / 2) * PICTURE_SIZE + DISTANCE_UP; }

point project(point p)
{
    point ret = {
        .x = project_x(p.x),
        .y = project_y(p.y)
    };

    return ret;
}
