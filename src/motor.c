#include <laser/debug.h>
#include <laser/motor.h>

#include <assert.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <laser/config.h>
#include <laser/math.h>

#ifdef LASER_DEVICE
#include <pigpio.h>
#endif

volatile char should_quit = 0;
volatile char motor_should_run = 0;

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
    #define DIRECTION_PITCH(d) do {} while (0)

    #define PULSE_ON(gpio) do {} while (0)
    #define PULSE_OFF(gpio) do {} while (0)

    #define SLEEP(us) do {                                              \
        struct timespec t = { .tv_sec = 0, .tv_nsec = us * 1000 };      \
        clock_nanosleep(CLOCK_MONOTONIC, 0, &t, NULL);                  \
    } while(0)
#else
    #define SLEEP(us) gpioDelay(us)

    #define DIRECTION_YAW(d) gpioWrite(YAW_DIRECTION_GPIO, d == DIRECTION_FORWARD ? 1 : 0)
    #define DIRECTION_PITCH(d) gpioWrite(PITCH_DIRECTION_GPIO, d == DIRECTION_FORWARD ? 1 : 0)

    #define PULSE_ON(gpio) gpioWrite(gpio, 1);
    #define PULSE_OFF(gpio) gpioWrite(gpio, 0);
#endif

void motor_init()
{
#ifdef LASER_DEVICE
    int rc = gpioInitialise();
    fprintf(stderr, "rc = %d\n", rc);
    assert(rc >= 0);

    gpioSetMode(YAW_DIRECTION_GPIO, PI_OUTPUT);
    gpioSetMode(YAW_PULSE_GPIO, PI_OUTPUT);
    gpioSetMode(PITCH_DIRECTION_GPIO, PI_OUTPUT);
    gpioSetMode(PITCH_PULSE_GPIO, PI_OUTPUT);

    gpioWrite(PITCH_PULSE_GPIO, 1);
    gpioWrite(YAW_PULSE_GPIO, 1);
#endif
}

typedef long long ll;
static size_t max(size_t a, size_t b) { return a > b ? a : b; }
static long maxl(long a, long b) { return a > b ? a : b; }

static void run_program_in_thread()
{
    const config_t* cfg = config();
    const ll rpm = (ll)cfg->motor_speed;
    const ll steps = (ll)max(cfg->steps_per_revolution_yaw, cfg->steps_per_revolution_pitch);

    const ll ideal_sleep_time = (ll)(60 * MICROSECONDS_PER_SECOND) / (rpm * steps) + 1;
    const ll new_sleep_time = maxl(ideal_sleep_time - BETWEEN_PULSE_SLEEP_TIME_US, BETWEEN_PULSE_SLEEP_TIME_US);
    DEBUG("sleep_time: %llu microseconds | new_sleep_time: %llu microseconds\n", ideal_sleep_time, new_sleep_time);

    direction_enum_t previous_direction_yaw = DIRECTION_FORWARD;
    direction_enum_t previous_direction_pitch = DIRECTION_FORWARD;
    for(size_t i = 0; i < current_picture->num_points; i++) {
        const motor_instruction_pair_t* instr = &current_picture->instructions[i];
        if (i == 0 || previous_direction_pitch != instr->pitch.direction
                   || previous_direction_yaw != instr->yaw.direction) {
            previous_direction_yaw = instr->yaw.direction;
            previous_direction_pitch = instr->pitch.direction;

            DIRECTION_YAW(instr->yaw.direction);
            DIRECTION_PITCH(instr->pitch.direction);
            SLEEP(DIRECTION_SLEEP_TIME);
        }
        const size_t instruction_steps = max(instr->yaw.steps, instr->pitch.steps);
        for(unsigned step = 0; step < instruction_steps; step++) {
            const char should_pulse_yaw = step < instr->yaw.steps;
            const char should_pulse_pitch = step < instr->pitch.steps;
            if (should_pulse_yaw)
                PULSE_OFF(YAW_PULSE_GPIO);
            if (should_pulse_pitch)
                PULSE_OFF(PITCH_PULSE_GPIO);

            SLEEP(BETWEEN_PULSE_SLEEP_TIME_US);

            if (should_pulse_yaw)
                PULSE_ON(YAW_PULSE_GPIO);
            if (should_pulse_pitch)
                PULSE_ON(PITCH_PULSE_GPIO);

            SLEEP(new_sleep_time);
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
    should_quit = 1;

    if (motor_should_run) {
        stop_motor();
    } else {
        SIGNAL();
    }

    pthread_join(g_current_motor_thread, NULL);
    fputs("motor thread exited\n", stderr);
    g_current_motor_thread = 0;
}

void start_motor()
{
    assert(g_current_motor_thread != 0);
    assert(!motor_should_run);
    assert(current_picture);
    motor_should_run = 1;
    SIGNAL();
}

void stop_motor()
{
    assert(motor_should_run);
    pthread_mutex_lock(&mutex);
    motor_should_run = 0;
    fputs("waiting for motor to shut down...\n", stderr);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
    fputs("motor shut down\n", stderr);
}

void motor_free()
{
#ifdef LASER_DEVICE
    gpioTerminate();
#endif
}

char motor_is_running()
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
