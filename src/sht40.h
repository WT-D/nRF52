#ifndef __SHT40_H_
#define __SHT40_H_

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/types.h>

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include "stdlib.h"	
#include <stdbool.h>   

#include "oled.h"

void sht40_init(void);
void sht40_read_raw_data(void);
float calculate_temperature(void);
float calculate_humidity(void);
void read_and_display_sht40_data(void);

#endif