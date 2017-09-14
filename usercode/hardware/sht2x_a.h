/*****************************************************
* File			:	sht2x_a.h
* Brief			:	sht2x_a检测模块
* Date           Author       Version
* 2017-05-24     DangQiang    V1.0
******************************************************/
#ifndef __SHT2X_A_H__
#define __SHT2X_A_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include <stm32f2xx.h>
#include "delay.h"
#include "sys.h"

/********引脚配置********/ 
#define SHT2X_A_GPIO_RCC		RCC_AHB1Periph_GPIOE	 
#define SHT2X_A_I2C_PORT		GPIOE
#define SHT2X_A_SCL_PIN			GPIO_Pin_11
#define SHT2X_A_SDA_PIN			GPIO_Pin_10	
	 
#define SHT2X_A_SCL					PEout(11)		/*SCL*/
#define SHT2X_A_SDA					PEout(10)		/*SDA*/ 
	 
#define SHT2X_A_SCL_IN			{GPIOE->MODER&=(~((unsigned int)3 << 22));}	/*SCL为输入口*/
#define SHT2X_A_SCL_OUT			{GPIOE->MODER&=(~((unsigned int)3 << 22));GPIOE->MODER|=((unsigned int)1 << 22);}/*SCL为输出口*/
	 
#define SHT2X_A_SDA_IN			{GPIOE->MODER&=(~((unsigned int)3 << 20));}	/*SDA为输入口*/
#define SHT2X_A_SDA_OUT			{GPIOE->MODER&=(~((unsigned int)3 << 20));GPIOE->MODER|=((unsigned int)1 << 20);}/*SDA为输出口*/
	 
#define SHT2X_A_SCL_STATE()       GPIO_ReadInputDataBit(SHT2X_A_I2C_PORT, SHT2X_A_SCL_PIN)
#define SHT2X_A_SDA_STATE()       GPIO_ReadInputDataBit(SHT2X_A_I2C_PORT, SHT2X_A_SDA_PIN)

typedef enum {
    TRIG_TEMP_MEASUREMENT_HM   = 0xE3, //command trig. temp meas. hold master
    TRIG_HUMI_MEASUREMENT_HM   = 0xE5, //command trig. humidity meas. hold master
    TRIG_TEMP_MEASUREMENT_POLL = 0xF3, //command trig. temp meas. no hold master
    TRIG_HUMI_MEASUREMENT_POLL = 0xF5, //command trig. humidity meas. no hold master
    USER_REG_W                 = 0xE6, //command writing user register
    USER_REG_R                 = 0xE7, //command reading user register
    SOFT_RESET                 = 0xFE  //command soft reset
} SHT2X_Command;

typedef enum {
    SHT2X_A_RES_12_14BIT         = 0x00, //RH=12bit, T=14bit
    SHT2X_A_RES_8_12BIT          = 0x01, //RH= 8bit, T=12bit
    SHT2X_A_RES_10_13BIT         = 0x80, //RH=10bit, T=13bit
    SHT2X_A_RES_11_11BIT         = 0x81, //RH=11bit, T=11bit
    SHT2X_A_RES_MASK             = 0x81  //Mask for res. bits (7,0) in user reg.
} SHT2X_Resolution;

typedef enum {
    SHT2X_A_EOB_ON               = 0x40, //end of battery
    SHT2X_A_EOB_MASK             = 0x40  //Mask for EOB bit(6) in user reg.
} SHT2X_Eob;

typedef enum {
    SHT2X_A_HEATER_ON            = 0x04, //heater on
    SHT2X_A_HEATER_OFF           = 0x00, //heater off
    SHT2X_A_HEATER_MASK          = 0x04  //Mask for Heater bit(2) in user reg.
} SHT2X_Heater;

typedef enum {
    TEMP,  
    HUMI 
} SHT2X_MeasureType;

typedef enum {
    I2C_ADR_W                  = 0x80, //sensor I2C address + write bit
    I2C_ADR_R                  = 0x81  //sensor I2C address + read bit
} SHT2X_I2cHeader;

typedef enum {
    ACK                        = 0,
    NO_ACK                     = 1
} SHT2X_I2cAck;

typedef enum {
    ACK_OK                     = 0x00,  
    ACK_ERROR                  = 0x01
} SHT2X_I2cAckState;

typedef struct {
    uint8_t SerialNumber[8];	//序列号	
		rt_uint8_t	Exist_flag;		//传感器存在标志位	
		rt_uint8_t	Enable_flag;	//传感器采集使能标志位	
		rt_uint8_t	Collect_time;	//传感器采集间隔		
		rt_uint32_t	ErrorCount;		//CRC校验或通信失败计数
    float Temperature_HM;	//温度值
    float Humidity_HM;		//湿度值
} SHT2X_PARAM;

extern SHT2X_PARAM g_sht2x_result[3];//传感器数据

extern void SHT2X_A_Init(void);//SHT2X_A初始化
extern void SHT2X_A_Measure(void);//SHT2X_A测量


#ifdef __cplusplus
}
#endif

#endif
