#include "libs/aht10.h"
#include "pico/stdlib.h"

aht10 sensor;
int input_read;

void aht10_init(uint SDA_PIN, uint SCL_PIN, i2c_inst_t* I2C_PORT) {
    sensor.SDA_PIN = SDA_PIN;
    sensor.SCL_PIN = SCL_PIN;
    sensor.I2C_PORT = I2C_PORT;

    i2c_init(sensor.I2C_PORT, 100 * 1000); // Inicializa I2C com 100 kHz.
    gpio_set_function(sensor.SDA_PIN, GPIO_FUNC_I2C); // Configura SDA para I2C.
    gpio_set_function(sensor.SCL_PIN, GPIO_FUNC_I2C); // Configura SCL para I2C.
    gpio_pull_up(sensor.SDA_PIN); // Habilita pull-up no pino SDA.
    gpio_pull_up(sensor.SCL_PIN); // Habilita pull-up no pino SCL.

    uint8_t cmd[] = {0xBE, 0x08, 0x00}; // Comando de calibração/inicialização AHT10.
    i2c_write_blocking(sensor.I2C_PORT, AHT10_ADDR, cmd, 3, false); // Envia comando ao sensor.
}

void aht10_trigger_monitoring() // Inicia uma nova medição no AHT10.
{
    uint8_t cmd[] = {0xAC, 0x33, 0x00}; // Comando para iniciar medição.
    i2c_write_blocking(sensor.I2C_PORT, AHT10_ADDR, cmd, 3, false); // Envia comando de trigger.
    sleep_ms(50); // Aguarda o tempo necessário para a medição.
}

void aht10_read(float data_output[]) // Lê dados de temperatura e umidade do AHT10.
{
    uint8_t data[6]; // Buffer para armazenar os 6 bytes de dados.
    input_read = i2c_read_blocking(sensor.I2C_PORT, AHT10_ADDR, data, 6, false); // Lê 6 bytes.

    // Decodifica os bytes brutos para umidade e temperatura.
    uint32_t umidade_raw = ((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | (data[3] >> 4);
    uint32_t temperatura_raw = (((uint32_t)data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];

    // Converte os valores brutos para umidade (%) e temperatura (°C).
    data_output[0] = ((float)umidade_raw / 1048576.0f);
    data_output[1] = ((float)temperatura_raw / 1048576.0f) * 200.0f - 50.0f;
}