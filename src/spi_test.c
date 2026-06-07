
#include <assert.h>
#include <signal.h>
#include <stdio.h>

#include <laser/gpio.h>

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


unsigned int spi_handle = 0;
unsigned int spi_handle2 = 0;

void terminate_program()
{
    fprintf(stderr, "\nTerminating GPIO.\n");
    close_all();
    spiClose(spi_handle);
    spiClose(spi_handle2);
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

    int ret = spiOpen(0, 1024*1024, 0);
    if (ret < 0)
        return 1;

    int ret2 = spiOpen(1, 1024*1024, 0);
    if (ret2 < 0)
        return 1;

    spi_handle = (unsigned int)ret;
    spi_handle2 = (unsigned int)ret2;

    for(;;) {
        uint8_t buf[2] = {0};
        if (spiRead(spi_handle, (char*)buf, 2) != 2)
            assert(0);

        // uint8_t buf2[2] = {0};
        // if (spiRead(spi_handle2, (char*)buf2, 2) != 2)
        //     assert(0);

        uint16_t raw = ((buf[0] << 8) | buf[1]) & 0x1FFF;
        int16_t val = (int16_t)(raw << 3) >> 3;

        // uint16_t raw2 = ((buf2[0] << 8) | buf2[1]) & 0x1FFF;
        // int16_t val2 = (int16_t)(raw2 << 3) >> 3;

        // fprintf(stderr, "\r%d      %d        ", val, val2);
        fprintf(stderr, "\r%d          ", val);

        const size_t on_time = 100;
        const float duty = 0.30f;
        const size_t off_time = (size_t)((float)on_time / duty) - on_time;
        const int16_t target = 0;

        DELAY(100000);

        // if (val > target) {
        //     open_left();
        //     DELAY(on_time);
        //     close_left();
        //     DELAY(off_time);
        // } else if (val < target) {
        //     open_right();
        //     DELAY(on_time);
        //     close_right();
        //     DELAY(off_time);
        // } else break;

        // DELAY(DEAD_TIME);
    }

    terminate_program();
    gpioTerminate();

    return 0;
}

#endif

