#include <stdio.h>          // Funções padrão de entrada/saída (printf).
#include "pico/stdlib.h"    // Funções padrão do Raspberry Pi Pico.
#include "libs/display.h"
#include "hardware/i2c.h"   // Biblioteca para comunicação I2C.

// Configuração do sensor AHT10 (I2C)
#define I2C_PORT_1 i2c0       // Define a porta I2C (i2c0).
#define AHT10_ADDR 0x38     // Endereço I2C do sensor AHT10.
#define SDA_AHT10 0         // Pino GPIO para SDA do AHT10.
#define SCL_AHT10 1        // Pino GPIO para SCL do AHT10.

float temperatura = 0.0, umidade = 0.0; // Variáveis globais para temperatura e umidade.

void aht10_init() // Inicializa o sensor AHT10.
{
    uint8_t cmd[] = {0xBE, 0x08, 0x00}; // Comando de calibração/inicialização AHT10.
    i2c_write_blocking(I2C_PORT_1, AHT10_ADDR, cmd, 3, false); // Envia comando ao sensor.
}

void aht10_trigger_monitoring() // Inicia uma nova medição no AHT10.
{
    uint8_t cmd[] = {0xAC, 0x33, 0x00}; // Comando para iniciar medição.
    i2c_write_blocking(I2C_PORT_1, AHT10_ADDR, cmd, 3, false); // Envia comando de trigger.
    sleep_ms(50); // Aguarda o tempo necessário para a medição.
}

void aht10_read() // Lê dados de temperatura e umidade do AHT10.
{
    uint8_t data[6]; // Buffer para armazenar os 6 bytes de dados.
    int input_read = i2c_read_blocking(I2C_PORT_1, AHT10_ADDR, data, 6, false); // Lê 6 bytes.
    if (input_read != 6) printf("Erro. Leitura incorreta do sensor.\n"); // Verifica erros na leitura.

    // Decodifica os bytes brutos para umidade e temperatura.
    uint32_t umidade_raw = ((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | (data[3] >> 4);
    uint32_t temperatura_raw = (((uint32_t)data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];

    // Converte os valores brutos para umidade (%) e temperatura (°C).
    umidade = ((float)umidade_raw / 1048576.0f);
    temperatura = ((float)temperatura_raw / 1048576.0f) * 200.0f - 50.0f;
}

void init_pins() // Inicializa pinos I2C e o display.
{
    i2c_init(I2C_PORT_1, 100 * 1000); // Inicializa I2C com 100 kHz.
    gpio_set_function(SDA_AHT10, GPIO_FUNC_I2C); // Configura SDA para I2C.
    gpio_set_function(SCL_AHT10, GPIO_FUNC_I2C); // Configura SCL para I2C.
    gpio_pull_up(SDA_AHT10); // Habilita pull-up no pino SDA.
    gpio_pull_up(SCL_AHT10); // Habilita pull-up no pino SCL.

    aht10_init(); // Inicializa o sensor AHT10.

    init_display();
}

int main() { // Função principal do programa.
    stdio_init_all(); // Inicializa console serial.
    init_pins(); // Inicializa pinos e periféricos.

    char texto_temperatura[15]; // Buffer para o texto de monitoramento.
    char texto_umidade[15];

    while (true) { // Loop infinito para leituras e atualizações.
        aht10_trigger_monitoring(); // Aciona nova medição no sensor.
        aht10_read(); // Lê os valores de temperatura e umidade.

        // Verifica condições de perigo e exibe mensagens no display.
        if (temperatura < 20 || umidade > 70) {
            char alerta[13];
            sprintf(alerta, "Ambiente Ruim");
            draw_display((char*)alerta, 6);
        }

        // Formata string com temperatura e umidade.
        sprintf(texto_temperatura, "GRAUS  %.1f°C", temperatura);
        sprintf(texto_umidade, "UMIDADE  %.1f%%", umidade);

        // Desenha texto de monitoramento no display.
        draw_display((char*)texto_temperatura, 2);
        draw_display((char*)texto_umidade, 4);

        show_on_display();

        sleep_ms(1000); // Pausa por 1 segundo antes da próxima iteração.
    }
}