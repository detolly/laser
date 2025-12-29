
#include <assert.h>

#include <laser/gpio.h>
#include <laser/picture.h>

#ifndef LASER_DEVICE
int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;
    return 0;
}
#else

#define PULSE_GPIO 26
#define PULSE_GPIO2 19

int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    gpioInitialise();
    gpioSetMode(PULSE_GPIO,  PI_OUTPUT);
    gpioSetMode(PULSE_GPIO2, PI_OUTPUT);

    const size_t on_time = 10;
    const float duty = 0.30f;
    const size_t off_time = (size_t)((float)on_time / duty) - on_time;

#define NUM_PULSES 20

    for(int i = 0; i < NUM_PULSES; i++) {
        gpioWrite(PULSE_GPIO2, 1);
        DELAY(on_time);
        gpioWrite(PULSE_GPIO2, 0);
        DELAY(off_time);
    }

    gpioWrite(PULSE_GPIO, 0);
    gpioWrite(PULSE_GPIO2, 0);

    gpioTerminate();
    return 0;
}

#endif

