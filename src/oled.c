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

#include "font.h"
#include "oled.h"

#define I2C0_NODE DT_NODELABEL(oled)
static const struct i2c_dt_spec dev_i2c = I2C_DT_SPEC_GET(I2C0_NODE);

uint8_t OLED_GRAM[80][6];

void OLED_WR_Byte(uint8_t dat,uint8_t mode)
{
    int ret;
    uint8_t command[2];

    command[0] = mode ? 0x40 : 0x00; // Set mode flag
    command[1] = dat; // Set data byte

    ret = i2c_write_dt(&dev_i2c, command, sizeof(command));
    if (ret != 0) {
        printk("Failed to write to I2C device address 0x%x \n", dev_i2c.addr);
        return;
    }
}

void OLED_ColorTurn(uint8_t i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,OLED_CMD);
		}
}

void OLED_DisplayTurn(uint8_t i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,OLED_CMD);
			OLED_WR_Byte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);
			OLED_WR_Byte(0xA0,OLED_CMD);
		}
}

void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);
	OLED_WR_Byte(0x14,OLED_CMD);
	OLED_WR_Byte(0xAF,OLED_CMD);
}

void OLED_Refresh(void)
{
    uint8_t i, n;
    int ret;
    uint8_t buffer[65]; // 1 byte for command + 64 bytes for data

    for (i = 0; i < 6; i++) // Assuming 6 pages
    {
        // Set page address
        OLED_WR_Byte(0xb0 + i, OLED_CMD); // Command to set page start address
        OLED_WR_Byte(0x00, OLED_CMD);     // Command to set lower column start address
        OLED_WR_Byte(0x12, OLED_CMD);     // Command to set higher column start address

        buffer[0] = 0x40; // Data mode command

        // Assuming you only need to send the first 64 bytes of each row for the OLED display
        for (n = 0; n < 64; n++) // Only take the first 64 bytes of each row
        {
            buffer[n + 1] = OLED_GRAM[n][i]; // Copy data from OLED_GRAM into buffer
        }

        // Send the combined data mode command and page data in one I2C transaction
        ret = i2c_write_dt(&dev_i2c, buffer, sizeof(buffer)); // 65 bytes total
        if (ret != 0)
        {
            printk("Failed to write to I2C device at address 0x%x\n", dev_i2c.addr);
            return;
        }
    }
}

void OLED_Clear(void)
{
	uint8_t i,n;
	for(i=0;i<6;i++)
	{
	   for(n=0;n<64;n++)
			{
			 OLED_GRAM[n][i]=0;
			}
  }
	OLED_Refresh();
}
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	if(t){OLED_GRAM[x][i]|=n;}
	else
	{
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
		OLED_GRAM[x][i]|=n;
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
	}
}
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode)
{
	uint8_t i,m,temp,size2,chr1;
	uint8_t x0=x,y0=y;
	if(size1==8)size2=6;
	else size2=(size1/8+((size1%8)?1:0))*(size1/2);
	chr1=chr-' '; 
	for(i=0;i<size2;i++)
	{
		if(size1==8)
			  {temp=asc2_0806[chr1][i];}
		else if(size1==12)
        {temp=asc2_1206[chr1][i];} 
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((size1!=8)&&((x-x0)==size1/2))
		{x=x0;y0=y0+8;}
		y=y0;
  }
}

void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t size1,uint8_t mode)
{
	while((*chr>=' ')&&(*chr<='~'))
	{
		OLED_ShowChar(x,y,*chr,size1,mode);
		if(size1==8)x+=6;
		else x+=size1/2;
		chr++;
  }
}

void OLED_Init(void)
{		
	OLED_WR_Byte(0xAE,OLED_CMD); /*display off*/
	OLED_WR_Byte(0x00,OLED_CMD); /*set lower column address*/ 
    OLED_WR_Byte(0x12,OLED_CMD); /*set higher column address*/
	OLED_WR_Byte(0x40,OLED_CMD); /*set display start line*/ 
	OLED_WR_Byte(0xB0,OLED_CMD); /*set page address*/ 
	OLED_WR_Byte(0x81,OLED_CMD); /*contract control*/ 
	OLED_WR_Byte(0x7f,OLED_CMD); /*128*/ 
	OLED_WR_Byte(0xA1,OLED_CMD); /*set segment remap*/ 
	OLED_WR_Byte(0xA6,OLED_CMD); /*normal / reverse*/ 
	OLED_WR_Byte(0xA8,OLED_CMD); /*multiplex ratio*/ 
	OLED_WR_Byte(0x2F,OLED_CMD); /*duty = 1/48*/ 
	OLED_WR_Byte(0xC8,OLED_CMD); /*Com scan direction*/ 
	OLED_WR_Byte(0xD3,OLED_CMD); /*set display offset*/ 
	OLED_WR_Byte(0x00,OLED_CMD); 
	OLED_WR_Byte(0xD5,OLED_CMD); /*set osc division*/ 
	OLED_WR_Byte(0x80,OLED_CMD); 
	OLED_WR_Byte(0xD9,OLED_CMD); /*set pre-charge period*/ 
	OLED_WR_Byte(0x21,OLED_CMD); 
	OLED_WR_Byte(0xDA,OLED_CMD); /*set COM pins*/ 
	OLED_WR_Byte(0x12,OLED_CMD); 
	OLED_WR_Byte(0xdb,OLED_CMD); /*set vcomh*/ 
	OLED_WR_Byte(0x40,OLED_CMD); 
	OLED_WR_Byte(0x8d,OLED_CMD); /*set charge pump enable*/ 
	OLED_WR_Byte(0x14,OLED_CMD);
	OLED_Clear();
	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 

}


