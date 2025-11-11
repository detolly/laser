#pragma once

extern float g_picture_size;
extern float g_distance_to_wall;
extern float g_distance_up;

const float* picture_size(void);
const float* distance_to_wall(void);
const float* distance_up(void);

void set_picture_size(float new_picture_size);
void set_distance_to_wall(float new_distance_to_wall);
void set_distance_up(float new_distance_up);
