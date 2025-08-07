// Bibliotecas inclusas
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "libs/vl53l0_1xv2.h"

// Definições de I2C
#define I2C_PORT i2c0
#define SDA_PIN 0
#define SCL_PIN 1

int main() {
    stdio_init_all();   // Inicialização geral
    
    sleep_ms(3000);
    printf("VL53L0X - Leitura de Distância (Laser)\n");

    config_vl53l0_1xv2(0, 1, i2c0);
    sleep_ms(3000);

    if (!init_vl53l0_1xv2()) {
        printf("Falha critica na inicializacao do sensor. Programa abortado.\n");
        while(1) { sleep_ms(1000); } // Trava aqui se a inicialização falhar
    }

    printf("Sensor iniciado com sucesso.\n");

    while (1) {         // Imprimir distância lida
        int distancia = read_distance();
        if (distancia < 0) {
            printf("Erro na leitura da distância.\n");
        } else {
            printf("Distância: %d mm (%.2f m)\n", distancia, distancia / 1000.0f);
        }
        sleep_ms(500);
    }

    return 0;
}
