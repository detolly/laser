
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

#define MOTOR_GPIO_0 26
#define MOTOR_GPIO_1 19
#define MOTOR_GPIO_2 13
#define MOTOR_GPIO_3 6

#define LOW 0
#define HIGH 1

static int current_step = 0;
static direction_enum_t current_direction = DIRECTION_FORWARD;

static void switch_off()
{
    gpioWrite(MOTOR_GPIO_0, LOW);
    gpioWrite(MOTOR_GPIO_1, LOW);
    gpioWrite(MOTOR_GPIO_2, LOW);
    gpioWrite(MOTOR_GPIO_3, LOW);
}

static void step()
{
    switch(current_step){
    case 0:
        gpioWrite(MOTOR_GPIO_0, LOW);
        gpioWrite(MOTOR_GPIO_1, LOW);
        gpioWrite(MOTOR_GPIO_2, LOW);
        gpioWrite(MOTOR_GPIO_3, HIGH);
        break;
    case 1:
        gpioWrite(MOTOR_GPIO_0, LOW);
        gpioWrite(MOTOR_GPIO_1, LOW);
        gpioWrite(MOTOR_GPIO_2, HIGH);
        gpioWrite(MOTOR_GPIO_3, HIGH);
    break;
    case 2:
        gpioWrite(MOTOR_GPIO_0, LOW);
        gpioWrite(MOTOR_GPIO_1, LOW);
        gpioWrite(MOTOR_GPIO_2, HIGH);
        gpioWrite(MOTOR_GPIO_3, LOW);
    break;
    case 3:
        gpioWrite(MOTOR_GPIO_0, LOW);
        gpioWrite(MOTOR_GPIO_1, HIGH);
        gpioWrite(MOTOR_GPIO_2, HIGH);
        gpioWrite(MOTOR_GPIO_3, LOW);
    break;
    case 4:
        gpioWrite(MOTOR_GPIO_0, LOW);
        gpioWrite(MOTOR_GPIO_1, HIGH);
        gpioWrite(MOTOR_GPIO_2, LOW);
        gpioWrite(MOTOR_GPIO_3, LOW);
    break;
    case 5:
        gpioWrite(MOTOR_GPIO_0, HIGH);
        gpioWrite(MOTOR_GPIO_1, HIGH);
        gpioWrite(MOTOR_GPIO_2, LOW);
        gpioWrite(MOTOR_GPIO_3, LOW);
    break;
    case 6:
        gpioWrite(MOTOR_GPIO_0, HIGH);
        gpioWrite(MOTOR_GPIO_1, LOW);
        gpioWrite(MOTOR_GPIO_2, LOW);
        gpioWrite(MOTOR_GPIO_3, LOW);
    break;
    case 7:
        gpioWrite(MOTOR_GPIO_0, HIGH);
        gpioWrite(MOTOR_GPIO_1, LOW);
        gpioWrite(MOTOR_GPIO_2, LOW);
        gpioWrite(MOTOR_GPIO_3, HIGH);
    break;
    }

    if(current_direction == DIRECTION_FORWARD)
        current_step++;
    else
        current_step--;

    if(current_step > 7)
        current_step = 0;
    else if(current_step < 0)
        current_step = 7;
}

int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    gpioInitialise();

    gpioSetMode(MOTOR_GPIO_0, PI_OUTPUT);
    gpioSetMode(MOTOR_GPIO_1, PI_OUTPUT);
    gpioSetMode(MOTOR_GPIO_2, PI_OUTPUT);
    gpioSetMode(MOTOR_GPIO_3, PI_OUTPUT);

    for(int i = 0; i < 64; i++) {
        step();
        DELAY(1000000/64);
    }

    switch_off();

    return 0;
}

#endif

