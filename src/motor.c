#include "laser_math.h"
#include <motor.h>

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

volatile bool should_quit = false;
volatile bool motor_should_run = false;

pthread_t g_current_motor_thread = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

#define WAIT() do {                                 \
    pthread_mutex_lock(&mutex);                     \
    pthread_cond_wait(&cond, &mutex);               \
    pthread_mutex_unlock(&mutex);                   \
} while(0)

#define SIGNAL() do {                               \
    pthread_mutex_lock(&mutex);                     \
    pthread_cond_signal(&cond);                     \
    pthread_mutex_unlock(&mutex);                   \
} while(0)

const picture_t* current_picture = NULL;

#define DIRECTION_YAW(d)
#define PULSE_YAW()

#define DIRECTION_PITCH(d)
#define PULSE_PITCH()

static void run_program_in_thread()
{
    for(size_t i = 0; i < current_picture->num_points; i++) {
        const motor_instruction_pair_t* instr = &current_picture->instructions[i];
        DIRECTION_YAW(instr->yaw.direction);
        DIRECTION_PITCH(instr->pitch.direction);
        for(unsigned x = 0; x < instr->yaw.steps; x++)
            PULSE_YAW();
        for(unsigned x = 0; x < instr->pitch.steps; x++)
            PULSE_PITCH();
    }
}

void* motor_thread(void*)
{
    while(!should_quit) {
        while(motor_should_run)
            run_program_in_thread();

        if (should_quit)
            break;

        fputs("motor_thread: motor stopped... waiting for signal", stderr);
        WAIT();
        fputs("motor_thread: woken up... starting motor", stderr);
    }
    return NULL;
}

void start_motor_thread()
{
    assert(g_current_motor_thread == 0);
    pthread_create(&g_current_motor_thread, NULL, &motor_thread, NULL);
}

void stop_motor_thread()
{
    should_quit = true;
    motor_should_run = false;
    pthread_join(g_current_motor_thread, NULL);
    g_current_motor_thread = 0;
}

void start_motor()
{
    assert(!motor_should_run);
    assert(current_picture);
    motor_should_run = true;
    SIGNAL();
}

void stop_motor()
{
    assert(motor_should_run);
    motor_should_run = false;
}

bool motor_is_running()
{
    return motor_should_run;
}

void set_picture(const picture_t* picture)
{
    assert(!motor_should_run);
    current_picture = picture;
}

pthread_t current_motor_thread()
{
    return g_current_motor_thread;
}
