/*****************************************************
* File			:	sht2x_c.h
* Brief			:	sht2x_c���ģ��
* Date           Author       Version
* 2017-05-24     DangQiang    V1.0
******************************************************/
#ifndef __SHT2X_C_H__
#define __SHT2X_C_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include <stm32f2xx.h>
#include "delay.h"
#include "sys.h"
#include "process.h"	 

/********��������********/ 
#define SHT2X_C_GPIO_RCC		RCC_AHB1Periph_GPIOE	 
#define SHT2X_C_I2C_PORT		GPIOE
#define SHT2X_C_SCL_PIN			GPIO_Pin_15
#define SHT2X_C_SDA_PIN			GPIO_Pin_14	
	 
#define SHT2X_C_SCL					PEout(15)		/*SCL*/
#define SHT2X_C_SDA					PEout(14)		/*SDA*/ 
	 
#define SHT2X_C_SCL_IN			{GPIOE->MODER&=(~((unsigned int)3 << 30));}	/*SCLΪ�����*/
#define SHT2X_C_SCL_OUT			{GPIOE->MODER&=(~((unsigned int)3 << 30));GPIOE->MODER|=((unsigned int)1 << 30);}/*SCLΪ�����*/
	 
#define SHT2X_C_SDA_IN			{GPIOE->MODER&=(~((unsigned int)3 << 28));}	/*SDAΪ�����*/
#define SHT2X_C_SDA_OUT			{GPIOE->MODER&=(~((unsigned int)3 << 28));GPIOE->MODER|=((unsigned int)1 << 28);}/*SDAΪ�����*/
	 
#define SHT2X_C_SCL_STATE()       GPIO_ReadInputDataBit(SHT2X_C_I2C_PORT, SHT2X_C_SCL_PIN)
#define SHT2X_C_SDA_STATE()       GPIO_ReadInputDataBit(SHT2X_C_I2C_PORT, SHT2X_C_SDA_PIN)

extern void SHT2X_C_Init(void);//SHT2X_C��ʼ��
extern void SHT2X_C_Measure(void);//SHT2X_C����


#ifdef __cplusplus
}
#endif

#endif
