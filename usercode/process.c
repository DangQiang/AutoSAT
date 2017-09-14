/*****************************************************
* File      	:	process.c
* Brief				:	下位机流程控制函数
* Date           Author       Version
* 2017-05-25     DangQiang    V1.0
******************************************************/
#include "process.h"

//rt_uint8_t	sht2x_enable_flag[3];//sht2x传感器采集使能标志位
//rt_uint8_t	sht2x_exist_flag[3];//sht2x传感器存在标志位
//rt_uint8_t	sht2x_collect_time[3];//sht2x传感器采集间隔

void	sht2x_a_thread_entry(void* parameter);//sht2x_a温湿度采集线程
void	sht2x_b_thread_entry(void* parameter);//sht2x_b温湿度采集线程
void	sht2x_c_thread_entry(void* parameter);//sht2x_c温湿度采集线程
/*******************************************************************************
* Function Name  : initial_thread_entry
* Description    : 系统初始化线程，公共功能的初始化
* Input          : null
*******************************************************************************/
void initial_thread_entry(void* parameter)
{
		rt_thread_t init_thread;
	
		/**********初始化公共功能***********/
		Shell_Io_Init();//SHELL IO 初始化
		AT24C02_Init();	//EEPROM初始化

		/**********初始化独立线程**********/
		//LED灯显示线程
		init_thread = rt_thread_create("LED",led_thread_entry, RT_NULL,256, 2, 2);   
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);

	  //CAN1协议处理线程 1	 
		init_thread = rt_thread_create("CAN1",can1_thread_entry, RT_NULL,512, 1, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);	
	
	  //温湿度传感器线程 
		init_thread = rt_thread_create("SHT2_A",sht2x_a_thread_entry, RT_NULL,512, 2, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);	

	  //温湿度传感器线程 
		init_thread = rt_thread_create("SHT2_B",sht2x_b_thread_entry, RT_NULL,512, 2, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);	

	  //温湿度传感器线程 
		init_thread = rt_thread_create("SHT2_C",sht2x_c_thread_entry, RT_NULL,512, 2, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);	
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
/*******************************************************************************
* Function Name  : Sht2x_Send_Data
* Description    : sht2x温湿度返回
* Input          : sht2x_data:传感器数据结构体
*******************************************************************************/
void Sht2x_Send_Data(rt_uint8_t sensor_num,SHT2X_PARAM sht2x_data)
{
		rt_uint16_t		tmp,hum;//uint温度湿度,中间变量
		rt_uint8_t		tx_data[6]={MID_ThgDetect,0x00,0x00,0x00,0x00,0x00};	//温湿度采集返回数据	
		
		tmp = sht2x_data.Temperature_HM * 100;//精度0.01℃
		hum = sht2x_data.Humidity_HM * 100;		//精度0.01%RH
		tx_data[1]=sensor_num;//传感器编号
		tx_data[2]=tmp>>8;		//传感器温度值高位			
		tx_data[3]=tmp;				//传感器温度值低位
		tx_data[4]=hum>>8;		//传感器湿度值高位	
		tx_data[5]=hum;				//传感器湿度值低位		
		CAN1_SendData(MSHEL_IDENTIFIER,tx_data,6);//指令返回			
}
/*******************************************************************************
* Function Name  : sht2x_a_thread_entry
* Description    : sht2x_a温湿度采集线程
* Input          : null
*******************************************************************************/
void sht2x_a_thread_entry(void* parameter)
{
		rt_uint32_t	count_a = 0;//计算，用以自己控制间隔为1S
		rt_uint8_t	time = 0,differ;//计算，用以自己控制间隔为1S
		rt_uint8_t	count_send = 0;//用于按时间间隔发送数据	

		rt_thread_delay(RT_TICK_PER_SECOND/5);//延时	0.2s
		SHT2X_A_Init();	//SHT21初始化	
		
		for(;;)
		{	
			rt_thread_delay(time);//延时,
			SHT2X_A_Measure();//温湿度C测量

			if((g_sht2x_result[0].Enable_flag)&&(!g_sht2x_result[0].Exist_flag))			
			{	
				differ = ((rt_thread_self()->thread_timer.timeout_tick - count_a))%100;//计算，用以自己控制间隔为1S	
				differ = (RT_TICK_PER_SECOND - differ)%100;//计算，用以自己控制间隔为1S
				time = (time + differ) %100;//计算，用以自己控制间隔为1S
				
				count_a	=	rt_thread_self()->thread_timer.timeout_tick;	
				
				count_send ++;
				if(count_send >= g_sht2x_result[0].Collect_time)
				{				
					Sht2x_Send_Data(0,g_sht2x_result[0]);//发送传感器C数据	
					count_send=0;
				}	
			}
		}

}
/*******************************************************************************
* Function Name  : sht2x_b_thread_entry
* Description    : sht2x_b温湿度采集线程
* Input          : null
*******************************************************************************/
void sht2x_b_thread_entry(void* parameter)
{
		rt_uint32_t	count_b = 0;//计算，用以自己控制间隔为1S
		rt_uint8_t	time = 0,differ;//计算，用以自己控制间隔为1S
		rt_uint8_t	count_send = 0;//用于按时间间隔发送数据	

		rt_thread_delay(RT_TICK_PER_SECOND/3);//延时	0.3s	
		SHT2X_B_Init();	//SHT21初始化	
		
		for(;;)
		{	
			rt_thread_delay(time);//延时,
			SHT2X_B_Measure();//温湿度C测量

			if((g_sht2x_result[1].Enable_flag)&&(!g_sht2x_result[1].Exist_flag))			
			{	
				differ = ((rt_thread_self()->thread_timer.timeout_tick - count_b))%100;//计算，用以自己控制间隔为1S	
				differ = (RT_TICK_PER_SECOND - differ)%100;//计算，用以自己控制间隔为1S
				time = (time + differ) %100;//计算，用以自己控制间隔为1S
				
				count_b	=	rt_thread_self()->thread_timer.timeout_tick;	
				
				count_send ++;
				if(count_send >= g_sht2x_result[1].Collect_time)
				{				
					Sht2x_Send_Data(1,g_sht2x_result[1]);//发送传感器C数据	
					count_send=0;
				}	
			}
		}

}/*******************************************************************************
* Function Name  : sht2x_c_thread_entry
* Description    : sht2x_c温湿度采集线程
* Input          : null
*******************************************************************************/
void sht2x_c_thread_entry(void* parameter)
{
		rt_uint32_t	count_c = 0;//计算，用以自己控制间隔为1S
		rt_uint8_t	time = 0,differ;//计算，用以自己控制间隔为1S
		rt_uint8_t	count_send = 0;//用于按时间间隔发送数据	

		rt_thread_delay(RT_TICK_PER_SECOND/2);//延时	0.5s		
		SHT2X_C_Init();	//SHT21初始化	
		
		for(;;)
		{	
			rt_thread_delay(time);//延时,
			SHT2X_C_Measure();//温湿度C测量

			if((g_sht2x_result[2].Enable_flag)&&(!g_sht2x_result[2].Exist_flag))			
			{	
				differ = ((rt_thread_self()->thread_timer.timeout_tick - count_c))%100;//计算，用以自己控制间隔为1S	
				differ = (RT_TICK_PER_SECOND - differ)%100;//计算，用以自己控制间隔为1S
				time = (time + differ) %100;//计算，用以自己控制间隔为1S
				
				count_c	=	rt_thread_self()->thread_timer.timeout_tick;	
				
				count_send ++;
				if(count_send >= g_sht2x_result[2].Collect_time)
				{				
					Sht2x_Send_Data(2,g_sht2x_result[2]);//发送传感器C数据	
					count_send=0;
				}	
			}
		}
}
