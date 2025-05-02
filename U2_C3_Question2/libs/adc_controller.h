#ifndef ADC_CONTROLLER_H
#define ADC_CONTROLLER_H

#define adc_temp_sensor 4
#define X_joystick 1
#define Y_joystick 0

extern int x_min;
extern int x_max;
extern int y_min;
extern int y_max;
extern int dead_zone;
extern int center_x;
extern int center_y;
extern volatile int x_location;
extern volatile int y_location;

void read_joystick();
void init_joystick();
void init_temp_sensor();
float temperature_check();
#endif