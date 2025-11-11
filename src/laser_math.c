#include <math.h>

#include <laser_math.h>
#include <config.h>

static float pitch(const point_t* p)
{
    return atanf(p->y / *distance_to_wall());
}

static float yaw(const point_t* p)
{
    return atan2f(*distance_to_wall(), p->x);
}

void angles_between_projected_points(angles_t* angles,
                                     const point_t* previous,
                                     const point_t* next)
{
    const float previous_yaw = yaw(previous);
    const float next_yaw = yaw(next);

    const float previous_pitch = pitch(previous);
    const float next_pitch = pitch(next);

    const float d_yaw = previous_yaw - next_yaw;            // higher angles are to the left
    const float d_pitch = next_pitch - previous_pitch;

    angles->yaw.value = fabsf(d_yaw);
    angles->yaw.direction = (d_yaw > 0 ? DIRECTION_POS : DIRECCTION_NEG);

    angles->pitch.value = fabsf(d_pitch);
    angles->pitch.direction = (d_pitch > 0 ? DIRECTION_POS : DIRECCTION_NEG);
}

static float project_x(float x)
{
    const float pic_size = *picture_size();
    return (((x + 1.f) / 2.f) * pic_size) - pic_size / 2;
}

static float project_y(float y)
{
    const float pic_size = *picture_size();
    return (((y + 1.f) / 2.f) * pic_size) + pic_size;
}

void project_point(point_t* point, const point_t* point_to_project)
{
    point->x = project_x(point_to_project->x);
    point->y = project_y(point_to_project->y);
}
