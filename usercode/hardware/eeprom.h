/*****************************************************
* File			:	eeprom.h
* Brief			:	EEPROM�洢ģ��ͷ�ļ�
* Date           Author       Version
* 2017-05-25     DangQiang    V1.1
******************************************************/
#ifndef __EEPROM_H
#define __EEPROM_H
 
/* Includes */
#include "eeprom.h"
#include "sys.h"
#include "delay.h"
	 
/********��������********/ 
#define IIC1_SDA_IN		{GPIOC->MODER&=(~((unsigned int)3 << 22));}	/*SDAΪ�����*/
#define IIC1_SDA_OUT	{GPIOC->MODER&=(~((unsigned int)3 << 22));GPIOC->MODER|=((unsigned int)1 << 22);}/*SDAΪ�����*/

#define IIC1_SCL			PCout(10)		/*SCL*/
#define IIC1_SDA			PCout(11)		/*SDA*/ 
#define READ1_SDA			PCin(11)		/*����SDA*/ 

#define iic1_rcc			RCC_AHB1Periph_GPIOC
#define iic1_gpio			GPIOC
#define iic1_scl_pin	(GPIO_Pin_10)
#define iic1_sda_pin	(GPIO_Pin_11)

/********���ݴ洢��ַ����0~254********/
#define PROD_NUM   0	//�������
#define HARD_REV   1	//��λ��Ӳ���汾��

/********���ź����ӿ�********/
uint8_t AT24C02_Init(void);																		/*��ʼ��24C02��IIC�ӿڣ����Լ죬ͨ������1��ʧ�ܷ���0*/
uint8_t AT24C02_ReadOneByte(uint8_t ReadAddr);								/*��AT24C02ָ����ַ����һ������*/
uint8_t AT24C02_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite);	/*��AT24C02ָ����ַд��һ������*/

#endif 

/************************************END OF FILE********************************/
