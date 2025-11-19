#include <laser/debug.h>
#include <laser/display.h>

#include <laser/gpio.h>

#ifndef LASER_DEVICE

#define LCD_SET_D4(d) do { } while (0)
#define LCD_SET_D5(d) do { } while (0)
#define LCD_SET_D6(d) do { } while (0)
#define LCD_SET_D7(d) do { } while (0)

#define LCD_SET_RS(val)     do { } while (0)
#define LCD_SET_ENABLE(val) do { } while (0)

#else

#define LCD_SET_D4(val)     gpioWrite(LCD_D4, val)
#define LCD_SET_D5(val)     gpioWrite(LCD_D5, val)
#define LCD_SET_D6(val)     gpioWrite(LCD_D6, val)
#define LCD_SET_D7(val)     gpioWrite(LCD_D7, val)

#define LCD_SET_RS(val)     gpioWrite(LCD_REGISTER_SELECT, val)
#define LCD_SET_ENABLE(val) gpioWrite(LCD_ENABLE, val)

#endif

static void display_pulse()
{
    LCD_SET_ENABLE(1);
    DELAY(1);
    LCD_SET_ENABLE(0);
    DELAY(50);
}

static void display_write_nibble(char data)
{
    LCD_SET_D4((data >> 0) & 1);
    LCD_SET_D5((data >> 1) & 1);
    LCD_SET_D6((data >> 2) & 1);
    LCD_SET_D7((data >> 3) & 1);

    display_pulse();
}

static void display_write_byte(char data, char rs)
{
    LCD_SET_RS(rs & 1);
    display_write_nibble(data >> 4);
    DELAY(1);
    display_write_nibble(data & 0b1111);
    DELAY(50);
}

static void display_cmd(char cmd)  { display_write_byte(cmd, 0); }
static void display_data(char c)   { display_write_byte(c, 1);  }

void display_init()
{
    DELAY(50);
    LCD_SET_RS(0);
    LCD_SET_ENABLE(0);
    display_write_nibble(0x30); DELAY(5000);
    display_write_nibble(0x30); DELAY(200);
    display_write_nibble(0x30); DELAY(200);
    display_write_nibble(0x20); DELAY(200);
    display_cmd(0x28);
    display_cmd(0x08);
    display_cmd(0x01);
    DELAY(2000);
    display_cmd(0x06);
    display_cmd(0x0C);
}

void display_write_string (const char* str, size_t len)
{
    for(size_t i = 0; i < len; i++) {
        display_data(str[i]);
        DELAY(50);
    }
}
