#include "sht40.h"

uint8_t SHT4X_CMD_MEASURE_HPM = 0xFD;
uint8_t SHT4X_CMD_MEASURE_LPM = 0xE0;
uint8_t SHT40_REST = 0X94;

#define I2C0_NODE DT_NODELABEL(sht40)
static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C0_NODE);

uint8_t rawData[6];
uint8_t str[50];

void sht40_init(void)
{
    int ret;
    if (!device_is_ready(dev_i2c.bus))
    {
        printk("I2C bus %s is not ready!\n\r", dev_i2c.bus->name);
        return;
    }

    ret = i2c_write_dt(&dev_i2c, &SHT40_REST, sizeof(SHT40_REST));
    if (ret != 0)
    {
        printk("Failed to write to I2C device address %x \n\r", dev_i2c.addr);
        return;
    }
    k_sleep(K_MSEC(20));

    ret = i2c_write_dt(&dev_i2c, &SHT4X_CMD_MEASURE_HPM, sizeof(SHT4X_CMD_MEASURE_HPM));
    if (ret != 0)
    {
        printk("Failed to write to I2C device address %x \n\r", dev_i2c.addr);
        return;
    }
    k_sleep(K_MSEC(20));
}

void sht40_read_raw_data(void)
{
    int ret;
    ret = i2c_write_dt(&dev_i2c, &SHT4X_CMD_MEASURE_HPM, sizeof(SHT4X_CMD_MEASURE_HPM));
    if (ret != 0)
    {
        printk("Failed to write to I2C device address %x \n\r", dev_i2c.addr);
        return;
    }
    k_sleep(K_MSEC(20));

    ret = i2c_read_dt(&dev_i2c, rawData, sizeof(rawData));
    if (ret != 0)
    {
        printk("Failed to read from I2C device");
        return; 
    }
}

float calculate_temperature(void)
{
    uint16_t temp = ((rawData[0] << 8) | rawData[1]);
    float temperature = (175.0 * (float)temp / 65535.0 - 45.0);
    return temperature;
}

float calculate_humidity(void)
{
    uint16_t humid = ((rawData[3] << 8) | rawData[4]);
    float humidity = (125.0 * (float)humid / 65535.0 - 6.0);
    return humidity;
}

void read_and_display_sht40_data(void)
{
    float Temperature = calculate_temperature();
    float Humidity = calculate_humidity();

    if ((Temperature >= -20) && (Temperature <= 125) && (Humidity >= 0) && (Humidity <= 100))
    {
        sprintf(str, "%.2f", Temperature);
        OLED_ShowString(20, 14, (uint8_t *)str, 8, 1);

        sprintf(str, "%.2f", Humidity);
        OLED_ShowString(20, 40, (uint8_t *)str, 8, 1);
        OLED_Refresh();
    }
}
