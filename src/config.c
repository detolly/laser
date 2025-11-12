
#include <config.h>
#include <laser_math.h>

int g_steps_per_revolution_yaw = 15000;
int g_steps_per_revolution_pitch = 15000;

char g_must_recalculate_pictures = 1;

float g_picture_size = 0.5f;
float g_distance_to_wall = 10.f;
float g_distance_up = 1.f;
float g_motor_speed = 750.f;

int steps_per_revolution_yaw(void) { return g_steps_per_revolution_yaw; }
int steps_per_revolution_pitch(void) { return g_steps_per_revolution_pitch; }

char must_recalculate_pictures(void) { return g_must_recalculate_pictures; }
float picture_size(void) { return g_picture_size; }
float distance_to_wall(void) { return g_distance_to_wall; }
float distance_up(void) { return g_distance_up; }
float motor_speed(void) { return g_motor_speed; }

void set_must_recalculate_pictures(char new)
{
    g_must_recalculate_pictures = new;
}

void set_steps_per_revolution_yaw(int steps)
{
    g_steps_per_revolution_yaw = steps;
    set_must_recalculate_pictures(1);
}

void set_steps_per_revolution_pitch(int steps)
{
    g_steps_per_revolution_pitch = steps;
    set_must_recalculate_pictures(1);
}

void set_motor_speed(float new_speed)
{
    g_motor_speed = new_speed;
}

void set_picture_size(float new_picture_size)
{
    g_picture_size = new_picture_size;
    set_must_recalculate_pictures(1);
}

void set_distance_to_wall(float new_distance_to_wall)
{
    g_distance_to_wall = new_distance_to_wall;
    set_must_recalculate_pictures(1);
}

void set_distance_up(float new_distance_up)
{
    g_distance_up = new_distance_up;
    set_must_recalculate_pictures(1);
}
