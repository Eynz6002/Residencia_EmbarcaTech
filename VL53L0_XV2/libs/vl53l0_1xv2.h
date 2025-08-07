#ifndef VL53L0_1XV2_H
#define VL53L0_1XV2_H

#include <stdint.h>
#include "hardware/i2c.h"

#define VL53L0X_ADDR                  0x29

#define REG_IDENTIFICATION_MODEL_ID   0xC0
#define REG_SYSRANGE_START            0x00
#define REG_RESULT_RANGE_STATUS       0x14
#define REG_RESULT_RANGE_MM           0x1E

typedef struct {
    uint SDA_PIN;
    uint SCL_PIN;
    i2c_inst_t* I2C_PORT;
} vl53l0_1xv2;

int config_vl53l0_1xv2(uint SDA_PIN, uint SCL_PIN, i2c_inst_t* i2c_port);
int init_vl53l0_1xv2();
int read_distance();

#endif