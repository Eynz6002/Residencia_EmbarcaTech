#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

#define display_SDA 14
#define display_SCL 15

void init_display();
void show_on_display();
void draw_display(char* msg, int indx);
#endif