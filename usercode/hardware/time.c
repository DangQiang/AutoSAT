/*****************************************************
* File			:	time.c
* Brief			:	��ʱ�����亯��
* Date           Author       Version
* 2015-01-21     DangQiang    V1.0
******************************************************/
#include "time.h"

rt_uint8_t TIM_Busy_Flag=0;//��ʱ�����б�־λ
rt_mymotor_t TIM_Motor[8];//������ƽṹ������

rt_uint8_t TIM_Use_Flag;//��ǰʹ�õĶ�ʱ��
rt_uint8_t TIM_Use_Firt;//ʹ�ö�ʱ�����׸�
rt_uint8_t TIM_Use_Last;//ʹ�ö�ʱ����ĩ��
rt_uint8_t TIM_Use_Amnt;//ʹ�ö�ʱ���ĸ���

/*******************************************************************************
* Function Name  : TIM_NVIC_Motor_Init
* Description    : ������ƶ�ʱ����ʼ��
* Input          : tim_num:��ʱ�����(3~7)
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
		case 3:	//��ʱ��3�ж���������
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
			NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
			NVIC_Init(&NVIC_InitStructure);				
		break;
		
		case 4:	//��ʱ��4�ж���������
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
			NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
			NVIC_Init(&NVIC_InitStructure);					
		break;
		
		case 5:	//��ʱ��5�ж���������
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
			NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
			NVIC_Init(&NVIC_InitStructure);					
		break;
		
		case 6:	//��ʱ��6�ж���������
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
			NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
			NVIC_Init(&NVIC_InitStructure);					
		break;
		
		case 7:	//��ʱ��7�ж���������
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
			NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
			NVIC_Init(&NVIC_InitStructure);					
		break;
		
		default:
		break;
	}
		
	TIMx=(TIM_TypeDef *)(TIM2_BASE+(0x0400*(tim_num-2)));//��ʱ�����ֳ�ʼ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2<<(tim_num-2),ENABLE);
	
	TIM_DeInit(TIMx);
	TIM_TimeBaseStructure.TIM_Period=0;					/*����*/
	TIM_TimeBaseStructure.TIM_Prescaler= (60 - 1);	/* 1us */
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIMx, TIM_FLAG_Update);					
	TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIMx, DISABLE);	//�رն�ʱ��
}
/*******************************************************************************
* Function Name  : TIM_Motor_Enable
* Description    : ʹ��ѡ��Ķ�ʱ��.
* Input          : tim_num:��ʱ�����(3~7),period����ʱ����
*******************************************************************************/
void TIM_Motor_Enable(rt_uint8_t tim_num,rt_uint16_t period)
{
	TIM_TypeDef* TIMx; 
	TIM_Busy_Flag |= (1<<tim_num);  
	TIMx=(TIM_TypeDef *)(TIM2_BASE+(0x0400*(tim_num-2)));//ѡ��ʱ��
	TIMx->ARR	=	period;
	TIM_Cmd(TIMx, ENABLE);									 
}
/*******************************************************************************
* Function Name  : TIM_Motor_Disable
* Description    : ֹͣѡ��Ķ�ʱ��.
* Input          : TIMx:��ʱ��(3~7).
*******************************************************************************/
void TIM_Motor_Disable(TIM_TypeDef* TIMx)
{
	rt_uint8_t timnum;//��ʱ�����
	TIM_Cmd(TIMx, DISABLE);	
	timnum=((uint32_t)TIMx-TIM2_BASE)/0x0400+2;//���㶨ʱ���ı��
	TIM_Busy_Flag &= ~(1<<timnum); //��ʱ��ռ�ñ�־λ���
}
/*******************************************************************************
* Function Name  : Motor_Start
* Description    : ���������ʱ�����亯��
* Input          : program:���������ں���.period����ʱ�����ڣ�count:������������ַ��
*******************************************************************************/
void Motor_Start(rt_uint32_t count,rt_uint16_t period,rt_err_t ( *program)(rt_uint32_t, TIM_TypeDef*))
{
	rt_uint8_t	i;//i������
	
	if(count==0)//�ڵ����������Чʱ��ѡ��ʱ��
	{return;}
	
	while(1)
	{
		rt_enter_critical();//�߳���
		for(i=TIM_Use_Firt;i<=TIM_Use_Last;i++)
		{
			if((1<<i)&TIM_Use_Flag)
			{
				if(!((1<<i)&TIM_Busy_Flag))//����
				{
					TIM_Motor[i]->control = program;
					TIM_Motor[i]->count = count;
					TIM_Motor_Enable(i,period);
					rt_exit_critical();//�˳��߳���
					return;
				}
			}			
		}
		rt_exit_critical();//�˳��߳���
		rt_thread_delay(2);//��ʱ2tick
	}

}
/*******************************************************************************
* Function Name  : TIM_Motor_Model_Init
* Description    : ������ƶ�ʱ��ģ���ʼ��
* Input          : tim_num:��ʱ�����(3~7),period����ʱ����
*******************************************************************************/
void TIM_Motor_Model_Init(rt_uint8_t flag)
{
	rt_uint8_t	i;
	
	TIM_Use_Flag	=	flag;
	
	/*�����õ��Ķ�ʱ�����׸���ĩ����������*/
	for(i=0;i<8;i++)
	{
		if((TIM_Use_Flag>>i)&1)
		{
			if(!TIM_Use_Firt)
			{TIM_Use_Firt=i;}			//�׸�����
			TIM_Use_Amnt++;				//����
			TIM_NVIC_Motor_Init(i);			/*��ʱ���жϳ�ʼ��*/
			TIM_Motor[i]=(struct rt_mymotor *)rt_malloc(LEN_MYMOTOR);//�����ڴ�
		}
		if((TIM_Use_Flag<<i)&(1<<7))
		{
			if(!TIM_Use_Last)			//ĩ������
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

