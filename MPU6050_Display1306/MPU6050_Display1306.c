#include <stdio.h>          // Funções de entrada/saída (printf).
#include <math.h>           // Funções matemáticas (sqrt, atan2, M_PI).
#include "pico/stdlib.h"    // Funções padrão do Raspberry Pi Pico.
#include "hardware/i2c.h"   // Biblioteca para comunicação I2C.
#include "libs/display.h"

// Configurações do MPU6050
#define MPU6050_ADDR 0x68   // Endereço I2C do MPU6050.
#define I2C_PORT i2c0       // Define a porta I2C (i2c0).
#define SDA_MPU6050 0       // Pino GPIO para SDA do MPU6050.
#define SCL_MPU6050 1       // Pino GPIO para SCL do MPU6050.

// Constante de conversão para 'g'
#define conv_g 16384.0      // Conversão de valor bruto para 'g' (para ±2g).

void read_mpu6050(int16_t *x, int16_t *y, int16_t *z) // Lê dados do acelerômetro MPU6050.
{
    uint8_t buffer[6]; // Buffer para armazenar os 6 bytes de dados.
    // Lê registrador do acelerômetro (0x3B).
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, (uint8_t[]){0x3B}, 1, true);
    i2c_read_blocking(I2C_PORT, MPU6050_ADDR, buffer, 6, false); // Lê 6 bytes.

    *x = (buffer[0] << 8) | buffer[1]; // Combina bytes para X.
    *y = (buffer[2] << 8) | buffer[3]; // Combina bytes para Y.
    *z = (buffer[4] << 8) | buffer[5]; // Combina bytes para Z.
}

void init_pins() // Inicializa pinos I2C, MPU6050, display e servo.
{
    i2c_init(I2C_PORT, 100000); // Inicializa I2C com 100 kHz.
    gpio_set_function(SDA_MPU6050, GPIO_FUNC_I2C); // Configura SDA para I2C.
    gpio_set_function(SCL_MPU6050, GPIO_FUNC_I2C); // Configura SCL para I2C.
    // Inicialização do MPU6050 (sair do modo sleep).
    uint8_t reset[2] = {0x6B, 0x00}; // Registrador Power Management 1.
    i2c_write_blocking(I2C_PORT, MPU6050_ADDR, reset, 2, false); // Envia comando de reset.
    init_display();
}

int main() { // Função principal do programa.
    stdio_init_all(); // Inicializa console serial.
    init_pins(); // Inicializa pinos e periféricos.

    int16_t x, y, z; // Variáveis para valores brutos do acelerômetro.
    float xg, yg, zg, incline; // Variáveis para valores em 'g' e inclinação.

    char texto_x[6]; // Buffer para string de coordenadas X.
    char texto_y[6]; // Buffer para string de coordenadas Y.
    char texto_z[6]; // Buffer para string de coordenadas Z.
    char texto_incline[19]; // Buffer para string da inclinação.

    while (true) { // Loop infinito de operação.
        read_mpu6050(&x, &y, &z); // Lê dados do MPU6050.

        xg = x / conv_g; // Converte X para 'g'.
        yg = y / conv_g; // Converte Y para 'g'.
        zg = z / conv_g; // Converte Z para 'g'.

        // Calcula inclinação usando atan2, eixo X versus plano YZ.
        incline = atan2(xg, sqrt(yg * yg + zg * zg)) * (180.0 / M_PI);

        // Formata strings para exibição.
        sprintf(texto_x, "X:%d", x);
        sprintf(texto_y, "Y:%d", y);
        sprintf(texto_z, "Z:%d", z);
        sprintf(texto_incline, "Incline: %.2f", incline);

        draw_display((char*)texto_x, 1);
        draw_display((char*)texto_y, 3);
        draw_display((char*)texto_z, 5);
        draw_display((char*)texto_incline, 7);
        
        show_on_display();
        sleep_ms(1000); // Pausa por 1 segundo.
    }
}