#pragma once

#define RAD_TO_DEG (180.f / 3.1415926535f)
#define TWO_PI (2.f * 3.1415926535f)

typedef struct {
    float x;
    float y;
} point_t;

typedef struct {
    float yaw;   // x
    float pitch; // y
} angles_t;

typedef struct {
    point_t original_point;
    point_t projected_point;
    point_t fixed_point;
    angles_t fixed_angles;
} projection_t;

float pitch(const point_t* p);
float yaw(const point_t* p);

void project_point(projection_t* point,
                   const point_t* point_to_project);

void calculate_grid_points(void);
void free_grid_points(void);
