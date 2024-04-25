#ifndef __GATT_H_
#define __GATT_H_

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/types.h>
#include <zephyr/drivers/sensor.h>

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include "ens160.h"
#include "sht40.h"

void sensors_data_update(void);

#endif