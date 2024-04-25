#include "gatt.h"

#define co2         0
#define tvoc        1
#define airq_index  2

static struct bt_uuid_128 uuid_tvoc = BT_UUID_INIT_128(
    BT_UUID_128_ENCODE(0xbef93f88, 0xa801, 0x405b, 0xbc21, 0xc42d39885535));

static struct bt_uuid_128 uuid_airq_index = BT_UUID_INIT_128(
    BT_UUID_128_ENCODE(0xcb11e4df, 0x70d3, 0x4f4d, 0xb5ad, 0x6670a723a3a0));

static struct bt_uuid_128 uuid_co2 = BT_UUID_INIT_128(
    BT_UUID_128_ENCODE(0x1519693d, 0xbb8f, 0x4504, 0xaff1, 0xfe9eccf6f03f));

// Client Characteristic Configuration Notification
static bool ess_notify;
static void ess_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
    ess_notify = (value == BT_GATT_CCC_NOTIFY);
}

BT_GATT_SERVICE_DEFINE(ess_svc,
                       BT_GATT_PRIMARY_SERVICE(BT_UUID_ESS),

                       // Temperture
                       BT_GATT_CHARACTERISTIC(BT_UUID_TEMPERATURE, BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_NONE, NULL, NULL, NULL),
                       BT_GATT_CCC(ess_ccc_cfg_changed,
                                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

                       // Humidity
                       BT_GATT_CHARACTERISTIC(BT_UUID_HUMIDITY, BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_NONE, NULL, NULL, NULL),
                       BT_GATT_CCC(ess_ccc_cfg_changed,
                                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

                       // CO2
                       BT_GATT_CHARACTERISTIC(&uuid_co2.uuid, BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_NONE, NULL, NULL, NULL),
                       BT_GATT_CCC(ess_ccc_cfg_changed,
                                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

                       // TVOC
                       BT_GATT_CHARACTERISTIC(&uuid_tvoc.uuid, BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_NONE, NULL, NULL, NULL),
                       BT_GATT_CCC(ess_ccc_cfg_changed,
                                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

                       // AQI
                       BT_GATT_CHARACTERISTIC(&uuid_airq_index.uuid, BT_GATT_CHRC_NOTIFY,
                                              BT_GATT_PERM_NONE, NULL, NULL, NULL),
                       BT_GATT_CCC(ess_ccc_cfg_changed,
                                   BT_GATT_PERM_READ | BT_GATT_PERM_WRITE), );

void sensors_data_update(void)
{
    if (!ess_notify)
        return;

    int16_t eco2 = ens160_read_data(co2);
    int16_t tvoc_data = ens160_read_data(tvoc);
    uint8_t air_q_index = ens160_read_data(airq_index);
    float Temperature = calculate_temperature();
    float Humidity = calculate_humidity();

    // Ensure the environmental conditions are within the expected range before notifying.
    if ((Temperature >= -20) && (Temperature <= 125) && (Humidity >= 0) && (Humidity <= 100)) {
        int intTemp = (int)(Temperature * 100);
        int intHumidity = (int)(Humidity * 100);

        uint16_t val = sys_cpu_to_le16(intTemp);
        // Notify for temperature. The attribute index for temperature characteristic is [2].
        bt_gatt_notify(NULL, &ess_svc.attrs[2], &val, sizeof(val));
        
        val = sys_cpu_to_le16(intHumidity);
        // Notify for humidity. The attribute index for humidity characteristic is [4].
        bt_gatt_notify(NULL, &ess_svc.attrs[4], &val, sizeof(val));
    }

    uint16_t val_co2 = sys_cpu_to_le16(eco2);
    // Notify for CO2. The attribute index for CO2 characteristic is [6].
    bt_gatt_notify(NULL, &ess_svc.attrs[6], &val_co2, sizeof(val_co2));

    uint16_t val_tvoc = sys_cpu_to_le16(tvoc_data);
    // Notify for TVOC. The attribute index for TVOC characteristic is [8].
    bt_gatt_notify(NULL, &ess_svc.attrs[8], &val_tvoc, sizeof(val_tvoc));

    uint16_t val_airq = sys_cpu_to_le16(air_q_index);
    // Notify for Air Quality Index. The attribute index for Air Quality Index characteristic is [10].
    bt_gatt_notify(NULL, &ess_svc.attrs[10], &val_airq, sizeof(val_airq));
}