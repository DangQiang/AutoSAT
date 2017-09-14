/*****************************************************
* File      	:	can1.c
* Brief				:	模块通信主控制函数
* Date           Author       Version
* 2017-05-25     DangQiang    V1.0
******************************************************/
#include "can1.h"

rt_mailbox_t		mb_can1_rece_data;	//CAN1数据有效邮箱
rt_sem_t				sem_can1_send_data;	//CAN1发送数据信号量

CanRxMsg	CAN1_REVE_DATA;	//接收数据结构体
CanTxMsg	CAN1_SEND_DATA;	//发送数据结构体
/*******************************************************************************
* Function Name  : CAN1_Init
* Description    : CAN1初始化函数
* Input          : null
*******************************************************************************/
void CAN1_Init(void)
{
		NVIC_InitTypeDef			NVIC_InitStructure;	
		GPIO_InitTypeDef			GPIO_InitStructure;
		CAN_InitTypeDef				CAN_InitStructure;
		CAN_FilterInitTypeDef	CAN_FilterInitStructure;
	
		/* Enable CANx RX0 interrupt IRQ channel */	
		NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);	
	
		/* CAN GPIOs configuration */
		/* Enable GPIO clock */
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		
		/* Configure CAN RX and TX pins */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		
		/* Connect CAN pins to AF9 */
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_CAN1);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_CAN1); 
		
		/* CAN configuration ********************************************************/  
		/* Enable CAN clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
		
		/* CAN register init */
		CAN_DeInit(CAN1);
		CAN_StructInit(&CAN_InitStructure);
		
		/* CAN cell init */
		CAN_InitStructure.CAN_TTCM = DISABLE;
		CAN_InitStructure.CAN_ABOM = DISABLE;
		CAN_InitStructure.CAN_AWUM = DISABLE;
		CAN_InitStructure.CAN_NART = DISABLE;
		CAN_InitStructure.CAN_RFLM = DISABLE;
		CAN_InitStructure.CAN_TXFP = DISABLE;
		CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
		
		/* CAN Baudrate = 1MBps (CAN clocked at 10 MHz) */
		/* CAN Baudrate = 10M / (CAN_SJW + CAN_BS1 + CAN_BS2) / CAN_Prescaler = 10/(1+2+2)/2 = 1M */
		CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
		CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
		CAN_InitStructure.CAN_BS2 = CAN_BS1_2tq;
		CAN_InitStructure.CAN_Prescaler = 2;
		CAN_Init(CAN1, &CAN_InitStructure);
		
		CAN_FilterInitStructure.CAN_FilterNumber = 0;														//CAN1滤波器号从0到13
		
		CAN_FilterInitStructure.CAN_FilterMode		= CAN_FilterMode_IdList;			//滤波屏蔽模式,列表模式下ID寄存器和屏蔽寄存器地位相等
		CAN_FilterInitStructure.CAN_FilterScale		= CAN_FilterScale_16bit;			//16位下面四个寄存器地位相等
		CAN_FilterInitStructure.CAN_FilterIdHigh	= ((MSHEL_IDENTIFIER|GROUP_IDENTIFIER)<<5);			//外罩总ID
		CAN_FilterInitStructure.CAN_FilterIdLow		= (GROUP_IDENTIFIER<<5);			//设备组ID
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh	= (GLOBAL_IDENTIFIER<<5);	//全局ID
		CAN_FilterInitStructure.CAN_FilterMaskIdLow		= (GLOBAL_IDENTIFIER<<5);	//全局ID
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;	  // /*!< Specifies the FIFO (0 or 1) which will be assigned to the filter.
																														//This parameter can be a value of @ref CAN_filter_FIFO */
		CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
		CAN_FilterInit(&CAN_FilterInitStructure); 

		CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
		CAN_FilterInit(&CAN_FilterInitStructure); 
		/* Enable FIFO 0 message pending Interrupt */
		CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}
/*******************************************************************************
* Function Name  : CAN1_RX0_IRQHandler
* Description    : CAN1接收中断处理函数
* Input          : null
*******************************************************************************/
void CAN1_RX0_IRQHandler(void)
{
		/* enter interrupt */
		rt_interrupt_enter();
	
		CAN_Receive(CAN1, CAN_FIFO0, &CAN1_REVE_DATA);//接收数据
		CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);			//清除挂起中断
		rt_mb_send(mb_can1_rece_data,(CAN1_REVE_DATA.DLC));//接收一帧完成，返回数据长度	
	
		/* leave interrupt */
		rt_interrupt_leave();		
}

/*******************************************************************************
* Function Name  : CAN1_SendData
* Description    : CAN1发送数据
* Input          : mid:发送ID；data_addr:目标数组;length:数据长度,max=8
*******************************************************************************/
void CAN1_SendData(rt_uint8_t mid,rt_uint8_t* data_addr,rt_uint8_t length)
{	
		rt_uint8_t i;
		rt_uint8_t transmit_mailbox = 0;
		
		rt_sem_take(sem_can1_send_data, RT_WAITING_FOREVER);//持有发送信号量，等待发送完成。	
	
		CAN1_SEND_DATA.StdId = mid;
		CAN1_SEND_DATA.IDE = CAN_ID_STD;
		CAN1_SEND_DATA.RTR = CAN_RTR_DATA;	/* 设置为数据帧 */
		CAN1_SEND_DATA.DLC = length;				/* 数据长度, can报文规定最大的数据长度为8字节 */
		for(i = 0;i < length; i ++)
		{
			CAN1_SEND_DATA.Data[i] = *(data_addr+i);
		}
		transmit_mailbox = CAN_Transmit(CAN1,&CAN1_SEND_DATA);  /* 返回这个信息请求发送的邮箱号0,1,2或没有邮箱申请发送no_box */	
		while((CAN_TransmitStatus(CAN1, transmit_mailbox)  !=  CANTXOK) && (i  !=  0xFF))
		{
			i ++;
		}
		rt_sem_release(sem_can1_send_data);//发送完成，释放发送信号量
}
/*******************************************************************************
* Function Name  : can1_thread_entry
* Description    : CAN1控制线程
* Input          : null
*******************************************************************************/
void can1_thread_entry(void* parameter)
{
		rt_uint8_t		can1_data[8];	//CAN线程数据	
		rt_uint32_t		can1_data_len;//CAN1接收数据长度
		rt_uint8_t		i,channel;
	
		mb_can1_rece_data	=	rt_mb_create("rece_data_ok",1,RT_IPC_FLAG_FIFO);		//CAN1指令通知邮箱初始化
		sem_can1_send_data	=	rt_sem_create("sem_send_data",1,RT_IPC_FLAG_FIFO);//CAN1发送数据信号量初始化
	
		CAN1_Init();	//CAN1初始化

		for(;;)
		{
			rt_mb_recv(mb_can1_rece_data,&can1_data_len,RT_WAITING_FOREVER);	
			
			if(can1_data_len == 0)//无数据空包为心跳包
			{
					CAN1_SendData(MSHEL_IDENTIFIER,can1_data,0);//心跳包应答为空包						
			}
			else		//数据包
			{		
				if(CAN1_REVE_DATA.StdId == (MSHEL_IDENTIFIER | GROUP_IDENTIFIER))//接收数据ID判断
				{
				
					for(i=0;i<can1_data_len;i++)//CAN数据复制，有效字节
					{can1_data[i]=CAN1_REVE_DATA.Data[i];}	
					for(i=can1_data_len;i<8;i++)//CAN数据复制，补0
					{can1_data[i]=0;}

					if(can1_data[0]==MID_Param)//参数
					{
						switch(can1_data[1]&(~MASK_MP_Write))
						{	
							case MP_Lamp://照明的，LED绿、黄、红
								if(can1_data[1]&MASK_MP_Write)//设置
								{
									Lamp_Handler(can1_data[2],can1_data[3]);//灯控制处理
									
									can1_data[0] = MID_MsgAck;
									can1_data[1] = 0;
									CAN1_SendData(MSHEL_IDENTIFIER,can1_data,2);//返回应答											
								}
								else//查询
								{
									can1_data[0] = MID_MsgAck;
									can1_data[1] = 0;
									CAN1_SendData(MSHEL_IDENTIFIER,can1_data,2);//返回应答

									can1_data[0] = MID_Param;
									can1_data[1] = MP_Lamp;										
									can1_data[3] = Lamp_Flag[can1_data[2]];//查询状态		
									CAN1_SendData(MSHEL_IDENTIFIER,can1_data,4);//返回数据									
								}
								break;
								
							case MP_Lock://门锁1，2，急停开关
								if(can1_data[1]&MASK_MP_Write)//设置
								{
									if(can1_data[2]==LockID_Lock1)//锁1
										{rt_mb_send(mb_lock1_rece_data,can1_data[3]);}//接收控制
//									else	if(can1_data[2]==LockID_Lock2)//锁2 //2017.9.14修改
//										{rt_mb_send(mb_lock2_rece_data,can1_data[3]);}//锁2控制
									
									can1_data[0] = MID_MsgAck;
									can1_data[1] = 0;
									CAN1_SendData(MSHEL_IDENTIFIER,can1_data,2);//返回应答											
								}
								else//查询
								{
									can1_data[0] = MID_MsgAck;
									can1_data[1] = 0;
									CAN1_SendData(MSHEL_IDENTIFIER,can1_data,2);//返回应答

									can1_data[0] = MID_Param;
									can1_data[1] = MP_Lock;									
									can1_data[3] = Lock_Flag[can1_data[2]];//查询状态		
									CAN1_SendData(MSHEL_IDENTIFIER,can1_data,4);//返回数据									
								}
								break;	

							case MP_ThgAcquisition://温湿度使能
								if(can1_data[1]&MASK_MP_Write)//设置
								{
									g_sht2x_result[can1_data[2]].Enable_flag = can1_data[3];//允许采集标志位，0禁止，1开启
									
									can1_data[0] = MID_MsgAck;
									can1_data[1] = g_sht2x_result[can1_data[2]].Exist_flag;									
									CAN1_SendData(MSHEL_IDENTIFIER,can1_data,2);//返回应答，D1返回传感器状态											
								}
								else//查询
								{
									channel = can1_data[2];									
									can1_data[0] = MID_MsgAck;
									can1_data[1] = g_sht2x_result[channel].Exist_flag;
									CAN1_SendData(MSHEL_IDENTIFIER,can1_data,2);//返回应答
									
									can1_data[0] = MID_Param;
									can1_data[1] = MP_ThgAcquisition;	
									can1_data[2] = channel;										
									can1_data[3] = g_sht2x_result[channel].Enable_flag;//查询状态		
									CAN1_SendData(MSHEL_IDENTIFIER,can1_data,4);//返回数据									
								}
								break;
								
							case MP_ThgInterval://温湿度计间隔
								if(can1_data[1]&MASK_MP_Write)//设置
								{
									g_sht2x_result[can1_data[2]].Collect_time = can1_data[3];//允许采集标志位，0禁止，1开启
									
									can1_data[0] = MID_MsgAck;
									can1_data[1] = g_sht2x_result[can1_data[2]].Exist_flag;									
									CAN1_SendData(MSHEL_IDENTIFIER,can1_data,2);//返回应答，D1返回传感器状态											
								}
								else//查询
								{
									channel = can1_data[2];										
									can1_data[0] = MID_MsgAck;
									can1_data[1] = g_sht2x_result[channel].Exist_flag;
									CAN1_SendData(MSHEL_IDENTIFIER,can1_data,2);//返回应答

									can1_data[0] = MID_Param;
									can1_data[1] = MP_ThgInterval;	
									can1_data[2] = channel;											
									can1_data[3] = g_sht2x_result[channel].Collect_time;//查询状态		
									CAN1_SendData(MSHEL_IDENTIFIER,can1_data,4);//返回数据									
								}
								break;								

								default:
									break;
						}							
						
					}
					else if(can1_data[0]== MID_ThgDetect)//温湿度值查询
					{
						channel = can1_data[1];
						can1_data[0] = MID_MsgAck;
						can1_data[1] = g_sht2x_result[channel].Exist_flag;
						CAN1_SendData(MSHEL_IDENTIFIER,can1_data,2);//返回应答

						can1_data[0] = MID_ThgDetect;
						can1_data[1] = channel;
						can1_data[2] = (((rt_uint16_t)(g_sht2x_result[channel].Temperature_HM * 100)) >> 8);//查询状态		
						can1_data[3] = ((rt_uint16_t)(g_sht2x_result[channel].Temperature_HM * 100));//查询状态	
						can1_data[4] = (((rt_uint16_t)(g_sht2x_result[channel].Humidity_HM * 100)) >> 8);//查询状态		
						can1_data[5] = ((rt_uint16_t)(g_sht2x_result[channel].Humidity_HM * 100));//查询状态							
						CAN1_SendData(MSHEL_IDENTIFIER,can1_data,6);//返回数据								
					}				
				}
			}					
		}
}
