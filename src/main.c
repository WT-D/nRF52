#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>

#include <zephyr/settings/settings.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include "sht40.h"
#include "ens160.h"
#include "oled.h"
#include "gui.h"

#define SLEEP_TIME_MS 100
#define STACKSIZE 2048
#define PRIORITY 7
#define PRIORITY_ens160 6

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_ESS_VAL)),
};

static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		printk("Connection failed (err 0x%02x)\n", err);
	} else {
		printk("Connected\n");
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	printk("Disconnected (reason 0x%02x)\n", reason);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

static void bt_ready(void)
{
	int err;

	printk("Bluetooth initialized\n");

	if (IS_ENABLED(CONFIG_SETTINGS)) {
		settings_load();
	}

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	printk("Advertising successfully started\n");
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Pairing cancelled: %s\n", addr);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.cancel = auth_cancel,
};

void gui_thread(void)
{
	while (1)
	{
		polling_button();
		k_sleep(K_MSEC(SLEEP_TIME_MS));
	}
	
}

int main(void)
{
    int err;

	sht40_init();
	ens160_init();
	button_init();
	OLED_Init();
	OLED_Clear();
	OLED_ColorTurn(0);
    OLED_DisplayTurn(0);
	OLED_ShowString(0,0,"Temper:",8,1);
	OLED_ShowString(52,14,"*C",8,1);
		
	OLED_ShowString(0,26,"Humidity:",8,1);
	OLED_ShowString(55,40,"%",8,1);
	OLED_Refresh();

	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return 0;
	}
	bt_ready();
	bt_conn_auth_cb_register(&auth_cb_display);

	return 0;
}

K_THREAD_DEFINE(gui_thread_id, STACKSIZE, gui_thread, NULL, NULL, NULL, PRIORITY, 0, 0);

