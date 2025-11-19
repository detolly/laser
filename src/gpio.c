#include <laser/debug.h>
#include <laser/gpio.h>

#include <assert.h>

void gpio_init()
{
#ifdef LASER_DEVICE
    int rc = gpioInitialise();
    DEBUG("rc = %d\n", rc);
    assert(rc >= 0);

    gpioSetMode(YAW_DIRECTION_GPIO, PI_OUTPUT);
    gpioSetMode(YAW_PULSE_GPIO, PI_OUTPUT);
    gpioSetMode(PITCH_DIRECTION_GPIO, PI_OUTPUT);
    gpioSetMode(PITCH_PULSE_GPIO, PI_OUTPUT);

    gpioWrite(PITCH_PULSE_GPIO, 1);
    gpioWrite(YAW_PULSE_GPIO, 1);

    gpioSetMode(LCD_REGISTER_SELECT, PI_OUTPUT);
    gpioSetMode(LCD_ENABLE, PI_OUTPUT);
    gpioSetMode(LCD_D4, PI_OUTPUT);
    gpioSetMode(LCD_D5, PI_OUTPUT);
    gpioSetMode(LCD_D6, PI_OUTPUT);
    gpioSetMode(LCD_D7, PI_OUTPUT);
#endif
}

void gpio_terminate()
{
#ifdef LASER_DEVICE
    gpioTerminate();
#endif
}
