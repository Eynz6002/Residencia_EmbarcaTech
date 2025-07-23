#include "pico/stdlib.h"    // Funções padrão do Raspberry Pi Pico.
#include "hardware/i2c.h"   // Biblioteca para comunicação I2C.
#include <stdio.h>          // Funções de entrada/saída (printf).
#include "libs/display.h"

// Configuração de pinos
// ======== BH1750 ========
#define I2C_PORT i2c0       // Define a porta I2C (i2c0).
#define BH1750_SDA 0        // Pino GPIO para SDA do BH1750.
#define BH1750_SCL 1        // Pino GPIO para SCL do BH1750.

// Configurações do Sensor BH1750
#define BH1750_ADDR 0x23    // Endereço I2C do sensor de luz BH1750.
#define BH1750_CMD_START 0x10 // Comando para iniciar medição contínua alta resolução.


uint16_t bh1750_read() // Lê luminosidade do sensor BH1750.
{
    uint8_t data[2]; // Buffer para dados lidos do sensor.
    i2c_read_blocking(I2C_PORT, BH1750_ADDR, data, 2, false); // Lê 2 bytes do sensor.
    return (data[0] << 8 | data[1]); // Combina bytes para valor 16 bits.
}

void bh1750_start_monitoring() // Envia comando para BH1750 iniciar medição.
{
    uint8_t cmd = BH1750_CMD_START; // Comando de início de medição.
    i2c_write_blocking(I2C_PORT, BH1750_ADDR, &cmd, 1, false); // Envia comando ao sensor.
}

void init_pins() // Inicializa pinos I2C e servo motor.
{
    i2c_init(I2C_PORT, 100 * 1000); // Inicializa I2C com 100 kHz.
    gpio_set_function(BH1750_SDA, GPIO_FUNC_I2C); // Configura SDA para I2C.
    gpio_set_function(BH1750_SCL, GPIO_FUNC_I2C); // Configura SCL para I2C.
    gpio_pull_up(BH1750_SDA); // Habilita pull-up no pino SDA.
    gpio_pull_up(BH1750_SCL); // Habilita pull-up no pino SCL.
    init_display();
}

int main() { // Função principal do programa.
    stdio_init_all(); // Inicializa console serial.
    init_pins(); // Inicializa pinos e periféricos.
    bh1750_start_monitoring(); // Inicia monitoramento de luminosidade.

    char lux_level[15];
    while (true) { // Loop infinito de operação.
        uint16_t lux = bh1750_read(); // Lê nível de luminosidade.
        lux = (lux) / 1.2;
        sprintf(lux_level, "LUX: %d", lux); // Imprime luminosidade no console.
        draw_display((char*)lux_level, 2);
        show_on_display();
        sleep_ms(1000); // Pausa por 1 segundo.
    }
}