/*****************************************************
* File			:	shake.c
* Brief			:	SHAKE控制函数
* Date           Author       Version
* 2017-02-20     DangQiang    V1.0
******************************************************/
#include "shake.h"

rt_uint32_t Shake_Steps =	0;								//电机脉冲运动步数
rt_uint8_t 	Shake_Micro_Global = 1;					//SHAKE当前细分数
rt_uint16_t	Shake_Steps_Per_Round;					//SHAKE每圈脉冲数，根据细分计算
rt_uint16_t	Shake_Speed_Step;								//SHAKE 脉冲
rt_uint32_t Shake_Time_Global = 0;					//SHAKE振荡时间
rt_thread_t shake_stop_thread	= RT_NULL;		//SHAKE倒数计时线程

void Shake_Reset(void);//SHAKE驱动器复位函数
void shake_stop_thread_entry(void* parameter);//SHAKE倒数计时线程函数
rt_err_t Shake_Common_Handler(rt_uint32_t cp_count,TIM_TypeDef* TIMx);//SHAKE中断处理函数
/*******************************************************************************
* Function Name  : Shake_Pin_Init
* Description    : SHAKE引脚初始化
* Input          : null
*******************************************************************************/
void Shake_Pin_Init(void)
{
		GPIO_InitTypeDef  GPIO_InitStructure;
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);	 				
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;	
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);	 				
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;	
		GPIO_Init(GPIOC, &GPIO_InitStructure);		

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);	 				
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;	
		GPIO_Init(GPIOB, &GPIO_InitStructure);		
		
		Shake_Reset();	//SHAKE驱动器复位	
}
/*******************************************************************************
* Function Name  : Shake_End_Wait
* Description    : SHAKE等待结束函数
* Input          : null
*	Return				 : 1:结束
*******************************************************************************/
rt_uint8_t Shake_End_Wait(void)
{
		while(Shake_Steps)//结束判断
		{rt_thread_delay(1);}
		return 1;
}
/*******************************************************************************
* Function Name  : Shake_Micro_Selt
* Description    : SHAKE细分选择
* Input          : null
*******************************************************************************/
void Shake_Micro_Selt(rt_uint8_t	micro_step)
{
		switch(micro_step)
		{	
			case 1:	//1细分
					SHAKE_MODE2 = 0;		
					SHAKE_MODE1 = 0;	
					SHAKE_MODE0 = 0;
					Shake_Micro_Global = 1;
				break;
			
			case 2:	//2细分
					SHAKE_MODE2 = 0;		
					SHAKE_MODE1 = 0;	
					SHAKE_MODE0 = 1;
					Shake_Micro_Global = 2;			
				break;
						
			case 4:	//4细分
					SHAKE_MODE2 = 0;		
					SHAKE_MODE1 = 1;	
					SHAKE_MODE0 = 0;
					Shake_Micro_Global = 4;			
				break;
									
			case 8:	//8细分
					SHAKE_MODE2 = 0;		
					SHAKE_MODE1 = 1;	
					SHAKE_MODE0 = 1;
					Shake_Micro_Global = 8;			
				break;
												
			case 16:	//16细分
					SHAKE_MODE2 = 1;		
					SHAKE_MODE1 = 0;	
					SHAKE_MODE0 = 0;
					Shake_Micro_Global = 16;			
				break;
															
			case 32:	//32细分
					SHAKE_MODE2 = 1;		
					SHAKE_MODE1 = 0;	
					SHAKE_MODE0 = 1;
					Shake_Micro_Global = 32;			
				break;
																		
			default:	//默认1细分
					SHAKE_MODE2 = 0;		
					SHAKE_MODE1 = 0;	
					SHAKE_MODE0 = 0;
					Shake_Micro_Global = 1;				
				break;
		}
}
/*******************************************************************************
* Function Name  : Shake_Reset
* Description    : SHAKE复位函数
* Input          : null
*******************************************************************************/
void Shake_Reset(void)
{
		SHAKE_RESET = 0;//复位引脚低电平
		rt_thread_delay(1);	
		SHAKE_RESET = 1;//复位引脚高电平		
		SHAKE_DEA;			//复位后禁止使能
}
/*******************************************************************************
* Function Name  : Shake_Stop
* Description    : SHAKE停止函数
* Input          : null
*******************************************************************************/
void Shake_Stop(void)
{
		Shake_Steps=0;//SHAKE脉冲为0
		Shake_End_Wait();//等待电机其他动作完成	
}
/*******************************************************************************
* Function Name  : shake_Speed_Control
* Description    : SHAKE速度控制
* Input          : null
*	Return				 : 1:结束
*******************************************************************************/
rt_uint8_t Shake_Speed_Control(TIM_TypeDef* TIMx)
{
		if(!Shake_Steps)//判断运动结束,不进行速度控制
		return 0;
		
		if(TIMx->ARR>Shake_Speed_Step)
			TIMx->ARR--;

		return 1;
}
/*******************************************************************************
* Function Name  : Shake_Common_Handler
* Description    : SHAKE定时器处理函数公共部分
* Input          : null
*******************************************************************************/
rt_err_t Shake_Common_Handler(rt_uint32_t cp_count,TIM_TypeDef* TIMx)
{
		if(Shake_Steps==0) 
		{
			SHAKE_DEA;				//电机驱动使能禁止			
			TIM_Motor_Disable(TIMx);//电机停止
			return 0;
		}
		else if(Shake_Steps!=0)
		{	
			SHAKE_STEP = ~SHAKE_STEP;		//脉冲翻转
//		Shake_Steps--;							//脉冲数减1	
			Shake_Speed_Control(TIMx);	//加速控制
		}
		return 0;
}
/*******************************************************************************
* Function Name  : Shake_Start
* Description    : SHAKE开始振荡函数
* Input          : speed_rpm：振荡频率rpm,shake_time:震荡时间s,shake_direct:震动方向0默认顺时针，其他逆时针
*******************************************************************************/
void Shake_Start(rt_uint16_t	speed_rpm,rt_uint16_t	shake_time,rt_uint8_t shake_direct)
{
		rt_uint16_t	speed_start;//起始速度为计算定时器间隔的1.2倍

		if(shake_stop_thread != RT_NULL)
			return;
		
		SHAKE_ENA;			//电机驱动使能
		
		/*旋转方向控制*/
		if(shake_direct)
		{SHAKE_DIR_ANTICLOCKWISE;}//逆时针
		else
		{SHAKE_DIR_CLOCKWISE;}//顺时针
	
		/* 根据频率选择细分 */
		if(speed_rpm	>=	1300)
		{
			Shake_Micro_Selt(1);//选择细分数
		}
		else if(speed_rpm	>=	800)
		{
			Shake_Micro_Selt(2);//选择细分数
		}
		else	if(speed_rpm	>=	400)
		{
			Shake_Micro_Selt(4);//选择细分数
		}
		else	if(speed_rpm	>=	200)
		{
			Shake_Micro_Selt(8);//选择细分数
		}
		else	if(speed_rpm	>=	100)
		{
			Shake_Micro_Selt(16);//选择细分数
		}		
		else	if(speed_rpm	>=	50)
		{
			Shake_Micro_Selt(32);//选择细分数
		}	
		
		Shake_Steps_Per_Round = SHAKE_STEPS_ONE_ROUND * Shake_Micro_Global;//根据细分数，算出每圈脉冲数
		Shake_Speed_Step	=	(60*1000000)/(speed_rpm	*	Shake_Steps_Per_Round);//根据rpm求定时器脉冲间隔(60*1000000)/(rpm*Shake_Steps_Per_Round)
		speed_start	=	Shake_Speed_Step + (Shake_Speed_Step/5);//起始速度为1.2倍计算值
		
		Shake_Steps = 1;
		Motor_Start(Shake_Steps,speed_start,Shake_Common_Handler);	//开始振荡
		Shake_Time_Global = shake_time * RT_TICK_PER_SECOND;//赋值开始时间
		
		if (shake_stop_thread == RT_NULL)
		{
			shake_stop_thread = rt_thread_create("STOP",shake_stop_thread_entry, RT_NULL,256, 2, 2);   
			if (shake_stop_thread != RT_NULL) rt_thread_startup(shake_stop_thread);
		}
		
}
/*******************************************************************************
* Function Name  : shake_stop_thread_entry
* Description    : SHAKE倒数计时线程
* Input          : null
*******************************************************************************/
void shake_stop_thread_entry(void* parameter)
{
		Led_On(STA_LED);//使能STA LED灯
	
		while(--Shake_Time_Global)
		{
			rt_thread_delay(1);
		}//达到定时时间后停止
		Shake_Stop();
	
		SHAKE_DEA;				//电机驱动使能禁止
		Led_Off(STA_LED);	//关闭状态指示灯
		shake_stop_thread = RT_NULL;
}
