/*****************************************************
* File			:	shell_io.c
* Brief			:	shell_io控制模块
* Date           Author       Version
* 2017-05-25     DangQiang    V1.0
******************************************************/
#include "shell_io.h"

rt_uint8_t Lamp_Flag[4]={0,0,0,0};//lamp状态,0表示灭，1表示常亮
rt_uint8_t Lock_Flag[3]={0,0,0};	//lock1,lock2,stop状态

rt_mailbox_t		mb_lock1_rece_data;	//LOCK1数据有效邮箱
rt_mailbox_t		mb_lock2_rece_data;	//LOCK2数据有效邮箱

//rt_uint8_t door_main_open_flag;	//main door
//rt_uint8_t door_sub_open_flag;	//subordinate door
rt_uint8_t	g_error_flag;


void lock_thread_entry(void* parameter);//开关状态检测线程
void error_thread_entry(void* parameter);//错误指示线程
void door1_thread_entry(void* parameter);//门1控制线程
void door2_thread_entry(void* parameter);//门2控制线程

/*******************************************************************************
* Function Name  : Shell_Io_Init
* Description    : Shell_Io_初始化
* Input          : null
*******************************************************************************/
void Shell_Io_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		rt_thread_t init_thread;
		
		//输出
		//PD3、4、5、6、7：外壳风扇，照明灯，锁1，锁2，蜂鸣器
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;	
		GPIO_Init(GPIOD, &GPIO_InitStructure);

		//输出	
		//PB3、4、5、6：LED总电源，红色LED，黄色LED，绿色LED
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;	
		GPIO_Init(GPIOB, &GPIO_InitStructure);	

		//输入
		//PA4、6、7：急停、锁2、锁1
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_7;	
		GPIO_Init(GPIOA, &GPIO_InitStructure);	

		FAN_SHELL_OPEN;//仪器风扇开
		LED_LIGHT_CLOSE;//LED照明灯关
		LOCK1_POWER_OFF;//锁1电源关
		LOCK2_POWER_OFF;//锁2电源关
		BUZZ_CLOSE;//蜂鸣器关闭
		LED_STATE_CLOSE;//关闭总电源			
		LED_RED_CLOSE;//关闭红灯
		LED_YEL_CLOSE;//关闭黄灯
		LED_GRN_CLOSE;//关闭绿灯	
				
		init_thread = rt_thread_create("DOOR1",door1_thread_entry, RT_NULL, 256, 2, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);	

		init_thread = rt_thread_create("DOOR2",door2_thread_entry, RT_NULL, 256, 2, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);
		
		init_thread = rt_thread_create("LOCK",lock_thread_entry, RT_NULL, 256, 2, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);		
		
		init_thread = rt_thread_create("ERROR",error_thread_entry, RT_NULL, 256, 2, 2);
		if (init_thread != RT_NULL) rt_thread_startup(init_thread);	

		Lamp_Handler(LampID_GRN,1);	//开机开启状态绿灯
		Lamp_Handler(LampID_LIG,0);	//开机默认关闭照明灯
}
/*******************************************************************************
* Function Name  : Lamp_Handler
* Description    : 灯处理函数
* Input          : type:类型，cmd：动作,1开,0关
*******************************************************************************/
void Lamp_Handler(rt_uint8_t type,rt_uint8_t cmd)
{
	if(type == LampID_LIG)//照明灯
	{
		if(cmd)//开
			{LED_LIGHT_OPEN;}
		else//关
			{LED_LIGHT_CLOSE;}
		Lamp_Flag[LampID_LIG] = cmd;//设置状态		
	}
	if(type == LampID_GRN)//绿灯
	{
		if(cmd)//开
		{	
			g_error_flag = 0;	
			LED_RED_CLOSE;//关闭红灯
			LED_YEL_CLOSE;//关闭黄灯
			LED_GRN_CLOSE;//关闭绿灯	
			Lamp_Flag[LampID_YEL] = 0;//设置状态	
			Lamp_Flag[LampID_RED] = 0;//设置状态				
			LED_STATE_OPEN;//开启总电源
			LED_GRN_OPEN;//绿灯开启
			
//			BUZZ_OPEN;//蜂鸣器开启
//			rt_thread_delay(RT_TICK_PER_SECOND/5); //响0.2s		
//			BUZZ_CLOSE;//蜂鸣器关闭			
		}
		else
			{LED_GRN_CLOSE;}//绿灯关闭		
		Lamp_Flag[LampID_GRN] = cmd;//设置状态				
	}
	else if(type == LampID_YEL)//黄灯
	{
		if(cmd)//开
		{
			g_error_flag = 0;			
			LED_RED_CLOSE;//关闭红灯
			LED_YEL_CLOSE;//关闭黄灯
			LED_GRN_CLOSE;//关闭绿灯	
			Lamp_Flag[LampID_GRN] = 0;//设置状态	
			Lamp_Flag[LampID_RED] = 0;//设置状态				
			LED_STATE_OPEN;//开启总电源
			LED_YEL_OPEN;//黄灯开启
			
//			BUZZ_OPEN;//蜂鸣器开启
//			rt_thread_delay(RT_TICK_PER_SECOND/5); //响0.2s		
//			BUZZ_CLOSE;//蜂鸣器关闭			
		}
		else
			{LED_YEL_CLOSE;}//黄灯关闭
		Lamp_Flag[LampID_YEL] = cmd;//设置状态	
	}
	else if(type == LampID_RED)//红灯
	{	
		if(cmd)//开
		{			
			g_error_flag = 1;
			Lamp_Flag[LampID_GRN] = 0;//设置状态	
			Lamp_Flag[LampID_YEL] = 0;//设置状态				
		}
		else
			{g_error_flag = 0;}//红色关闭
		Lamp_Flag[LampID_RED] = cmd;//设置状态			
	}	
}

/*******************************************************************************
* Function Name  : error_thread_entry
* Description    : 错误指示线程
* Input          : null
*******************************************************************************/
void error_thread_entry(void* parameter)
{
	for(;;)
	{
		while(g_error_flag)
		{
			LED_RED_CLOSE;//关闭红灯
			LED_YEL_CLOSE;//关闭黄灯
			LED_GRN_CLOSE;//关闭绿灯				
			LED_STATE_OPEN;//开启总电源
			
			LED_RED_OPEN;//红色开启	
			BUZZ_OPEN;//蜂鸣器开启	
			rt_thread_delay(RT_TICK_PER_SECOND*2/5); //响0.4s	
	
			BUZZ_CLOSE;//蜂鸣器关闭				
			LED_RED_CLOSE;		
			rt_thread_delay(RT_TICK_PER_SECOND*3/5); //响0.6s		
			
		}
	}
}
/*******************************************************************************
* Function Name  : door_thread_entry
* Description    : 门锁处理_线程
* Input          : null
*******************************************************************************/
void door1_thread_entry(void* parameter)
{
		rt_uint8_t data_lock1[4]={MID_Param,MP_Lock,LockID_Lock1,0};//急停开关返回数据//2017.9.14添加	
		rt_uint32_t	lock1data_len;				//LOCK1接收数据
		mb_lock1_rece_data	=	rt_mb_create("rece_data_l1",1,RT_IPC_FLAG_FIFO);		//LOCK1指令通知邮箱初始化	
	
		for(;;)
		{
			rt_mb_recv(mb_lock1_rece_data,&lock1data_len,RT_WAITING_FOREVER);
			{
				
				if(lock1data_len == 1)//1开锁-断电
				{
						LOCK1_POWER_OFF;//锁1失电	
						LOCK2_POWER_OFF;//锁2失电	//2017.9.14修改
						rt_thread_delay(RT_TICK_PER_SECOND);//延时1s					
				}
				else if(lock1data_len == 0)//0关锁-通电
				{
					if(!SENSOR_LOCK1_TRIG)//判断锁未关闭状态	
					{
						LOCK1_POWER_ON;//锁1通电			
						LOCK2_POWER_ON;//锁2通电 //2017.9.14修改				
						rt_thread_delay(RT_TICK_PER_SECOND);//延时1s		
						if(!SENSOR_LOCK1_TRIG)//关闭失败
						{
							LOCK1_POWER_OFF;//锁1失电	
							LOCK2_POWER_OFF;//锁2失电	//2017.9.14修改
						}
					}
					else
					{
						rt_thread_delay(RT_TICK_PER_SECOND);//延时1s							
					}
				}

				data_lock1[3] = SENSOR_LOCK1_STATE;	//读取LOCK1状态				
				CAN1_SendData(MSHEL_IDENTIFIER,data_lock1,4);//返回状态	

			}
		}
}
/*******************************************************************************
* Function Name  : door2_thread_entry
* Description    : 门锁处理_线程
* Input          : null
*******************************************************************************/
void door2_thread_entry(void* parameter)
{
		rt_uint16_t	lock2_power_on_count;	//LOCK2通电时间计时	
		rt_uint32_t	lock2_data_len;				//LOCK2接收数据长度
	
		mb_lock2_rece_data	=	rt_mb_create("rece_data_l2",1,RT_IPC_FLAG_FIFO);		//LOCK2指令通知邮箱初始化		
	
		for(;;)
		{
			rt_mb_recv(mb_lock2_rece_data,&lock2_data_len,RT_WAITING_FOREVER);
			{
				if(lock2_data_len == 0)//0无动作，1开启
				{continue;}
				
				lock2_power_on_count = 80;//单次通电时间10S
				
				while(SENSOR_LOCK2_TRIG)
				{
					LOCK2_POWER_ON;//锁2通电						
					rt_thread_delay(RT_TICK_PER_SECOND/10);//延时0.1s
					lock2_power_on_count--;		
					if(lock2_power_on_count==0)
					{break;}
				}
				rt_thread_delay(RT_TICK_PER_SECOND);//延时1s				
				LOCK2_POWER_OFF;//锁2失电			
			}
		}
}
/*******************************************************************************
* Function Name  : lock_thread_entry
* Description    : 开关状态检测线程
* Input          : null
*******************************************************************************/
void lock_thread_entry(void* parameter)
{
		rt_uint8_t data_stop[4]={MID_Param,MP_Lock,LockID_Stop,0};//急停开关返回数据

		for(;;)
		{
			Lock_Flag[LockID_Lock1] =  SENSOR_LOCK1_STATE;//LOCK1状态，1表示开启，0关闭
			Lock_Flag[LockID_Lock2] =  SENSOR_LOCK2_STATE;//LOCK1状态，1表示开启，0关闭			
			Lock_Flag[LockID_Stop] 	=  SENSOR_STOP_STATE;	//STOP状态，1表示开启，0关闭	
			
			rt_thread_delay(RT_TICK_PER_SECOND/10);//延时0.1s				
			
			if(Lock_Flag[LockID_Stop] != SENSOR_STOP_STATE)//STOP状态发生变化
			{
					data_stop[2] = LockID_Stop;				//急停开关		
					data_stop[3] = SENSOR_STOP_STATE;	//读取急停开关状态
					CAN1_SendData(MSHEL_IDENTIFIER,data_stop,4);//返回数据									
			}

			if(Lock_Flag[LockID_Lock1] != SENSOR_LOCK1_STATE)//LOCK1状态发生变化
			{
					data_stop[2] = LockID_Lock1;				//LOCK1						
					data_stop[3] = SENSOR_LOCK1_STATE;	//读取LOCK1状态
					CAN1_SendData(MSHEL_IDENTIFIER,data_stop,4);//返回数据									
			}
			
			if(Lock_Flag[LockID_Lock2] != SENSOR_LOCK2_STATE)//LOCK2状态发生变化
			{
					data_stop[2] = LockID_Lock2;				//LOCK2					
					data_stop[3] = SENSOR_LOCK2_STATE;	//读取LOCK2状态
					CAN1_SendData(MSHEL_IDENTIFIER,data_stop,4);//返回数据									
			}			
		}
}
