
#include <laser/gpio.h>
#include <laser/display.h>

static const char string[] = "greetings";

int main(int argc, const char* argv[])
{
    (void)argc;
    (void)argv;

    gpio_init();

    display_init();
    display_write_string(string, sizeof(string)-1);

    gpio_terminate();
}

