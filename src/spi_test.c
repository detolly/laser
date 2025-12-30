
#include <assert.h>
#include <stdio.h>

#include <laser/gpio.h>
#include <string.h>

#ifndef LASER_DEVICE
int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;
    return 0;
}
#else

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    gpioInitialise();
    int ret = spiOpen(0, 1024*1024, 0);
    if (ret < 0)
        return 1;

    unsigned int spi_handle = (unsigned int)ret;
    for(int i = 0; i < 0xFFFFFF; i++) {

        uint8_t buf[2] = {0};
        if (spiRead(spi_handle, (char*)buf, 2) != 2)
            assert(0);

        // fprintf(stderr, BYTE_TO_BINARY_PATTERN " " BYTE_TO_BINARY_PATTERN "\n",
        //                 BYTE_TO_BINARY(buf[0]),
        //                 BYTE_TO_BINARY(buf[1]));

        uint16_t raw = ((buf[0] << 8) | buf[1]) & 0x1FFF;
        int16_t val = (int16_t)(raw << 3) >> 3;

        // fprintf(stderr, BYTE_TO_BINARY_PATTERN " " BYTE_TO_BINARY_PATTERN "\n",
        //                 BYTE_TO_BINARY(((char*)&val)[1]),
        //                 BYTE_TO_BINARY(((char*)&val)[0]));

        fprintf(stderr, "\r%d                  ", val);
        gpioDelay(1000);

    }

    spiClose(spi_handle);
    fprintf(stderr, "\n");

    gpioTerminate();
    return 0;
}

#endif

