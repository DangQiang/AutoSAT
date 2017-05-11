/*****************************************************
* File			:	sys.c
* Brief			:	ϵͳ���弰������ʼ��c�ļ�
* Date           Author       Version
* 2015-01-28     DangQiang    V1.0
******************************************************/
#include "sys.h"

/*******************************************************************************
* Function Name  : ISP_Model_Init
* Description    : ISP���ܳ�ʼ��
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
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_8);//Ĭ�ϵ͵�ƽ,�����
}

/*******************************************************************************
* Function Name  : ISP_Handler
* Description    : ISP���ܴ�����
* Input          : null
*******************************************************************************/
void ISP_Handler(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_8);//�ߵ�ƽΪBOOT0���ŵ��ݽ��г��	
	rt_thread_delay(RT_TICK_PER_SECOND/10);
	NVIC_SystemReset();//ϵͳ��λ
}



