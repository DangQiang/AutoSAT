/*****************************************************
* File			:	shell_io.h
* Brief			:	shell_io����ģ��
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

//���		 
//PD3��4��5��6��7����Ƿ��ȣ������ƣ���1����2��������	 
//PB3��4��5��6��LED�ܵ�Դ����ɫLED����ɫLED����ɫLED	 
#define	FAN_SHELL_OPEN	GPIO_SetBits(GPIOD, GPIO_Pin_3);	//����
#define	FAN_SHELL_CLOSE	GPIO_ResetBits(GPIOD, GPIO_Pin_3);	 

#define	LED_LIGHT_OPEN	GPIO_SetBits(GPIOD, GPIO_Pin_4);	//������
#define	LED_LIGHT_CLOSE	GPIO_ResetBits(GPIOD, GPIO_Pin_4);

#define	LOCK1_POWER_ON	GPIO_SetBits(GPIOD, GPIO_Pin_5);	//��1
#define	LOCK1_POWER_OFF	GPIO_ResetBits(GPIOD, GPIO_Pin_5);

#define	LOCK2_POWER_ON	GPIO_SetBits(GPIOD, GPIO_Pin_6);	//��2
#define	LOCK2_POWER_OFF	GPIO_ResetBits(GPIOD, GPIO_Pin_6);	 

#define	BUZZ_OPEN		GPIO_SetBits(GPIOD, GPIO_Pin_7);			//������
#define	BUZZ_CLOSE	GPIO_ResetBits(GPIOD, GPIO_Pin_7);	

#define	LED_STATE_OPEN	GPIO_SetBits(GPIOB, GPIO_Pin_3);	//״̬��
#define	LED_STATE_CLOSE	GPIO_ResetBits(GPIOB, GPIO_Pin_3);	

#define	LED_RED_OPEN	GPIO_ResetBits(GPIOB, GPIO_Pin_4);	//��LED
#define	LED_RED_CLOSE	GPIO_SetBits(GPIOB, GPIO_Pin_4);	

#define	LED_YEL_OPEN	GPIO_ResetBits(GPIOB, GPIO_Pin_5);	//��LED
#define	LED_YEL_CLOSE	GPIO_SetBits(GPIOB, GPIO_Pin_5);	

#define	LED_GRN_OPEN	GPIO_ResetBits(GPIOB, GPIO_Pin_6);	//��LED
#define	LED_GRN_CLOSE	GPIO_SetBits(GPIOB, GPIO_Pin_6);	
	 
//����
//PA4��6��7����ͣ����2����1	 
#define SENSOR_STOP_STATE		(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4))//��ͣ����״̬
#define SENSOR_LOCK2_STATE	(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6))//LOCK2������״̬
#define SENSOR_LOCK1_STATE	(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7))//LOCK1������״̬
#define SENSOR_LOCK2_TRIG		(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6) == 0)//LOCK2����������
#define SENSOR_LOCK1_TRIG		(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7) == 0)//LOCK1����������

extern void Shell_Io_Init(void);//Shell_Io_��ʼ��
extern void Lamp_Handler(rt_uint8_t type,rt_uint8_t cmd);//�ƴ�����

extern rt_uint8_t Lamp_Flag[4];	//lamp״̬,0��ʾ��1��ʾ����
extern rt_uint8_t Lock_Flag[3];	//lock1,lock2,stop״̬

extern rt_mailbox_t		mb_lock1_rece_data;	//LOCK1������Ч����
extern rt_mailbox_t		mb_lock2_rece_data;	//LOCK2������Ч����

extern rt_uint8_t door_main_open_flag;//main door
extern rt_uint8_t door_sub_open_flag;//subordinate door

#ifdef __cplusplus
}
#endif

#endif
