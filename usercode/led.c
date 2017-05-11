/*****************************************************
* File      	:	led.c
* Brief				:	led�ƿ��ƺ���
* Date           Author       Version
* 2015-03-17     DangQiang    V1.0
******************************************************/
#include "led.h"

/*******************************************************************************
* Function Name  : led_thread_entry
* Description    : led_�߳�
* Input          : null
*******************************************************************************/
void led_thread_entry(void* parameter)
{
		Led_Model_Init();//Ledģ���ʼ��
		
		for (;;)
		{
			Led_On(ERR_LED);			
			rt_thread_delay( 2 ); 
			Led_Off(ERR_LED);				
			rt_thread_delay( 98 );//T1+T2=RT_TICK_PER_SECOND
		}
}
/*******************************************************************************
* Function Name  : Led_Model_Init
* Description    : Ledģ���ʼ��
* Input          : null
*******************************************************************************/
void Led_Model_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(led1_rcc,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = led1_pin | led2_pin;	
	GPIO_Init(led1_gpio, &GPIO_InitStructure);	
	
	Led_Off(ERR_LED);//�ر�LED
	Led_Off(STA_LED);
}
/*******************************************************************************
* Function Name  : Led_Off
* Description    : Led�ƹر�
* Input          : null
*******************************************************************************/
void Led_Off(rt_uint32_t n)
{
    switch (n)
    {
    case STA_LED:
        GPIO_SetBits(led1_gpio, led1_pin);
        break;
    case ERR_LED:
        GPIO_SetBits(led2_gpio, led2_pin);
        break;
    default:
        break;
    }
}
/*******************************************************************************
* Function Name  : Led_On
* Description    : Led�ƿ���
* Input          : null
*******************************************************************************/
void Led_On(rt_uint32_t n)
{
    switch (n)
    {
    case STA_LED:
        GPIO_ResetBits(led1_gpio, led1_pin);
        break;
    case ERR_LED:
        GPIO_ResetBits(led2_gpio, led2_pin);
        break;
    default:
        break;
    }
}

