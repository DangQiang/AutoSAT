/*****************************************************
* File      	:	process.c
* Brief				:	��λ�����̿��ƺ���ͷ�ļ�
* Date           Author       Version
* 2015-02-10     DangQiang    V1.0
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

/* Interface */	
void		initial_thread_entry(void* parameter);//ϵͳ��ʼ���߳�

rt_thread_t	Invoke_Parallel(void ( *program)(void* parameter),rt_uint8_t priority,rt_uint32_t* flag);//���е��ú���
void	Invoke_End_Wait(rt_uint32_t* flag);//�����Ӻ���ִ����ɵȴ�

#endif
