/*****************************************************
* File			:	sht2x_b.h
* Brief			:	sht2x_b���ģ��
* Date           Author       Version
* 2017-05-24     DangQiang    V1.0
******************************************************/
#ifndef __SHT2X_B_H__
#define __SHT2X_B_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include <stm32f2xx.h>
#include "delay.h"
#include "sys.h"
#include "process.h"
	 
/********��������********/ 
#define SHT2X_B_GPIO_RCC		RCC_AHB1Periph_GPIOE	 
#define SHT2X_B_I2C_PORT		GPIOE
#define SHT2X_B_SCL_PIN			GPIO_Pin_13
#define SHT2X_B_SDA_PIN			GPIO_Pin_12	
	 
#define SHT2X_B_SCL					PEout(13)		/*SCL*/
#define SHT2X_B_SDA					PEout(12)		/*SDA*/ 
	 
#define SHT2X_B_SCL_IN			{GPIOE->MODER&=(~((unsigned int)3 << 26));}	/*SCLΪ�����*/
#define SHT2X_B_SCL_OUT			{GPIOE->MODER&=(~((unsigned int)3 << 26));GPIOE->MODER|=((unsigned int)1 << 26);}/*SCLΪ�����*/
	 
#define SHT2X_B_SDA_IN			{GPIOE->MODER&=(~((unsigned int)3 << 24));}	/*SDAΪ�����*/
#define SHT2X_B_SDA_OUT			{GPIOE->MODER&=(~((unsigned int)3 << 24));GPIOE->MODER|=((unsigned int)1 << 24);}/*SDAΪ�����*/
	 
#define SHT2X_B_SCL_STATE()       GPIO_ReadInputDataBit(SHT2X_B_I2C_PORT, SHT2X_B_SCL_PIN)
#define SHT2X_B_SDA_STATE()       GPIO_ReadInputDataBit(SHT2X_B_I2C_PORT, SHT2X_B_SDA_PIN)

extern void SHT2X_B_Init(void);//SHT2X_B��ʼ��
extern void SHT2X_B_Measure(void);//SHT2X_B����


#ifdef __cplusplus
}
#endif

#endif
