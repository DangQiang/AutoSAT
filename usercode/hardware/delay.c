/*****************************************************
* File			:	delay.c
* Brief			:	延时函数
* Date           Author       Version
* 2015-01-07     DangQiang    V1.0
******************************************************/
#include "delay.h"

/*******************************************************************************
* Function Name  : delay_1us
* Description    : 延时1us函数，由delay_us内部调用
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
* Description    : 延时us函数
* Input          : us：延时时间，单位us
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
    us = us * SysTick->LOAD/(1000000/RT_TICK_PER_SECOND); 			/*获取延时经过的TICK数*/

    delta = SysTick->VAL; 															/*获得当前时间*/

    do 
    { 
        if ( delta > SysTick->VAL ) 
            current_delay = delta - SysTick->VAL; 							
        if ( delta < SysTick->VAL ) 
            current_delay = SysTick->LOAD  - SysTick->VAL + delta; 	/*延时跨越了一次OS Tick的边界时的处理*/
    } while( current_delay < us ); 
	}
}

