#include <laser/debug.h>
#include <laser/gpio.h>

#include <assert.h>

void gpio_init()
{
#ifdef LASER_DEVICE
    int rc = gpioInitialise();
    DEBUG("rc = %d\n", rc);
    assert(rc >= 0);

    assert(gpioSetMode(YAW_DIRECTION_GPIO, PI_OUTPUT) == 0);
    assert(gpioSetMode(YAW_PULSE_GPIO, PI_OUTPUT) == 0);
    assert(gpioSetMode(PITCH_DIRECTION_GPIO, PI_OUTPUT) == 0);
    assert(gpioSetMode(PITCH_PULSE_GPIO, PI_OUTPUT) == 0);

    assert(gpioWrite(PITCH_PULSE_GPIO, 1) == 0);
    assert(gpioWrite(YAW_PULSE_GPIO, 1) == 0);

    assert(gpioSetMode(LCD_REGISTER_SELECT, PI_OUTPUT) == 0);
    assert(gpioSetMode(LCD_ENABLE, PI_OUTPUT) == 0);
    assert(gpioSetMode(LCD_D4, PI_OUTPUT) == 0);
    assert(gpioSetMode(LCD_D5, PI_OUTPUT) == 0);
    assert(gpioSetMode(LCD_D6, PI_OUTPUT) == 0);
    assert(gpioSetMode(LCD_D7, PI_OUTPUT) == 0);
#endif
}

void gpio_terminate()
{
#ifdef LASER_DEVICE
    gpioTerminate();
#endif
}
