/*****************************************************
* File      	:	led.h
* Brief				:	led灯控制函数头文件
* Date           Author       Version
* 2015-03-17     DangQiang    V1.0
******************************************************/
#ifndef __LED_H__
#define __LED_H__

/* Includes	*/
#include <rtthread.h>
#include <stm32f2xx.h>

/* Pins */
#define led1_rcc       RCC_AHB1Periph_GPIOE
#define led1_gpio      GPIOE
#define led1_pin       (GPIO_Pin_4)

#define led2_rcc       RCC_AHB1Periph_GPIOE
#define led2_gpio      GPIOE
#define led2_pin       (GPIO_Pin_3)

/* Interface */
#define STA_LED	1
#define ERR_LED	2

void Led_Model_Init(void);
void Led_On(rt_uint32_t n);
void Led_Off(rt_uint32_t n);
void led_thread_entry(void* parameter);

#endif
