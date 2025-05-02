#include "libs/buttons_leds.h"
#include "pico/stdlib.h"

void ctrl_green(int state)
{
    gpio_put(led_red, state);
}
void ctrl_blue(int state)
{
    gpio_put(led_red, state);
}
void ctrl_red(int state)
{
    gpio_put(led_red, state);
}
void init_leds()
{
    gpio_init(led_green);
    gpio_set_dir(led_green, GPIO_OUT);
    gpio_put(led_green, false);
    gpio_init(led_blue);
    gpio_set_dir(led_blue, GPIO_OUT);
    gpio_put(led_blue, false);
    gpio_init(led_red);
    gpio_set_dir(led_red, GPIO_OUT);
    gpio_put(led_red, false);
}
void init_buttons()
{
    gpio_init(button_A);
    gpio_set_dir(button_A, GPIO_IN);
    gpio_pull_up(button_A);
    gpio_init(button_B);
    gpio_set_dir(button_B, GPIO_IN);
    gpio_pull_up(button_B);
}