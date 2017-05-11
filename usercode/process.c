/*****************************************************
* File      	:	process.c
* Brief				:	下位机流程控制函数
* Date           Author       Version
* 2015-03-17     DangQiang    V1.0
******************************************************/
#include "process.h"

/*******************************************************************************
* Function Name  : initial_thread_entry
* Description    : 系统初始化线程，公共功能的初始化
* Input          : null
*******************************************************************************/
void initial_thread_entry(void* parameter)
{
		rt_thread_t init_thread;
	
		/**********初始化公共功能***********/
		TIM_Motor_Model_Init(TIM_Use_3);//电机控制定时器的初始化
//		AT24C02_Init();			//EEPROM初始化
//		ISP_Model_Init();			//ISP功能初始化

		/**********初始化独立线程**********/
		//LED灯显示线程
		init_thread = rt_thread_create("LED",led_thread_entry, RT_NULL,256, 2, 2);   
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);

	  //CAN1协议处理线程 1	 
		init_thread = rt_thread_create("CAN1",can1_thread_entry, RT_NULL,512, 1, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);	
}
/*******************************************************************************
* Function Name  : Pause_Handler
* Description    : 系统暂停处理函数
* Input          : null
*******************************************************************************/
void Pause_Handler(void)
{
		rt_enter_critical();//调度器锁					
		//计量泵处理
			
		rt_exit_critical();//调度器锁								
}
/*******************************************************************************
* Function Name  : Pause_Cancel
* Description    : 暂停恢复函数
* Input          : null
*******************************************************************************/
void Pause_Cancel(void)
{				
		//泵处理
}

/*******************************************************************************
* Function Name  : Invoke_Parallel
* Description    : 并行调用函数
* Input          : program:函数入口；priority:优先级；flag：完成标志位
*******************************************************************************/
rt_thread_t Invoke_Parallel(void ( *program)(void* parameter),rt_uint8_t priority,rt_uint32_t* flag)
{
		rt_thread_t init_thread;
		
		init_thread = rt_thread_create("test1",program, flag, 256, priority, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);
		return init_thread;
}
/*******************************************************************************
* Function Name  : Invoke_End_Wait
* Description    : 并行调用等待完成函数
* Input          : flag：完成标志位
*******************************************************************************/
void Invoke_End_Wait(rt_uint32_t* flag)
{
		rt_uint32_t *finish_flag=(rt_uint32_t*)flag;
		
		while(!(*finish_flag))//查询完成标志位
		{rt_thread_delay(1);}//延时

		*finish_flag=0;//清除完成标志位
}
