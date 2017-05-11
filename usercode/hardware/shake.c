/*****************************************************
* File			:	shake.c
* Brief			:	SHAKE���ƺ���
* Date           Author       Version
* 2017-02-20     DangQiang    V1.0
******************************************************/
#include "shake.h"

rt_uint32_t Shake_Steps =	0;								//��������˶�����
rt_uint8_t 	Shake_Micro_Global = 1;					//SHAKE��ǰϸ����
rt_uint16_t	Shake_Steps_Per_Round;					//SHAKEÿȦ������������ϸ�ּ���
rt_uint16_t	Shake_Speed_Step;								//SHAKE ����
rt_uint32_t Shake_Time_Global = 0;					//SHAKE��ʱ��
rt_thread_t shake_stop_thread	= RT_NULL;		//SHAKE������ʱ�߳�

void Shake_Reset(void);//SHAKE��������λ����
void shake_stop_thread_entry(void* parameter);//SHAKE������ʱ�̺߳���
rt_err_t Shake_Common_Handler(rt_uint32_t cp_count,TIM_TypeDef* TIMx);//SHAKE�жϴ�����
/*******************************************************************************
* Function Name  : Shake_Pin_Init
* Description    : SHAKE���ų�ʼ��
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
		
		Shake_Reset();	//SHAKE��������λ	
}
/*******************************************************************************
* Function Name  : Shake_End_Wait
* Description    : SHAKE�ȴ���������
* Input          : null
*	Return				 : 1:����
*******************************************************************************/
rt_uint8_t Shake_End_Wait(void)
{
		while(Shake_Steps)//�����ж�
		{rt_thread_delay(1);}
		return 1;
}
/*******************************************************************************
* Function Name  : Shake_Micro_Selt
* Description    : SHAKEϸ��ѡ��
* Input          : null
*******************************************************************************/
void Shake_Micro_Selt(rt_uint8_t	micro_step)
{
		switch(micro_step)
		{	
			case 1:	//1ϸ��
					SHAKE_MODE2 = 0;		
					SHAKE_MODE1 = 0;	
					SHAKE_MODE0 = 0;
					Shake_Micro_Global = 1;
				break;
			
			case 2:	//2ϸ��
					SHAKE_MODE2 = 0;		
					SHAKE_MODE1 = 0;	
					SHAKE_MODE0 = 1;
					Shake_Micro_Global = 2;			
				break;
						
			case 4:	//4ϸ��
					SHAKE_MODE2 = 0;		
					SHAKE_MODE1 = 1;	
					SHAKE_MODE0 = 0;
					Shake_Micro_Global = 4;			
				break;
									
			case 8:	//8ϸ��
					SHAKE_MODE2 = 0;		
					SHAKE_MODE1 = 1;	
					SHAKE_MODE0 = 1;
					Shake_Micro_Global = 8;			
				break;
												
			case 16:	//16ϸ��
					SHAKE_MODE2 = 1;		
					SHAKE_MODE1 = 0;	
					SHAKE_MODE0 = 0;
					Shake_Micro_Global = 16;			
				break;
															
			case 32:	//32ϸ��
					SHAKE_MODE2 = 1;		
					SHAKE_MODE1 = 0;	
					SHAKE_MODE0 = 1;
					Shake_Micro_Global = 32;			
				break;
																		
			default:	//Ĭ��1ϸ��
					SHAKE_MODE2 = 0;		
					SHAKE_MODE1 = 0;	
					SHAKE_MODE0 = 0;
					Shake_Micro_Global = 1;				
				break;
		}
}
/*******************************************************************************
* Function Name  : Shake_Reset
* Description    : SHAKE��λ����
* Input          : null
*******************************************************************************/
void Shake_Reset(void)
{
		SHAKE_RESET = 0;//��λ���ŵ͵�ƽ
		rt_thread_delay(1);	
		SHAKE_RESET = 1;//��λ���Ÿߵ�ƽ		
		SHAKE_DEA;			//��λ���ֹʹ��
}
/*******************************************************************************
* Function Name  : Shake_Stop
* Description    : SHAKEֹͣ����
* Input          : null
*******************************************************************************/
void Shake_Stop(void)
{
		Shake_Steps=0;//SHAKE����Ϊ0
		Shake_End_Wait();//�ȴ���������������	
}
/*******************************************************************************
* Function Name  : shake_Speed_Control
* Description    : SHAKE�ٶȿ���
* Input          : null
*	Return				 : 1:����
*******************************************************************************/
rt_uint8_t Shake_Speed_Control(TIM_TypeDef* TIMx)
{
		if(!Shake_Steps)//�ж��˶�����,�������ٶȿ���
		return 0;
		
		if(TIMx->ARR>Shake_Speed_Step)
			TIMx->ARR--;

		return 1;
}
/*******************************************************************************
* Function Name  : Shake_Common_Handler
* Description    : SHAKE��ʱ����������������
* Input          : null
*******************************************************************************/
rt_err_t Shake_Common_Handler(rt_uint32_t cp_count,TIM_TypeDef* TIMx)
{
		if(Shake_Steps==0) 
		{
			SHAKE_DEA;				//�������ʹ�ܽ�ֹ			
			TIM_Motor_Disable(TIMx);//���ֹͣ
			return 0;
		}
		else if(Shake_Steps!=0)
		{	
			SHAKE_STEP = ~SHAKE_STEP;		//���巭ת
//		Shake_Steps--;							//��������1	
			Shake_Speed_Control(TIMx);	//���ٿ���
		}
		return 0;
}
/*******************************************************************************
* Function Name  : Shake_Start
* Description    : SHAKE��ʼ�񵴺���
* Input          : speed_rpm����Ƶ��rpm,shake_time:��ʱ��s,shake_direct:�𶯷���0Ĭ��˳ʱ�룬������ʱ��
*******************************************************************************/
void Shake_Start(rt_uint16_t	speed_rpm,rt_uint16_t	shake_time,rt_uint8_t shake_direct)
{
		rt_uint16_t	speed_start;//��ʼ�ٶ�Ϊ���㶨ʱ�������1.2��

		if(shake_stop_thread != RT_NULL)
			return;
		
		SHAKE_ENA;			//�������ʹ��
		
		/*��ת�������*/
		if(shake_direct)
		{SHAKE_DIR_ANTICLOCKWISE;}//��ʱ��
		else
		{SHAKE_DIR_CLOCKWISE;}//˳ʱ��
	
		/* ����Ƶ��ѡ��ϸ�� */
		if(speed_rpm	>=	1300)
		{
			Shake_Micro_Selt(1);//ѡ��ϸ����
		}
		else if(speed_rpm	>=	800)
		{
			Shake_Micro_Selt(2);//ѡ��ϸ����
		}
		else	if(speed_rpm	>=	400)
		{
			Shake_Micro_Selt(4);//ѡ��ϸ����
		}
		else	if(speed_rpm	>=	200)
		{
			Shake_Micro_Selt(8);//ѡ��ϸ����
		}
		else	if(speed_rpm	>=	100)
		{
			Shake_Micro_Selt(16);//ѡ��ϸ����
		}		
		else	if(speed_rpm	>=	50)
		{
			Shake_Micro_Selt(32);//ѡ��ϸ����
		}	
		
		Shake_Steps_Per_Round = SHAKE_STEPS_ONE_ROUND * Shake_Micro_Global;//����ϸ���������ÿȦ������
		Shake_Speed_Step	=	(60*1000000)/(speed_rpm	*	Shake_Steps_Per_Round);//����rpm��ʱ��������(60*1000000)/(rpm*Shake_Steps_Per_Round)
		speed_start	=	Shake_Speed_Step + (Shake_Speed_Step/5);//��ʼ�ٶ�Ϊ1.2������ֵ
		
		Shake_Steps = 1;
		Motor_Start(Shake_Steps,speed_start,Shake_Common_Handler);	//��ʼ��
		Shake_Time_Global = shake_time * RT_TICK_PER_SECOND;//��ֵ��ʼʱ��
		
		if (shake_stop_thread == RT_NULL)
		{
			shake_stop_thread = rt_thread_create("STOP",shake_stop_thread_entry, RT_NULL,256, 2, 2);   
			if (shake_stop_thread != RT_NULL) rt_thread_startup(shake_stop_thread);
		}
		
}
/*******************************************************************************
* Function Name  : shake_stop_thread_entry
* Description    : SHAKE������ʱ�߳�
* Input          : null
*******************************************************************************/
void shake_stop_thread_entry(void* parameter)
{
		Led_On(STA_LED);//ʹ��STA LED��
	
		while(--Shake_Time_Global)
		{
			rt_thread_delay(1);
		}//�ﵽ��ʱʱ���ֹͣ
		Shake_Stop();
	
		SHAKE_DEA;				//�������ʹ�ܽ�ֹ
		Led_Off(STA_LED);	//�ر�״ָ̬ʾ��
		shake_stop_thread = RT_NULL;
}
