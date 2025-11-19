#include <laser/debug.h>
#include <laser/display.h>

#include <assert.h>

#include <laser/gpio.h>

#ifndef LASER_DEVICE

#define LCD_SET_D4(d) do { } while (0)
#define LCD_SET_D5(d) do { } while (0)
#define LCD_SET_D6(d) do { } while (0)
#define LCD_SET_D7(d) do { } while (0)

#define LCD_SET_RS(val)     do { } while (0)
#define LCD_SET_ENABLE(val) do { } while (0)

#else

#define LCD_SET_D4(val)     assert(gpioWrite(LCD_D4, val & 1) == 0)
#define LCD_SET_D5(val)     assert(gpioWrite(LCD_D5, val & 1) == 0)
#define LCD_SET_D6(val)     assert(gpioWrite(LCD_D6, val & 1) == 0)
#define LCD_SET_D7(val)     assert(gpioWrite(LCD_D7, val & 1) == 0)

#define LCD_SET_RS(val)     assert(gpioWrite(LCD_REGISTER_SELECT, val & 1) == 0)
#define LCD_SET_ENABLE(val) assert(gpioWrite(LCD_ENABLE, val & 1) == 0)

#endif

static void display_pulse()
{
    LCD_SET_ENABLE(1);
    DELAY(5);
    LCD_SET_ENABLE(0);
    DELAY(100);
}

static void display_write_nibble(char data)
{
    LCD_SET_D4((data >> 0) & 1);
    LCD_SET_D5((data >> 1) & 1);
    LCD_SET_D6((data >> 2) & 1);
    LCD_SET_D7((data >> 3) & 1);

    display_pulse();

    LCD_SET_D4(0);
    LCD_SET_D5(0);
    LCD_SET_D6(0);
    LCD_SET_D7(0);
}

static void display_write_byte(char data, char rs)
{
    LCD_SET_RS(rs & 1);

    display_write_nibble(data >> 4);
    display_write_nibble(data & 0b1111);

    LCD_SET_RS(0);
}

static void display_cmd(char cmd)  { display_write_byte(cmd, 0); }
static void display_data(char c)   { display_write_byte(c, 1);  }

static void display_on_off(char display_on, char cursor_on, char cursor_position_on)
{
    display_cmd((char)(0b00001000 | ((display_on & 1) << 2)
                                  | ((cursor_on & 1) << 1)
                                  | (cursor_position_on & 1)));
    DELAY(50);
}

static void display_clear()
{
    display_write_nibble(0b0000);
    display_write_nibble(0b0001);
    DELAY(2000);
}

void display_init()
{
    LCD_SET_RS(0);
    LCD_SET_ENABLE(0);

    display_write_nibble(0b0011);
    DELAY(5000); // function set
    
    display_write_nibble(0b0011);
    DELAY(200);
    display_write_nibble(0b0011);
    DELAY(50);

    // function set
    display_write_nibble(0b0010);
    DELAY(50);

    // we are now in 4 bit mode!

    // function set
    display_write_nibble(0b0010);
    display_write_nibble(0b1000);
    DELAY(50);

    display_on_off(0, 0, 0);
    display_clear();

    // entry mode set
    display_write_nibble(0b0000);
    display_write_nibble(0b0110);
    DELAY(50);

    display_on_off(1, 0, 0);
}

void display_write_string (const char* str, size_t len)
{
    display_clear();

    for(size_t i = 0; i < len; i++) {
        display_data(str[i]);
        DELAY(50);
    }
}
