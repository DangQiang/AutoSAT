/*****************************************************
* File			:	shake.h
* Brief			:	SHAKE���ƺ���ͷ�ļ�
* Date           Author       Version
* 2017-02-20     DangQiang    V1.0
******************************************************/
#ifndef __SHAKE_H
#define __SHAKE_H
	 
/* Includes	*/
#include <rtthread.h>
#include "stm32f2xx.h"
#include "sys.h"
#include "time.h"
#include "led.h"

/**���Ŷ���****/
#define	SHAKE_MODE0	PAout(6)		//MODE0-->PA.6
#define	SHAKE_MODE1	PAout(5)		//MODE1-->PA.5
#define	SHAKE_MODE2	PAout(4)		//MODE2-->PA.4

#define	SHAKE_STEP	PAout(7)		//STEP-->PA.7
#define	SHAKE_ENBL	PCout(4)		//ENBL-->PC.4
#define	SHAKE_DIR		PCout(5)		//DIR -->PC.5

#define	SHAKE_RESET	PBout(0)		//RESET-->PB0

#define	SHAKE_ENA		SHAKE_ENBL=0;	/*SHAKE-ena*/
#define	SHAKE_DEA		SHAKE_ENBL=1;	/*SHAKE-dena*/

#define SHAKE_DIR_CLOCKWISE			SHAKE_DIR=0; //˳ʱ��
#define SHAKE_DIR_ANTICLOCKWISE	SHAKE_DIR=1; //��ʱ��
/**********************************************************/
//#define	SHAKE_DIR_NZERO SHAKE_DIR=1;
//#define	SHAKE_DIR_ZERO 	SHAKE_DIR=0;

#define	SHAKE_STEPS_ONE_ROUND	8		//SHAKEÿȦ��������8 = 4*2(2�ζ�ʱ����һ������)


/***�����ӿ�****/
extern rt_uint32_t Shake_Time_Global;					//SHAKE��ʱ��Shake_Time_Global
extern void Shake_Pin_Init(void);	//SHAKE������ų�ʼ��
extern void Shake_Stop(void);			//SHAKEֹͣ����
extern void Shake_Start(rt_uint16_t	speed_rpm,rt_uint16_t	shake_time,rt_uint8_t shake_direct);//SHAKE��ʼ��������
#endif /* __shake_H */

/************************************END OF FILE********************************/

