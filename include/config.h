#pragma once

extern int g_steps_per_revolution_yaw;
extern int g_steps_per_revolution_pitch;

int steps_per_revolution_yaw(void);
int steps_per_revolution_pitch(void);

void set_steps_per_revolution_yaw(int steps_per_revolution_yaw);
void set_steps_per_revolution_pitch(int steps_per_revolution_pitch);

extern char g_must_recalculate_pictures;
extern float g_picture_size;
extern float g_distance_to_wall;
extern float g_distance_up;
extern float g_motor_speed;

char must_recalculate_pictures(void);
float picture_size(void);
float distance_to_wall(void);
float distance_up(void);
float motor_speed(void);

void set_must_recalculate_pictures(char new);
void set_motor_speed(float new_speed);
void set_picture_size(float new_picture_size);
void set_distance_to_wall(float new_distance_to_wall);
void set_distance_up(float new_distance_up);
