
#include <config.h>

float g_picture_size = 0.2f;
float g_distance_to_wall = 5.0f;
float g_distance_up = 1.0f;

const float* picture_size(void)
{
    return &g_picture_size;
}

const float* distance_to_wall(void)
{
    return &g_picture_size;
}

const float* distance_up(void)
{
    return &g_distance_up;
}

void set_picture_size(float new_picture_size)
{
    g_picture_size = new_picture_size;
}

void set_distance_to_wall(float new_distance_to_wall)
{
    g_distance_to_wall = new_distance_to_wall;
}

void set_distance_up(float new_distance_up)
{
    g_distance_up = new_distance_up;
}
