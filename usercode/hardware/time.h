/*****************************************************
* File      : time.h
* Brief			:	定时器分配函数头文件
* Date           Author       Version
* 2015-01-21     DangQiang    V1.0
******************************************************/
#ifndef __TIME_H
#define __TIME_H

/* Includes	*/
#include "rtthread.h"
#include "stm32f2xx.h"

struct rt_mymotor//电机控制结构体
{
		rt_uint32_t count;
		rt_err_t  (*control) (rt_uint32_t cp_count,TIM_TypeDef* TIMx);
};

#define LEN_MYMOTOR sizeof(struct rt_mymotor)//结构体大小
typedef struct rt_mymotor *rt_mymotor_t; //结构体指针
extern rt_mymotor_t TIM_Motor[8];//电机控制结构体数组
/* Interface */
#define TIM_Use_3 (1<<3)//定时器使用
#define TIM_Use_4 (1<<4)//定时器使用
#define TIM_Use_5 (1<<5)//定时器使用
#define TIM_Use_6 (1<<6)//定时器使用
#define TIM_Use_7 (1<<7)//定时器使用

extern void TIM_Motor_Model_Init(rt_uint8_t flag);//定时器选择功能模块初始化
extern void Motor_Start(rt_uint32_t count,rt_uint16_t period,rt_err_t ( *program)(rt_uint32_t, TIM_TypeDef*));//电机定时器启动
extern void TIM_Motor_Enable(rt_uint8_t tim_num,rt_uint16_t period);//电机启动函数
extern void TIM_Motor_Disable(TIM_TypeDef* TIMx);//关闭定时器
#endif
