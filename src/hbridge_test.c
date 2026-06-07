
#include <assert.h>
#include <signal.h>
#include <stdio.h>

#include <laser/gpio.h>
#include <laser/picture.h>
#include <stdlib.h>

#ifndef LASER_DEVICE
int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;
    return 0;
}
#else

#define HBRIDGE_P1 26
#define HBRIDGE_N1 19
#define HBRIDGE_P2 13
#define HBRIDGE_N2 6

#define DEAD_TIME 5

void close_left()
{
    gpioWrite(HBRIDGE_P1, 0);
    gpioWrite(HBRIDGE_N2, 1);
}

void close_right()
{
    gpioWrite(HBRIDGE_P2, 0);
    gpioWrite(HBRIDGE_N1, 1);
}

void close_all()
{
    gpioWrite(HBRIDGE_P1, 0);
    gpioWrite(HBRIDGE_P2, 0);
    gpioWrite(HBRIDGE_N1, 1);
    gpioWrite(HBRIDGE_N2, 1);

}

void open_left()
{
    gpioWrite(HBRIDGE_P1, 1);
    gpioWrite(HBRIDGE_N2, 0);
}

void open_right()
{
    gpioWrite(HBRIDGE_P2, 1);
    gpioWrite(HBRIDGE_N1, 0);
}

unsigned max(unsigned a, unsigned b)
{
    return a > b ? a : b;
}

void terminate_program()
{
    fprintf(stderr, "\nTerminating GPIO.\n");
    close_all();
}

void on_signal(int signal)
{
    fprintf(stderr, "\nReceived signal %d.\n", signal);
    terminate_program();
}

int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    signal(SIGQUIT, on_signal);
    signal(SIGINT, on_signal);
    signal(SIGABRT, on_signal);
    signal(SIGKILL, on_signal);
    signal(SIGSTOP, on_signal);

    gpioInitialise();
    gpioSetMode(HBRIDGE_P1,  PI_OUTPUT);
    gpioSetMode(HBRIDGE_P2,  PI_OUTPUT);
    gpioSetMode(HBRIDGE_N1,  PI_OUTPUT);
    gpioSetMode(HBRIDGE_N2,  PI_OUTPUT);

    close_all();

    const size_t on_time = 2000;
    const float duty = 0.6f;
    const size_t off_time = (size_t)((float)on_time / duty) - on_time;

    #define US_ON 1000

    size_t start = gpioTick();
    for(;;) {
        if (gpioTick() - start > US_ON)
            break;
        open_left();
        DELAY(on_time);
        close_left();
        DELAY(off_time);
    }

    DELAY(DEAD_TIME);

    DELAY(500000);

    start = gpioTick();
    for(;;) {
        if (gpioTick() - start > US_ON)
            break;
        open_right();
        DELAY(on_time);
        close_right();
        DELAY(off_time);
    }

    close_all();

    gpioTerminate();
    return 0;
}

#endif

