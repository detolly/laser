
#include <config.h>

volatile float g_picture_size = 0.2f;
volatile float g_distance_to_wall = 5.0f;
volatile float g_distance_up = 1.0f;

float picture_size()
{
    return g_picture_size;
}

float distance_to_wall()
{
    return g_picture_size;
}

float distance_up()
{
    return g_distance_up;
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
