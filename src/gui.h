#ifndef __GUI_H_
#define __GUI_H_

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

void button_init(void);
void polling_button(void);

#endif