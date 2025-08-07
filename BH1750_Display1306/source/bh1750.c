#include "libs/bh1750.h"
#include "pico/stdlib.h"

bh1750 sensor;

uint16_t bh1750_read() // Lê luminosidade do sensor BH1750.
{
    uint8_t data[2]; // Buffer para dados lidos do sensor.
    i2c_read_blocking(sensor.I2C_PORT, BH1750_ADDR, data, 2, false); // Lê 2 bytes do sensor.
    sensor.lecture = (data[0] << 8 | data[1]);
    return sensor.lecture; // Combina bytes para valor 16 bits.
}

void bh1750_start_monitoring() // Envia comando para BH1750 iniciar medição.
{
    uint8_t cmd = BH1750_CMD_START; // Comando de início de medição.
    i2c_write_blocking(sensor.I2C_PORT, BH1750_ADDR, &cmd, 1, false); // Envia comando ao sensor.
}

void config_bh1750(uint SDA_PIN, uint SCL_PIN, i2c_inst_t* I2C_PORT) {
    sensor.SDA_PIN = SDA_PIN;
    sensor.SCL_PIN = SCL_PIN;
    sensor.I2C_PORT = I2C_PORT;

    i2c_init(sensor.I2C_PORT, 100 * 1000); // Inicializa I2C com 100 kHz.
    gpio_set_function(sensor.SDA_PIN, GPIO_FUNC_I2C); // Configura SDA para I2C.
    gpio_set_function(sensor.SCL_PIN, GPIO_FUNC_I2C); // Configura SCL para I2C.
    gpio_pull_up(sensor.SDA_PIN); // Habilita pull-up no pino SDA.
    gpio_pull_up(sensor.SCL_PIN); // Habilita pull-up no pino SCL.
}