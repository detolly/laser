#include <config.h>
#include <laser_math.h>

int g_steps_per_revolution_yaw = 5000;
int g_steps_per_revolution_pitch = 5000;

int steps_per_revolution_yaw(void) { return g_steps_per_revolution_yaw; }
int steps_per_revolution_pitch(void) { return g_steps_per_revolution_pitch; }

void set_steps_per_revolution_yaw(int steps)
{
    g_steps_per_revolution_yaw = steps;
    calculate_grid_points();
}

void set_steps_per_revolution_pitch(int steps)
{
    g_steps_per_revolution_pitch = steps;
    calculate_grid_points();
}

float g_picture_size = 5.f;
float g_distance_to_wall = 10.f;
float g_distance_up = 5.f;
float g_motor_speed = 750.f;

float picture_size(void) { return g_picture_size; }
float distance_to_wall(void) { return g_distance_to_wall; }
float distance_up(void) { return g_distance_up; }
float motor_speed(void) { return g_motor_speed; }

void set_motor_speed(float new_speed)
{
    g_motor_speed = new_speed;
}

void set_picture_size(float new_picture_size)
{
    g_picture_size = new_picture_size;
}

void set_distance_to_wall(float new_distance_to_wall)
{
    g_distance_to_wall = new_distance_to_wall;
    calculate_grid_points();
}

void set_distance_up(float new_distance_up)
{
    g_distance_up = new_distance_up;
}
