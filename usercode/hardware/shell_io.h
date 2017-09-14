/*****************************************************
* File			:	shell_io.h
* Brief			:	shell_io控制模块
* Date           Author       Version
* 2017-05-25     DangQiang    V1.0
******************************************************/
#ifndef __SHELL_IO_H__
#define __SHELL_IO_H__

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include <stm32f2xx.h>
#include "delay.h"
#include "sys.h"
#include "process.h"

//输出		 
//PD3、4、5、6、7：外壳风扇，照明灯，锁1，锁2，蜂鸣器	 
//PB3、4、5、6：LED总电源，红色LED，黄色LED，绿色LED	 
#define	FAN_SHELL_OPEN	GPIO_SetBits(GPIOD, GPIO_Pin_3);	//风扇
#define	FAN_SHELL_CLOSE	GPIO_ResetBits(GPIOD, GPIO_Pin_3);	 

#define	LED_LIGHT_OPEN	GPIO_SetBits(GPIOD, GPIO_Pin_4);	//照明灯
#define	LED_LIGHT_CLOSE	GPIO_ResetBits(GPIOD, GPIO_Pin_4);

#define	LOCK1_POWER_ON	GPIO_SetBits(GPIOD, GPIO_Pin_5);	//锁1
#define	LOCK1_POWER_OFF	GPIO_ResetBits(GPIOD, GPIO_Pin_5);

#define	LOCK2_POWER_ON	GPIO_SetBits(GPIOD, GPIO_Pin_6);	//锁2
#define	LOCK2_POWER_OFF	GPIO_ResetBits(GPIOD, GPIO_Pin_6);	 

#define	BUZZ_OPEN		GPIO_SetBits(GPIOD, GPIO_Pin_7);			//蜂鸣器
#define	BUZZ_CLOSE	GPIO_ResetBits(GPIOD, GPIO_Pin_7);	

#define	LED_STATE_OPEN	GPIO_SetBits(GPIOB, GPIO_Pin_3);	//状态灯
#define	LED_STATE_CLOSE	GPIO_ResetBits(GPIOB, GPIO_Pin_3);	

#define	LED_RED_OPEN	GPIO_ResetBits(GPIOB, GPIO_Pin_4);	//红LED
#define	LED_RED_CLOSE	GPIO_SetBits(GPIOB, GPIO_Pin_4);	

#define	LED_YEL_OPEN	GPIO_ResetBits(GPIOB, GPIO_Pin_5);	//黄LED
#define	LED_YEL_CLOSE	GPIO_SetBits(GPIOB, GPIO_Pin_5);	

#define	LED_GRN_OPEN	GPIO_ResetBits(GPIOB, GPIO_Pin_6);	//绿LED
#define	LED_GRN_CLOSE	GPIO_SetBits(GPIOB, GPIO_Pin_6);	
	 
//输入
//PA4、6、7：急停、锁2、锁1	 
#define SENSOR_STOP_STATE		(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4))//急停开关状态
#define SENSOR_LOCK2_STATE	(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6))//LOCK2传感器状态
#define SENSOR_LOCK1_STATE	(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7))//LOCK1传感器状态
#define SENSOR_LOCK2_TRIG		(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) == 0)//LOCK2传感器触发
#define SENSOR_LOCK1_TRIG		(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7) == 0)//LOCK1传感器触发

extern void Shell_Io_Init(void);//Shell_Io_初始化
extern void Lamp_Handler(rt_uint8_t type,rt_uint8_t cmd);//灯处理函数

extern rt_uint8_t Lamp_Flag[4];	//lamp状态,0表示灭，1表示常亮
extern rt_uint8_t Lock_Flag[3];	//lock1,lock2,stop状态

extern rt_mailbox_t		mb_lock1_rece_data;	//LOCK1数据有效邮箱
extern rt_mailbox_t		mb_lock2_rece_data;	//LOCK2数据有效邮箱

extern rt_uint8_t door_main_open_flag;//main door
extern rt_uint8_t door_sub_open_flag;//subordinate door

#ifdef __cplusplus
}
#endif

#endif
