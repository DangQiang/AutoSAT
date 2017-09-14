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

}

/*******************************************************************************
* Function Name  : ISP_Handler
* Description    : ISP功能处理函数
* Input          : null
*******************************************************************************/
void ISP_Handler(void)
{
	NVIC_SystemReset();//系统复位
}



