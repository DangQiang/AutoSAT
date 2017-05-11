/*****************************************************
* File      	:	can1.h
* Brief				:	SHAKE模块主控制函数
* Date           Author       Version
* 2017-02-21     DangQiang    V1.0
******************************************************/
#ifndef __CAN1_H
#define __CAN1_H
 
/* Includes	*/
#include <rtthread.h>
#include "stm32f2xx.h"
#include "process.h"
#include "shake.h"

void can1_thread_entry(void* parameter);//CAN1控制线程

#endif /* __CAN1_H */

/************************************END OF FILE********************************/
