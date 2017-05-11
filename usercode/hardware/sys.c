/*****************************************************
* File			:	sys.c
* Brief			:	系统定义及公共初始化c文件
* Date           Author       Version
* 2015-01-28     DangQiang    V1.0
******************************************************/
#include "sys.h"

/*******************************************************************************
* Function Name  : ISP_Model_Init
* Description    : ISP功能初始化
* Input          : null
*******************************************************************************/
void ISP_Model_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//PB8
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_8);//默认低电平,不充电
}

/*******************************************************************************
* Function Name  : ISP_Handler
* Description    : ISP功能处理函数
* Input          : null
*******************************************************************************/
void ISP_Handler(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_8);//高电平为BOOT0引脚电容进行充电	
	rt_thread_delay(RT_TICK_PER_SECOND/10);
	NVIC_SystemReset();//系统复位
}



