/*****************************************************
* File			:	time.c
* Brief			:	定时器分配函数
* Date           Author       Version
* 2015-01-21     DangQiang    V1.0
******************************************************/
#include "time.h"

rt_uint8_t TIM_Busy_Flag=0;//定时器空闲标志位
rt_mymotor_t TIM_Motor[8];//电机控制结构体数组

rt_uint8_t TIM_Use_Flag;//当前使用的定时器
rt_uint8_t TIM_Use_Firt;//使用定时器的首个
rt_uint8_t TIM_Use_Last;//使用定时器的末个
rt_uint8_t TIM_Use_Amnt;//使用定时器的个数

/*******************************************************************************
* Function Name  : TIM_NVIC_Motor_Init
* Description    : 电机控制定时器初始化
* Input          : tim_num:定时器编号(3~7)
*******************************************************************************/
void TIM_NVIC_Motor_Init(rt_uint8_t tim_num)
{
	TIM_TypeDef* 	TIMx;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  												  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	switch(tim_num)
	{		
		case 3:	//定时器3中断向量配置
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
			NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
			NVIC_Init(&NVIC_InitStructure);				
		break;
		
		case 4:	//定时器4中断向量配置
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
			NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
			NVIC_Init(&NVIC_InitStructure);					
		break;
		
		case 5:	//定时器5中断向量配置
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
			NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
			NVIC_Init(&NVIC_InitStructure);					
		break;
		
		case 6:	//定时器6中断向量配置
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
			NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
			NVIC_Init(&NVIC_InitStructure);					
		break;
		
		case 7:	//定时器7中断向量配置
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
			NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
			NVIC_Init(&NVIC_InitStructure);					
		break;
		
		default:
		break;
	}
		
	TIMx=(TIM_TypeDef *)(TIM2_BASE+(0x0400*(tim_num-2)));//定时器部分初始化
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2<<(tim_num-2),ENABLE);
	
	TIM_DeInit(TIMx);
	TIM_TimeBaseStructure.TIM_Period=0;					/*周期*/
	TIM_TimeBaseStructure.TIM_Prescaler= (60 - 1);	/* 1us */
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIMx, TIM_FLAG_Update);					
	TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIMx, DISABLE);	//关闭定时器
}
/*******************************************************************************
* Function Name  : TIM_Motor_Enable
* Description    : 使能选择的定时器.
* Input          : tim_num:定时器编号(3~7),period：定时周期
*******************************************************************************/
void TIM_Motor_Enable(rt_uint8_t tim_num,rt_uint16_t period)
{
	TIM_TypeDef* TIMx; 
	TIM_Busy_Flag |= (1<<tim_num);  
	TIMx=(TIM_TypeDef *)(TIM2_BASE+(0x0400*(tim_num-2)));//选择定时器
	TIMx->ARR	=	period;
	TIM_Cmd(TIMx, ENABLE);									 
}
/*******************************************************************************
* Function Name  : TIM_Motor_Disable
* Description    : 停止选择的定时器.
* Input          : TIMx:定时器(3~7).
*******************************************************************************/
void TIM_Motor_Disable(TIM_TypeDef* TIMx)
{
	rt_uint8_t timnum;//定时器编号
	TIM_Cmd(TIMx, DISABLE);	
	timnum=((uint32_t)TIMx-TIM2_BASE)/0x0400+2;//计算定时器的编号
	TIM_Busy_Flag &= ~(1<<timnum); //定时器占用标志位清除
}
/*******************************************************************************
* Function Name  : Motor_Start
* Description    : 电机启动定时器分配函数
* Input          : program:电机控制入口函数.period：定时器周期，count:脉冲数变量地址。
*******************************************************************************/
void Motor_Start(rt_uint32_t count,rt_uint16_t period,rt_err_t ( *program)(rt_uint32_t, TIM_TypeDef*))
{
	rt_uint8_t	i;//i：计数
	
	if(count==0)//在电机脉冲数无效时不选择定时器
	{return;}
	
	while(1)
	{
		rt_enter_critical();//线程锁
		for(i=TIM_Use_Firt;i<=TIM_Use_Last;i++)
		{
			if((1<<i)&TIM_Use_Flag)
			{
				if(!((1<<i)&TIM_Busy_Flag))//空闲
				{
					TIM_Motor[i]->control = program;
					TIM_Motor[i]->count = count;
					TIM_Motor_Enable(i,period);
					rt_exit_critical();//退出线程锁
					return;
				}
			}			
		}
		rt_exit_critical();//退出线程锁
		rt_thread_delay(2);//延时2tick
	}

}
/*******************************************************************************
* Function Name  : TIM_Motor_Model_Init
* Description    : 电机控制定时器模块初始化
* Input          : tim_num:定时器编号(3~7),period：定时周期
*******************************************************************************/
void TIM_Motor_Model_Init(rt_uint8_t flag)
{
	rt_uint8_t	i;
	
	TIM_Use_Flag	=	flag;
	
	/*计算用到的定时器的首个，末个，和总数*/
	for(i=0;i<8;i++)
	{
		if((TIM_Use_Flag>>i)&1)
		{
			if(!TIM_Use_Firt)
			{TIM_Use_Firt=i;}			//首个计算
			TIM_Use_Amnt++;				//计数
			TIM_NVIC_Motor_Init(i);			/*定时器中断初始化*/
			TIM_Motor[i]=(struct rt_mymotor *)rt_malloc(LEN_MYMOTOR);//申请内存
		}
		if((TIM_Use_Flag<<i)&(1<<7))
		{
			if(!TIM_Use_Last)			//末个计算
			{TIM_Use_Last=(7-i);}			
		}
	}
}
/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 Handler.
*******************************************************************************/
void TIM3_IRQHandler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();
   
	if ( TIM_GetITStatus(TIM3 , TIM_IT_Update) != RESET ) 
	{
		TIM_ClearITPendingBit(TIM3 , TIM_FLAG_Update);
		TIM_Motor[3]->control(TIM_Motor[3]->count,TIM3);
   }	 
	/* leave interrupt */
	rt_interrupt_leave();
}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 Handler.
*******************************************************************************/
void TIM4_IRQHandler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();
	
   if ( TIM_GetITStatus(TIM4 , TIM_IT_Update) != RESET ) 
	{
		TIM_ClearITPendingBit(TIM4 , TIM_FLAG_Update);
		TIM_Motor[4]->control(TIM_Motor[4]->count,TIM4);   
	}
    
	/* leave interrupt */
	rt_interrupt_leave();
}
/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : This function handles TIM5 Handler.
*******************************************************************************/
void TIM5_IRQHandler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();
	
   if ( TIM_GetITStatus(TIM5 , TIM_IT_Update) != RESET ) 
	{
		TIM_ClearITPendingBit(TIM5 , TIM_FLAG_Update);
		TIM_Motor[5]->control(TIM_Motor[5]->count,TIM5);   
  }
    
	/* leave interrupt */
	rt_interrupt_leave();
}

/*******************************************************************************
* Function Name  : TIM6_DAC_IRQHandler
* Description    : This function handles TIM6 Handler.
*******************************************************************************/
void TIM6_DAC_IRQHandler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();
	
   if ( TIM_GetITStatus(TIM6 , TIM_IT_Update) != RESET ) 
	{
		TIM_ClearITPendingBit(TIM6 , TIM_FLAG_Update);
		TIM_Motor[6]->control(TIM_Motor[6]->count,TIM6);   
	}
    
	/* leave interrupt */
	rt_interrupt_leave();
}

/*******************************************************************************
* Function Name  : TIM7_IRQHandler
* Description    : This function handles TIM7 Handler.
*******************************************************************************/
void TIM7_IRQHandler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();
	
   if ( TIM_GetITStatus(TIM7 , TIM_IT_Update) != RESET ) 
	{
		TIM_ClearITPendingBit(TIM7 , TIM_FLAG_Update);
		TIM_Motor[7]->control(TIM_Motor[7]->count,TIM7);   
	}
    
	/* leave interrupt */
	rt_interrupt_leave();
}

