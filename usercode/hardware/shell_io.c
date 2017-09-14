/*****************************************************
* File			:	shell_io.c
* Brief			:	shell_io����ģ��
* Date           Author       Version
* 2017-05-25     DangQiang    V1.0
******************************************************/
#include "shell_io.h"

rt_uint8_t Lamp_Flag[4]={0,0,0,0};//lamp״̬,0��ʾ��1��ʾ����
rt_uint8_t Lock_Flag[3]={0,0,0};	//lock1,lock2,stop״̬

rt_mailbox_t		mb_lock1_rece_data;	//LOCK1������Ч����
rt_mailbox_t		mb_lock2_rece_data;	//LOCK2������Ч����

//rt_uint8_t door_main_open_flag;	//main door
//rt_uint8_t door_sub_open_flag;	//subordinate door
rt_uint8_t	g_error_flag;


void lock_thread_entry(void* parameter);//����״̬����߳�
void error_thread_entry(void* parameter);//����ָʾ�߳�
void door1_thread_entry(void* parameter);//��1�����߳�
void door2_thread_entry(void* parameter);//��2�����߳�

/*******************************************************************************
* Function Name  : Shell_Io_Init
* Description    : Shell_Io_��ʼ��
* Input          : null
*******************************************************************************/
void Shell_Io_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		rt_thread_t init_thread;
		
		//���
		//PD3��4��5��6��7����Ƿ��ȣ������ƣ���1����2��������
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;	
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		//���	
		//PB3��4��5��6��LED�ܵ�Դ����ɫLED����ɫLED����ɫLED
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;	
		GPIO_Init(GPIOB, &GPIO_InitStructure);	

		//����
		//PA4��6��7����ͣ����2����1
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7;	
		GPIO_Init(GPIOA, &GPIO_InitStructure);	

		FAN_SHELL_OPEN;//�������ȿ�
		LED_LIGHT_CLOSE;//LED�����ƹ�
		LOCK1_POWER_OFF;//��1��Դ��
		LOCK2_POWER_OFF;//��2��Դ��
		BUZZ_CLOSE;//�������ر�
		LED_STATE_CLOSE;//�ر��ܵ�Դ			
		LED_RED_CLOSE;//�رպ��
		LED_YEL_CLOSE;//�رջƵ�
		LED_GRN_CLOSE;//�ر��̵�	
				
		init_thread = rt_thread_create("DOOR1",door1_thread_entry, RT_NULL, 256, 2, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);	

		init_thread = rt_thread_create("DOOR2",door2_thread_entry, RT_NULL, 256, 2, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);
		
		init_thread = rt_thread_create("LOCK",lock_thread_entry, RT_NULL, 256, 2, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);		
		
		init_thread = rt_thread_create("ERROR",error_thread_entry, RT_NULL, 256, 2, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);	

		Lamp_Handler(LampID_GRN,1);	//��������״̬�̵�
		Lamp_Handler(LampID_LIG,0);	//����Ĭ�Ϲر�������
}
/*******************************************************************************
* Function Name  : Lamp_Handler
* Description    : �ƴ�����
* Input          : type:���ͣ�cmd������,1��,0��
*******************************************************************************/
void Lamp_Handler(rt_uint8_t type,rt_uint8_t cmd)
{
	if(type == LampID_LIG)//������
	{
		if(cmd)//��
			{LED_LIGHT_OPEN;}
		else//��
			{LED_LIGHT_CLOSE;}
		Lamp_Flag[LampID_LIG] = cmd;//����״̬		
	}
	if(type == LampID_GRN)//�̵�
	{
		if(cmd)//��
		{	
			g_error_flag = 0;	
			LED_RED_CLOSE;//�رպ��
			LED_YEL_CLOSE;//�رջƵ�
			LED_GRN_CLOSE;//�ر��̵�	
			Lamp_Flag[LampID_YEL] = 0;//����״̬	
			Lamp_Flag[LampID_RED] = 0;//����״̬				
			LED_STATE_OPEN;//�����ܵ�Դ
			LED_GRN_OPEN;//�̵ƿ���
			
//			BUZZ_OPEN;//����������
//			rt_thread_delay(RT_TICK_PER_SECOND/5); //��0.2s		
//			BUZZ_CLOSE;//�������ر�			
		}
		else
			{LED_GRN_CLOSE;}//�̵ƹر�		
		Lamp_Flag[LampID_GRN] = cmd;//����״̬				
	}
	else if(type == LampID_YEL)//�Ƶ�
	{
		if(cmd)//��
		{
			g_error_flag = 0;			
			LED_RED_CLOSE;//�رպ��
			LED_YEL_CLOSE;//�رջƵ�
			LED_GRN_CLOSE;//�ر��̵�	
			Lamp_Flag[LampID_GRN] = 0;//����״̬	
			Lamp_Flag[LampID_RED] = 0;//����״̬				
			LED_STATE_OPEN;//�����ܵ�Դ
			LED_YEL_OPEN;//�Ƶƿ���
			
//			BUZZ_OPEN;//����������
//			rt_thread_delay(RT_TICK_PER_SECOND/5); //��0.2s		
//			BUZZ_CLOSE;//�������ر�			
		}
		else
			{LED_YEL_CLOSE;}//�Ƶƹر�
		Lamp_Flag[LampID_YEL] = cmd;//����״̬	
	}
	else if(type == LampID_RED)//���
	{	
		if(cmd)//��
		{			
			g_error_flag = 1;
			Lamp_Flag[LampID_GRN] = 0;//����״̬	
			Lamp_Flag[LampID_YEL] = 0;//����״̬				
		}
		else
			{g_error_flag = 0;}//��ɫ�ر�
		Lamp_Flag[LampID_RED] = cmd;//����״̬			
	}	
}

/*******************************************************************************
* Function Name  : error_thread_entry
* Description    : ����ָʾ�߳�
* Input          : null
*******************************************************************************/
void error_thread_entry(void* parameter)
{
	for(;;)
	{
		while(g_error_flag)
		{
			LED_RED_CLOSE;//�رպ��
			LED_YEL_CLOSE;//�رջƵ�
			LED_GRN_CLOSE;//�ر��̵�				
			LED_STATE_OPEN;//�����ܵ�Դ
			
			LED_RED_OPEN;//��ɫ����	
			BUZZ_OPEN;//����������	
			rt_thread_delay(RT_TICK_PER_SECOND*2/5); //��0.4s	
	
			BUZZ_CLOSE;//�������ر�				
			LED_RED_CLOSE;		
			rt_thread_delay(RT_TICK_PER_SECOND*3/5); //��0.6s		
			
		}
	}
}
/*******************************************************************************
* Function Name  : door_thread_entry
* Description    : ��������_�߳�
* Input          : null
*******************************************************************************/
void door1_thread_entry(void* parameter)
{
		rt_uint8_t data_lock1[4]={MID_Param,MP_Lock,LockID_Lock1,0};//��ͣ���ط�������//2017.9.14���	
		rt_uint32_t	lock1data_len;				//LOCK1��������
		mb_lock1_rece_data	=	rt_mb_create("rece_data_l1",1,RT_IPC_FLAG_FIFO);		//LOCK1ָ��֪ͨ�����ʼ��	
	
		for(;;)
		{
			rt_mb_recv(mb_lock1_rece_data,&lock1data_len,RT_WAITING_FOREVER);
			{
				
				if(lock1data_len == 1)//1����-�ϵ�
				{
						LOCK1_POWER_OFF;//��1ʧ��	
						LOCK2_POWER_OFF;//��2ʧ��	//2017.9.14�޸�
						rt_thread_delay(RT_TICK_PER_SECOND);//��ʱ1s					
				}
				else if(lock1data_len == 0)//0����-ͨ��
				{
					if(!SENSOR_LOCK1_TRIG)//�ж���δ�ر�״̬	
					{
						LOCK1_POWER_ON;//��1ͨ��			
						LOCK2_POWER_ON;//��2ͨ�� //2017.9.14�޸�				
						rt_thread_delay(RT_TICK_PER_SECOND);//��ʱ1s		
						if(!SENSOR_LOCK1_TRIG)//�ر�ʧ��
						{
							LOCK1_POWER_OFF;//��1ʧ��	
							LOCK2_POWER_OFF;//��2ʧ��	//2017.9.14�޸�
						}
					}
					else
					{
						rt_thread_delay(RT_TICK_PER_SECOND);//��ʱ1s							
					}
				}

				data_lock1[3] = SENSOR_LOCK1_STATE;	//��ȡLOCK1״̬				
				CAN1_SendData(MSHEL_IDENTIFIER,data_lock1,4);//����״̬	

			}
		}
}
/*******************************************************************************
* Function Name  : door2_thread_entry
* Description    : ��������_�߳�
* Input          : null
*******************************************************************************/
void door2_thread_entry(void* parameter)
{
		rt_uint16_t	lock2_power_on_count;	//LOCK2ͨ��ʱ���ʱ	
		rt_uint32_t	lock2_data_len;				//LOCK2�������ݳ���
	
		mb_lock2_rece_data	=	rt_mb_create("rece_data_l2",1,RT_IPC_FLAG_FIFO);		//LOCK2ָ��֪ͨ�����ʼ��		
	
		for(;;)
		{
			rt_mb_recv(mb_lock2_rece_data,&lock2_data_len,RT_WAITING_FOREVER);
			{
				if(lock2_data_len == 0)//0�޶�����1����
				{continue;}
				
				lock2_power_on_count = 80;//����ͨ��ʱ��10S
				
				while(SENSOR_LOCK2_TRIG)
				{
					LOCK2_POWER_ON;//��2ͨ��						
					rt_thread_delay(RT_TICK_PER_SECOND/10);//��ʱ0.1s
					lock2_power_on_count--;		
					if(lock2_power_on_count==0)
					{break;}
				}
				rt_thread_delay(RT_TICK_PER_SECOND);//��ʱ1s				
				LOCK2_POWER_OFF;//��2ʧ��			
			}
		}
}
/*******************************************************************************
* Function Name  : lock_thread_entry
* Description    : ����״̬����߳�
* Input          : null
*******************************************************************************/
void lock_thread_entry(void* parameter)
{
		rt_uint8_t data_stop[4]={MID_Param,MP_Lock,LockID_Stop,0};//��ͣ���ط�������

		for(;;)
		{
			Lock_Flag[LockID_Lock1] =  SENSOR_LOCK1_STATE;//LOCK1״̬��1��ʾ������0�ر�
			Lock_Flag[LockID_Lock2] =  SENSOR_LOCK2_STATE;//LOCK1״̬��1��ʾ������0�ر�			
			Lock_Flag[LockID_Stop] 	=  SENSOR_STOP_STATE;	//STOP״̬��1��ʾ������0�ر�	
			
			rt_thread_delay(RT_TICK_PER_SECOND/10);//��ʱ0.1s				
			
			if(Lock_Flag[LockID_Stop] != SENSOR_STOP_STATE)//STOP״̬�����仯
			{
					data_stop[2] = LockID_Stop;				//��ͣ����		
					data_stop[3] = SENSOR_STOP_STATE;	//��ȡ��ͣ����״̬
					CAN1_SendData(MSHEL_IDENTIFIER,data_stop,4);//��������									
			}

			if(Lock_Flag[LockID_Lock1] != SENSOR_LOCK1_STATE)//LOCK1״̬�����仯
			{
					data_stop[2] = LockID_Lock1;				//LOCK1						
					data_stop[3] = SENSOR_LOCK1_STATE;	//��ȡLOCK1״̬
					CAN1_SendData(MSHEL_IDENTIFIER,data_stop,4);//��������									
			}
			
			if(Lock_Flag[LockID_Lock2] != SENSOR_LOCK2_STATE)//LOCK2״̬�����仯
			{
					data_stop[2] = LockID_Lock2;				//LOCK2					
					data_stop[3] = SENSOR_LOCK2_STATE;	//��ȡLOCK2״̬
					CAN1_SendData(MSHEL_IDENTIFIER,data_stop,4);//��������									
			}			
		}
}
