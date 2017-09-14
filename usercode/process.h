/*****************************************************
* File      	:	process.c
* Brief				:	下位机流程控制函数头文件
* Date           Author       Version
* 2017-05-25     DangQiang    V1.0
******************************************************/
#ifndef __PROCESS_H__
#define __PROCESS_H__

/* Includes	*/
#include <rtthread.h>
#include <stm32f2xx.h>
#include "led.h"
#include "eeprom.h"
#include "time.h"
#include "can1.h"
#include "shell_io.h"
#include "sht2x_a.h"
#include "sht2x_b.h"
#include "sht2x_c.h"

/* Interface */	
void	initial_thread_entry(void* parameter);//系统初始化线程

extern	rt_uint8_t	sht2x_enable_flag[3];//sht2x传感器采集使能标志位
extern	rt_uint8_t	sht2x_exist_flag[3];//sht2x传感器存在标志位
extern	rt_uint8_t	sht2x_collect_time[3];//sht2x传感器采集间隔

rt_thread_t	Invoke_Parallel(void ( *program)(void* parameter),rt_uint8_t priority,rt_uint32_t* flag);//并行调用函数
void	Invoke_End_Wait(rt_uint32_t* flag);//并行子函数执行完成等待

#endif
