#pragma once

#include <stddef.h>
#include <pthread.h>

#include <laser_math.h>
#include <picture.h>

void motor_init();

void start_motor();
void stop_motor();

char motor_is_running();

void start_motor_thread();
void stop_motor_thread();

void set_picture(const picture_t* picture);

pthread_t current_motor_thread();
