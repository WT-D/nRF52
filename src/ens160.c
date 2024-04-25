#include "ens160.h"

// ADDR
uint8_t OPMODE_ADDR = 0x10;
uint8_t AIRQ_Index  = 0x21;
uint8_t TVOC_DATA   = 0x22;
uint8_t ECO2_DATA   = 0x24;
// CMD
uint8_t  IDLE_MODE = 0x01;
uint8_t  STANDARD_MODE = 0x02;
uint8_t  DSLEEP_MODE = 0x00;
uint8_t  ENS160_RESET = 0xF0;

#define co2         0
#define tvoc        1
#define airq_index  2

uint8_t str1[50];

#define I2C0_NODE DT_NODELABEL(ens160)
static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C0_NODE);

void ens160_init(void)
{
    int ret;
    uint8_t cmd[2] = {OPMODE_ADDR, STANDARD_MODE};   
    if (!device_is_ready(dev_i2c.bus))
    {
        printk("I2C bus %s is not ready!\n\r", dev_i2c.bus->name);
        return;
    }
    
    ret = i2c_write_dt(&dev_i2c, cmd, sizeof(cmd)); 
    if (ret != 0) {
        printk("Failed to set ENS160 to standard mode at address %x\n\r", dev_i2c.addr);
        return;
    }
}

uint16_t ens160_read_data(uint8_t p_type)
{
    uint8_t rawData[2];
    uint16_t result;
    int ret;

    switch (p_type) {
        case 0: i2c_write_dt(&dev_i2c, &ECO2_DATA, sizeof(ECO2_DATA)); break;
        case 1: i2c_write_dt(&dev_i2c, &TVOC_DATA, sizeof(TVOC_DATA)); break;
        case 2: i2c_write_dt(&dev_i2c, &AIRQ_Index, sizeof(AIRQ_Index)); break;
    }

    ret = i2c_read_dt(&dev_i2c, rawData, sizeof(rawData));
    if (ret != 0) {
        printk("Failed to read from I2C device for air index\n");
        return -1;
    }

    result = ((rawData[1] << 8) | rawData[0]);
    return result;
}

void ens160_result(void)
{
    int16_t eco2 = ens160_read_data(co2);
    int16_t tvoc_1 = ens160_read_data(tvoc);
    int16_t tvoc_2 = ens160_read_data(tvoc);
    uint8_t air_q_index = ens160_read_data(airq_index);

    if (tvoc_2 - tvoc_1 < 0)
    {
        OLED_Clear();
        sprintf((char*)str1, "%2d", tvoc_2);
        OLED_ShowString(0, 40, (uint8_t *)str1, 8, 1);
    }
    else
    {
        sprintf((char*)str1, "%3d", tvoc_1);
        OLED_ShowString(0, 40, (uint8_t *)str1, 8, 1);
    }

    sprintf((char*)str1, "%3d", eco2);
    OLED_ShowString(0, 14, (uint8_t *)str1, 8, 1);

    sprintf((char*)str1, "%d", air_q_index);
    OLED_ShowString(55, 40, (uint8_t *)str1, 8, 1);

    OLED_Refresh();
}
