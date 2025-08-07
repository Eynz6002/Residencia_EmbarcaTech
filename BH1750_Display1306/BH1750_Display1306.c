#include "pico/stdlib.h"    // Funções padrão do Raspberry Pi Pico.
#include "hardware/i2c.h"   // Biblioteca para comunicação I2C.
#include <stdio.h>          // Funções de entrada/saída (printf).
#include "libs/display.h"
#include "libs/bh1750.h"

int main() { // Função principal do programa.
    stdio_init_all(); // Inicializa console serial.
    config_bh1750(0, 1, i2c0);
    init_display();
    bh1750_start_monitoring(); // Inicia monitoramento de luminosidade.

    char lux_level[15];
    while (true) { // Loop infinito de operação.
        uint16_t lux_lecture = bh1750_read();
        sprintf(lux_level, "LUX: %u", lux_lecture); // Imprime luminosidade no console.
        draw_display((char*)lux_level, 2);
        show_on_display();
        sleep_ms(1000); // Pausa por 1 segundo.
    }
}