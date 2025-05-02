#include "libs/adc_controller.h"
#include "hardware/adc.h"

int dead_zone = 512; // Definição e inicialização (opcional)
int x_min = 0;      // Definição e inicialização (opcional)
int x_max = 4095;   // Definição e inicialização (opcional)
int y_min = 0;
int y_max = 4095;
int center_x = 2048;
int center_y = 2048;
volatile int x_location;
volatile int y_location;

void init_joystick()
{
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);
}
void read_joystick()
{
    adc_select_input(X_joystick);
    x_location = adc_read();
    adc_select_input(Y_joystick);
    y_location = adc_read();
}
void init_temp_sensor()
{
    adc_init();
    adc_set_temp_sensor_enabled(true);
}
float temperature_check()
{
    adc_select_input(adc_temp_sensor);
    uint16_t raw_value = adc_read();
    const float conversion_factor = 3.3f / (1 << 12);
    return  27.0f - ((raw_value * conversion_factor) - 0.706f) / 0.001721f;
    
}