/*****************************************************
* File      	:	process.c
* Brief				:	��λ�����̿��ƺ���ͷ�ļ�
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
void	initial_thread_entry(void* parameter);//ϵͳ��ʼ���߳�

extern	rt_uint8_t	sht2x_enable_flag[3];//sht2x�������ɼ�ʹ�ܱ�־λ
extern	rt_uint8_t	sht2x_exist_flag[3];//sht2x���������ڱ�־λ
extern	rt_uint8_t	sht2x_collect_time[3];//sht2x�������ɼ����

rt_thread_t	Invoke_Parallel(void ( *program)(void* parameter),rt_uint8_t priority,rt_uint32_t* flag);//���е��ú���
void	Invoke_End_Wait(rt_uint32_t* flag);//�����Ӻ���ִ����ɵȴ�

#endif
