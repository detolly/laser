
#include <laser/gpio.h>
#include <laser/display.h>

int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    gpio_init();

    static const char string[] = "greetings from";
    static const char string2[] = "beyond grave";

    display_init();
    display_write_string(string, sizeof(string) - 1, string2, sizeof(string2) - 1);

    gpio_terminate();
}

