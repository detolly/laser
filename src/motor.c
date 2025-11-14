#define _POSIX_C_SOURCE 200809L

#include <motor.h>

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sched.h>

#include <config.h>
#include <laser_math.h>

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

#ifndef LASER_DEVICE

#define DIRECTION_YAW(d) do {} while (0)
#define PULSE_YAW() do {} while (0)

#define DIRECTION_PITCH(d) do {} while (0)
#define PULSE_PITCH() do {} while (0)

#define SLEEP(us) do {                                              \
    struct timespec t = { .tv_sec = 0, .tv_nsec = us * 1000 };      \
    clock_nanosleep(CLOCK_MONOTONIC, 0, &t, NULL);                  \
} while(0)

#else

#define YAW_DIRECTION_GPIO 13
#define YAW_PULSE_GPIO 6

#define PITCH_DIRECTION_GPIO 26
#define PITCH_PULSE_GPIO 19

#define DIRECTION_YAW(d) gpioWrite(YAW_DIRECTION_GPIO, d == DIRECTION_FORWARD ? 1 : 0)
#define PULSE_YAW() gpioWrite(YAW_PULSE_GPIO, 1); gpioWrite(YAW_PULSE_GPIO, 0)

#define DIRECTION_PITCH(d) gpioWrite(PITCH_DIRECTION_GPIO, d == DIRECTION_FORWARD ? 1 : 0);
#define PULSE_YAW() gpioWrite(PITCH_PULSE_GPIO, 1); gpioWrite(PITCH_PULSE_GPIO, 0)

#define SLEEP(us) gpioSleep(PI_TIME_RELATIVE, 0, us);

#endif

void motor_init()
{
#ifdef LASER_DEVICE
    int rc = gpioInitialise();
    assert(rc != 0);

    gpioSetMode(YAW_DIRECTION_GPIO, PI_OUTPUT);
    gpioSetMode(YAW_PULSE_GPIO, PI_OUTPUT);
    gpioSetMode(PITCH_DIRECTION_GPIO, PI_OUTPUT);
    gpioSetMode(PITCH_PULSE_GPIO, PI_OUTPUT);
#endif
}

static size_t max(size_t a, size_t b)
{
    return a > b ? a : b;
}

#define MICROSECONDS_IN_SECONDS 1'000'000

static void run_program_in_thread()
{
    typedef long long ll;
    const config_t* cfg = config();

    const ll rpm = (ll)cfg->motor_speed;
    const ll steps = (ll)max(cfg->steps_per_revolution_yaw, cfg->steps_per_revolution_yaw);
    const ll sleep_time = (ll)MICROSECONDS_IN_SECONDS / ((rpm * steps) / 60) + 1;

#ifdef LASER_DEBUG
    // fprintf(stderr, "sleep_time: %llu\n", sleep_time);
#endif

    for(size_t i = 0; i < current_picture->num_points; i++) {
        const motor_instruction_pair_t* instr = &current_picture->instructions[i];
        DIRECTION_YAW(instr->yaw.direction);
        DIRECTION_PITCH(instr->pitch.direction);
        for(unsigned i = 0; i < max(instr->yaw.steps, instr->pitch.steps); i++) {
            if (i < instr->yaw.steps)
                PULSE_YAW();
            if (i < instr->pitch.steps)
                PULSE_PITCH();
            SLEEP(sleep_time);
        }
    }
}

static const struct sched_param prio = {
    .sched_priority = 99
};

void* motor_thread(void*)
{
    pthread_setschedparam(g_current_motor_thread, SCHED_FIFO, &prio);

    while(!should_quit) {
        fputs("motor_thread: starting motor\n", stderr);
        while(motor_should_run)
            run_program_in_thread();

        fputs("motor_thread: motor stopped... signaling that we have stopped\n", stderr);
        SIGNAL();

        if (should_quit)
            break;

        fputs("motor_thread: waiting for wake-up\n", stderr);
        WAIT();
        fputs("motor_thread: woken up\n", stderr);
    }

    fputs("motor_thread: quitting thread.\n", stderr);
    return NULL;
}

void start_motor_thread()
{
    assert(g_current_motor_thread == 0);
    pthread_create(&g_current_motor_thread, NULL, &motor_thread, NULL);
}

void stop_motor_thread()
{
    assert(g_current_motor_thread != 0);

    pthread_mutex_lock(&mutex);
    should_quit = true;
    motor_should_run = false;
    fputs("waiting for motor to shut down...\n", stderr);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
    fputs("motor shut down\n", stderr);

    pthread_join(g_current_motor_thread, NULL);
    fputs("motor thread exited\n", stderr);
    g_current_motor_thread = 0;
}

void start_motor()
{
    assert(g_current_motor_thread != 0);
    assert(!motor_should_run);
    assert(current_picture);
    motor_should_run = true;
    SIGNAL();
}

void stop_motor()
{
    assert(motor_should_run);
    pthread_mutex_lock(&mutex);
    motor_should_run = false;
    fputs("waiting for motor to shut down...\n", stderr);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
    fputs("motor shut down\n", stderr);
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
