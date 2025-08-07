#include <stdio.h>          // Funções padrão de entrada/saída (printf).
#include "pico/stdlib.h"    // Funções padrão do Raspberry Pi Pico.
#include "libs/display.h"
#include "libs/aht10.h"
#include "hardware/i2c.h"   // Biblioteca para comunicação I2C.

// Configuração do sensor AHT10 (I2C)
#define SDA_AHT10 0         // Pino GPIO para SDA do AHT10.
#define SCL_AHT10 1        // Pino GPIO para SCL do AHT10.

int main() { // Função principal do programa.
    stdio_init_all(); // Inicializa console serial.
    init_display();
    aht10_init(SDA_AHT10, SCL_AHT10, i2c0);

    char texto_temperatura[15]; // Buffer para o texto de monitoramento.
    char texto_umidade[15];
    float aht10_lectures[2];

    while (true) { // Loop infinito para leituras e atualizações.
        aht10_trigger_monitoring(); // Aciona nova medição no sensor.
        aht10_read(aht10_lectures); // Lê os valores de temperatura e umidade.

        // Verifica condições de perigo e exibe mensagens no display.
        if (aht10_lectures[1] < 20 || aht10_lectures[0] > 70) {
            char alerta[13];
            sprintf(alerta, "Ambiente Ruim");
            draw_display((char*)alerta, 6);
        }

        // Formata string com temperatura e umidade.
        sprintf(texto_temperatura, "GRAUS  %.1f°C", aht10_lectures[1]);
        sprintf(texto_umidade, "UMIDADE  %.1f%%", aht10_lectures[0]);

        // Desenha texto de monitoramento no display.
        draw_display((char*)texto_temperatura, 2);
        draw_display((char*)texto_umidade, 4);

        show_on_display();

        sleep_ms(1000); // Pausa por 1 segundo antes da próxima iteração.
    }
}