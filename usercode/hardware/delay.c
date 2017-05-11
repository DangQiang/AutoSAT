/*****************************************************
* File			:	delay.c
* Brief			:	��ʱ����
* Date           Author       Version
* 2015-01-07     DangQiang    V1.0
******************************************************/
#include "delay.h"

/*******************************************************************************
* Function Name  : delay_1us
* Description    : ��ʱ1us��������delay_us�ڲ�����
* Input          : null
* Retrun         : null
*******************************************************************************/
void delay_1us(void) 
{ 
		rt_uint32_t i=4;
		while(i--);
}
/*******************************************************************************
* Function Name  : delay_us
* Description    : ��ʱus����
* Input          : us����ʱʱ�䣬��λus
* Retrun         : null
*******************************************************************************/
void delay_us(rt_uint32_t us) 
{ 
    rt_uint32_t delta; 
    rt_uint32_t current_delay; 
		if(us==1)
		{
			delay_1us();
		}
		else {
		us=us-1;
    us = us * SysTick->LOAD/(1000000/RT_TICK_PER_SECOND); 			/*��ȡ��ʱ������TICK��*/

    delta = SysTick->VAL; 															/*��õ�ǰʱ��*/

    do 
    { 
        if ( delta > SysTick->VAL ) 
            current_delay = delta - SysTick->VAL; 							
        if ( delta < SysTick->VAL ) 
            current_delay = SysTick->LOAD  - SysTick->VAL + delta; 	/*��ʱ��Խ��һ��OS Tick�ı߽�ʱ�Ĵ���*/
    } while( current_delay < us ); 
	}
}

