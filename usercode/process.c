/*****************************************************
* File      	:	process.c
* Brief				:	��λ�����̿��ƺ���
* Date           Author       Version
* 2017-05-25     DangQiang    V1.0
******************************************************/
#include "process.h"

//rt_uint8_t	sht2x_enable_flag[3];//sht2x�������ɼ�ʹ�ܱ�־λ
//rt_uint8_t	sht2x_exist_flag[3];//sht2x���������ڱ�־λ
//rt_uint8_t	sht2x_collect_time[3];//sht2x�������ɼ����

void	sht2x_a_thread_entry(void* parameter);//sht2x_a��ʪ�Ȳɼ��߳�
void	sht2x_b_thread_entry(void* parameter);//sht2x_b��ʪ�Ȳɼ��߳�
void	sht2x_c_thread_entry(void* parameter);//sht2x_c��ʪ�Ȳɼ��߳�
/*******************************************************************************
* Function Name  : initial_thread_entry
* Description    : ϵͳ��ʼ���̣߳��������ܵĳ�ʼ��
* Input          : null
*******************************************************************************/
void initial_thread_entry(void* parameter)
{
		rt_thread_t init_thread;
	
		/**********��ʼ����������***********/
		Shell_Io_Init();//SHELL IO ��ʼ��
		AT24C02_Init();	//EEPROM��ʼ��

		/**********��ʼ�������߳�**********/
		//LED����ʾ�߳�
		init_thread = rt_thread_create("LED",led_thread_entry, RT_NULL,256, 2, 2);   
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);

	  //CAN1Э�鴦���߳� 1	 
		init_thread = rt_thread_create("CAN1",can1_thread_entry, RT_NULL,512, 1, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);	
	
	  //��ʪ�ȴ������߳� 
		init_thread = rt_thread_create("SHT2_A",sht2x_a_thread_entry, RT_NULL,512, 2, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);	

	  //��ʪ�ȴ������߳� 
		init_thread = rt_thread_create("SHT2_B",sht2x_b_thread_entry, RT_NULL,512, 2, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);	

	  //��ʪ�ȴ������߳� 
		init_thread = rt_thread_create("SHT2_C",sht2x_c_thread_entry, RT_NULL,512, 2, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);	
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
/*******************************************************************************
* Function Name  : Sht2x_Send_Data
* Description    : sht2x��ʪ�ȷ���
* Input          : sht2x_data:���������ݽṹ��
*******************************************************************************/
void Sht2x_Send_Data(rt_uint8_t sensor_num,SHT2X_PARAM sht2x_data)
{
		rt_uint16_t		tmp,hum;//uint�¶�ʪ��,�м����
		rt_uint8_t		tx_data[6]={MID_ThgDetect,0x00,0x00,0x00,0x00,0x00};	//��ʪ�Ȳɼ���������	
		
		tmp = sht2x_data.Temperature_HM * 100;//����0.01��
		hum = sht2x_data.Humidity_HM * 100;		//����0.01%RH
		tx_data[1]=sensor_num;//���������
		tx_data[2]=tmp>>8;		//�������¶�ֵ��λ			
		tx_data[3]=tmp;				//�������¶�ֵ��λ
		tx_data[4]=hum>>8;		//������ʪ��ֵ��λ	
		tx_data[5]=hum;				//������ʪ��ֵ��λ		
		CAN1_SendData(MSHEL_IDENTIFIER,tx_data,6);//ָ���			
}
/*******************************************************************************
* Function Name  : sht2x_a_thread_entry
* Description    : sht2x_a��ʪ�Ȳɼ��߳�
* Input          : null
*******************************************************************************/
void sht2x_a_thread_entry(void* parameter)
{
		rt_uint32_t	count_a = 0;//���㣬�����Լ����Ƽ��Ϊ1S
		rt_uint8_t	time = 0,differ;//���㣬�����Լ����Ƽ��Ϊ1S
		rt_uint8_t	count_send = 0;//���ڰ�ʱ������������	

		rt_thread_delay(RT_TICK_PER_SECOND/5);//��ʱ	0.2s
		SHT2X_A_Init();	//SHT21��ʼ��	
		
		for(;;)
		{	
			rt_thread_delay(time);//��ʱ,
			SHT2X_A_Measure();//��ʪ��C����

			if((g_sht2x_result[0].Enable_flag)&&(!g_sht2x_result[0].Exist_flag))			
			{	
				differ = ((rt_thread_self()->thread_timer.timeout_tick - count_a))%100;//���㣬�����Լ����Ƽ��Ϊ1S	
				differ = (RT_TICK_PER_SECOND - differ)%100;//���㣬�����Լ����Ƽ��Ϊ1S
				time = (time + differ) %100;//���㣬�����Լ����Ƽ��Ϊ1S
				
				count_a	=	rt_thread_self()->thread_timer.timeout_tick;	
				
				count_send ++;
				if(count_send >= g_sht2x_result[0].Collect_time)
				{				
					Sht2x_Send_Data(0,g_sht2x_result[0]);//���ʹ�����C����	
					count_send=0;
				}	
			}
		}

}
/*******************************************************************************
* Function Name  : sht2x_b_thread_entry
* Description    : sht2x_b��ʪ�Ȳɼ��߳�
* Input          : null
*******************************************************************************/
void sht2x_b_thread_entry(void* parameter)
{
		rt_uint32_t	count_b = 0;//���㣬�����Լ����Ƽ��Ϊ1S
		rt_uint8_t	time = 0,differ;//���㣬�����Լ����Ƽ��Ϊ1S
		rt_uint8_t	count_send = 0;//���ڰ�ʱ������������	

		rt_thread_delay(RT_TICK_PER_SECOND/3);//��ʱ	0.3s	
		SHT2X_B_Init();	//SHT21��ʼ��	
		
		for(;;)
		{	
			rt_thread_delay(time);//��ʱ,
			SHT2X_B_Measure();//��ʪ��C����

			if((g_sht2x_result[1].Enable_flag)&&(!g_sht2x_result[1].Exist_flag))			
			{	
				differ = ((rt_thread_self()->thread_timer.timeout_tick - count_b))%100;//���㣬�����Լ����Ƽ��Ϊ1S	
				differ = (RT_TICK_PER_SECOND - differ)%100;//���㣬�����Լ����Ƽ��Ϊ1S
				time = (time + differ) %100;//���㣬�����Լ����Ƽ��Ϊ1S
				
				count_b	=	rt_thread_self()->thread_timer.timeout_tick;	
				
				count_send ++;
				if(count_send >= g_sht2x_result[1].Collect_time)
				{				
					Sht2x_Send_Data(1,g_sht2x_result[1]);//���ʹ�����C����	
					count_send=0;
				}	
			}
		}

}/*******************************************************************************
* Function Name  : sht2x_c_thread_entry
* Description    : sht2x_c��ʪ�Ȳɼ��߳�
* Input          : null
*******************************************************************************/
void sht2x_c_thread_entry(void* parameter)
{
		rt_uint32_t	count_c = 0;//���㣬�����Լ����Ƽ��Ϊ1S
		rt_uint8_t	time = 0,differ;//���㣬�����Լ����Ƽ��Ϊ1S
		rt_uint8_t	count_send = 0;//���ڰ�ʱ������������	

		rt_thread_delay(RT_TICK_PER_SECOND/2);//��ʱ	0.5s		
		SHT2X_C_Init();	//SHT21��ʼ��	
		
		for(;;)
		{	
			rt_thread_delay(time);//��ʱ,
			SHT2X_C_Measure();//��ʪ��C����

			if((g_sht2x_result[2].Enable_flag)&&(!g_sht2x_result[2].Exist_flag))			
			{	
				differ = ((rt_thread_self()->thread_timer.timeout_tick - count_c))%100;//���㣬�����Լ����Ƽ��Ϊ1S	
				differ = (RT_TICK_PER_SECOND - differ)%100;//���㣬�����Լ����Ƽ��Ϊ1S
				time = (time + differ) %100;//���㣬�����Լ����Ƽ��Ϊ1S
				
				count_c	=	rt_thread_self()->thread_timer.timeout_tick;	
				
				count_send ++;
				if(count_send >= g_sht2x_result[2].Collect_time)
				{				
					Sht2x_Send_Data(2,g_sht2x_result[2]);//���ʹ�����C����	
					count_send=0;
				}	
			}
		}
}
