#ifndef __ENS160_H_
#define __ENS160_H_

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

void ens160_init(void);
uint16_t ens160_read_data(uint8_t p_type);
void ens160_result(void);

#endif