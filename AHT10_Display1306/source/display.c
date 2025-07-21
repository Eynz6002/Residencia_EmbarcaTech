#include "libs/display.h"
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "libs/ssd1306.h"
#include "hardware/i2c.h"
#include <stdint.h>

struct render_area frame_area = {
    start_column: 0,
    end_column: ssd1306_width - 1,
    start_page: 0,
    end_page: ssd1306_n_pages - 1
};

char* display_string[] = 
{
    "",// x = 0, y = 0
    "",// x = 1, y = 8
    "",// x = 2, y = 16
    "",// x = 3, y = 24
    "",// x = 4, y = 32
    "",// x = 5, y = 40
    "",// x = 6, y = 48
    ""// x = 7, y = 56
};
uint8_t display_buffer[ssd1306_buffer_length];

void init_display()
{
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(display_SDA, GPIO_FUNC_I2C);
    gpio_set_function(display_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(display_SDA);
    gpio_pull_up(display_SCL);
    ssd1306_init();
    calculate_render_area_buffer_length(&frame_area);
    memset(display_buffer, 0, ssd1306_buffer_length);
    render_on_display(display_buffer, &frame_area);
}
void draw_display(char* msg, int indx)
{
    display_string[indx] = msg;
    ssd1306_draw_string(display_buffer, 5, 8*indx, display_string[indx]);
}
void show_on_display()
{
    render_on_display(display_buffer, &frame_area);
    memset(display_buffer, 0, ssd1306_buffer_length);
}