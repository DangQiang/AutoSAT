/*****************************************************
* File			:	eeprom.h
* Brief			:	EEPROM存储模块头文件
* Date           Author       Version
* 2017-05-25     DangQiang    V1.1
******************************************************/
#ifndef __EEPROM_H
#define __EEPROM_H
 
/* Includes */
#include "eeprom.h"
#include "sys.h"
#include "delay.h"
	 
/********引脚配置********/ 
#define IIC1_SDA_IN		{GPIOC->MODER&=(~((unsigned int)3 << 22));}	/*SDA为输入口*/
#define IIC1_SDA_OUT	{GPIOC->MODER&=(~((unsigned int)3 << 22));GPIOC->MODER|=((unsigned int)1 << 22);}/*SDA为输出口*/

#define IIC1_SCL			PCout(10)		/*SCL*/
#define IIC1_SDA			PCout(11)		/*SDA*/ 
#define READ1_SDA			PCin(11)		/*输入SDA*/ 

#define iic1_rcc			RCC_AHB1Periph_GPIOC
#define iic1_gpio			GPIOC
#define iic1_scl_pin	(GPIO_Pin_10)
#define iic1_sda_pin	(GPIO_Pin_11)

/********数据存储地址定义0~254********/
#define PROD_NUM   0	//仪器编号
#define HARD_REV   1	//下位机硬件版本号

/********开放函数接口********/
uint8_t AT24C02_Init(void);																		/*初始化24C02的IIC接口，并自检，通过返回1，失败返回0*/
uint8_t AT24C02_ReadOneByte(uint8_t ReadAddr);								/*在AT24C02指定地址读出一个数据*/
uint8_t AT24C02_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite);	/*在AT24C02指定地址写入一个数据*/

#endif 

/************************************END OF FILE********************************/
