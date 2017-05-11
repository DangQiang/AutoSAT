/*****************************************************
* File      	:	process.c
* Brief				:	��λ�����̿��ƺ���
* Date           Author       Version
* 2015-03-17     DangQiang    V1.0
******************************************************/
#include "process.h"

/*******************************************************************************
* Function Name  : initial_thread_entry
* Description    : ϵͳ��ʼ���̣߳��������ܵĳ�ʼ��
* Input          : null
*******************************************************************************/
void initial_thread_entry(void* parameter)
{
		rt_thread_t init_thread;
	
		/**********��ʼ����������***********/
		TIM_Motor_Model_Init(TIM_Use_3);//������ƶ�ʱ���ĳ�ʼ��
//		AT24C02_Init();			//EEPROM��ʼ��
//		ISP_Model_Init();			//ISP���ܳ�ʼ��

		/**********��ʼ�������߳�**********/
		//LED����ʾ�߳�
		init_thread = rt_thread_create("LED",led_thread_entry, RT_NULL,256, 2, 2);   
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);

	  //CAN1Э�鴦���߳� 1	 
		init_thread = rt_thread_create("CAN1",can1_thread_entry, RT_NULL,512, 1, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);	
}
/*******************************************************************************
* Function Name  : Pause_Handler
* Description    : ϵͳ��ͣ������
* Input          : null
*******************************************************************************/
void Pause_Handler(void)
{
		rt_enter_critical();//��������					
		//�����ô���
			
		rt_exit_critical();//��������								
}
/*******************************************************************************
* Function Name  : Pause_Cancel
* Description    : ��ͣ�ָ�����
* Input          : null
*******************************************************************************/
void Pause_Cancel(void)
{				
		//�ô���
}

/*******************************************************************************
* Function Name  : Invoke_Parallel
* Description    : ���е��ú���
* Input          : program:������ڣ�priority:���ȼ���flag����ɱ�־λ
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
* Description    : ���е��õȴ���ɺ���
* Input          : flag����ɱ�־λ
*******************************************************************************/
void Invoke_End_Wait(rt_uint32_t* flag)
{
		rt_uint32_t *finish_flag=(rt_uint32_t*)flag;
		
		while(!(*finish_flag))//��ѯ��ɱ�־λ
		{rt_thread_delay(1);}//��ʱ

		*finish_flag=0;//�����ɱ�־λ
}
