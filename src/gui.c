#include "gui.h"
#include "sht40.h"
#include "ens160.h"
#include "oled.h"
#include "gatt.h"

#define SLEEP_TIME_MS 100
#define SW0_NODE DT_ALIAS(sw0)
#define SW1_NODE DT_ALIAS(sw1)

bool button0_pressed = false;
bool button1_pressed = false;
bool sht40_flag = true;
bool ens160_flag = false;

static const struct gpio_dt_spec button_0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static const struct gpio_dt_spec button_1 = GPIO_DT_SPEC_GET(SW1_NODE, gpios);

void button_init(void)
{
    int ret;
    if (!device_is_ready(button_0.port))
    {
        return;
    }

    if (!device_is_ready(button_1.port))
    {
        return;
    }

    ret = gpio_pin_configure_dt(&button_0, GPIO_INPUT);
    if (ret < 0)
    {
        return;
    }
    
    ret = gpio_pin_configure_dt(&button_1, GPIO_INPUT);
    if (ret < 0)
    {
        return;
    }
}

void polling_button(void)
{

    bool current_button0_state = gpio_pin_get_dt(&button_0);
    bool current_button1_state = gpio_pin_get_dt(&button_1);

    // Check for button 4 press
    if (current_button0_state && !button0_pressed)
    {
        button0_pressed = true;
        sht40_flag = 1;
        ens160_flag = 0;
        OLED_Clear();
    }
    else if (!current_button0_state)
    {
        button0_pressed = false;
    }

    // Check for button 5 press
    if (current_button1_state && !button1_pressed)
    {
        button1_pressed = true;
        sht40_flag = 0;
        ens160_flag = 1;
        OLED_Clear();
    }
    else if (!current_button1_state)
    {
        button1_pressed = false;
    }

    if (sht40_flag)
    {
        sht40_read_raw_data();
        read_and_display_sht40_data();

        OLED_ShowString(0, 0, "Temper:", 8, 1); 
        OLED_ShowString(52, 14, "*C", 8, 1);    

        OLED_ShowString(0, 26, "Humidity:", 8, 1); 
        OLED_ShowString(55, 40, "%", 8, 1);        
        OLED_Refresh();
    }
    else if (ens160_flag)
    {
        ens160_result();
        OLED_ShowString(0, 0, "CO2:", 8, 1); 
        OLED_ShowString(25, 14, "ppm", 8, 1);

        OLED_ShowString(0, 26, "TVOC:", 8, 1); 
        OLED_ShowString(25, 40, "ppb", 8, 1);  
        OLED_Refresh();
    }
    sensors_data_update();
}
