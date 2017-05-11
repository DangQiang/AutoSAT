/*****************************************************
* File      : time.h
* Brief			:	��ʱ�����亯��ͷ�ļ�
* Date           Author       Version
* 2015-01-21     DangQiang    V1.0
******************************************************/
#ifndef __TIME_H
#define __TIME_H

/* Includes	*/
#include "rtthread.h"
#include "stm32f2xx.h"

struct rt_mymotor//������ƽṹ��
{
		rt_uint32_t count;
		rt_err_t  (*control) (rt_uint32_t cp_count,TIM_TypeDef* TIMx);
};

#define LEN_MYMOTOR sizeof(struct rt_mymotor)//�ṹ���С
typedef struct rt_mymotor *rt_mymotor_t; //�ṹ��ָ��
extern rt_mymotor_t TIM_Motor[8];//������ƽṹ������
/* Interface */
#define TIM_Use_3 (1<<3)//��ʱ��ʹ��
#define TIM_Use_4 (1<<4)//��ʱ��ʹ��
#define TIM_Use_5 (1<<5)//��ʱ��ʹ��
#define TIM_Use_6 (1<<6)//��ʱ��ʹ��
#define TIM_Use_7 (1<<7)//��ʱ��ʹ��

extern void TIM_Motor_Model_Init(rt_uint8_t flag);//��ʱ��ѡ����ģ���ʼ��
extern void Motor_Start(rt_uint32_t count,rt_uint16_t period,rt_err_t ( *program)(rt_uint32_t, TIM_TypeDef*));//�����ʱ������
extern void TIM_Motor_Enable(rt_uint8_t tim_num,rt_uint16_t period);//�����������
extern void TIM_Motor_Disable(TIM_TypeDef* TIMx);//�رն�ʱ��
#endif
