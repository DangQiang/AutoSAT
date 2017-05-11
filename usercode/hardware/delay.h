/*****************************************************
* File			:	delay.h
* Brief			:	延时函数头文件
* Date           Author       Version
* 2015-01-07     DangQiang    V1.0
******************************************************/
#ifndef __DELAY_H
#define __DELAY_H

/* Includes	*/ 
#include <rtthread.h>
#include <stm32f2xx.h>
#include <core_cm3.h> 

/* Interface */	 
void delay_us(rt_uint32_t us);//用于10ms以内的延时

#endif /* __DELAY_H */

/*********************END OF FILE*********************/























