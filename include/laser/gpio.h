#pragma once

#define YAW_DIRECTION_GPIO 13
#define YAW_PULSE_GPIO 6
#define PITCH_DIRECTION_GPIO 26
#define PITCH_PULSE_GPIO 19

#define LCD_REGISTER_SELECT 4
#define LCD_ENABLE 17
#define LCD_D4 27
#define LCD_D5 22
#define LCD_D6 10
#define LCD_D7 9

#ifndef LASER_DEVICE

#include <time.h>

#define DELAY(us) do {                                              \
    struct timespec t = { .tv_sec = 0, .tv_nsec = us * 1000 };      \
    clock_nanosleep(CLOCK_MONOTONIC, 0, &t, NULL);                  \
} while(0)

#else

#include <pigpio.h>
#define DELAY(us) gpioDelay(us)

#endif

void gpio_init();
void gpio_terminate();

