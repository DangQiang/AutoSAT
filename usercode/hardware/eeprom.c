/*****************************************************
* File			:	eeprom.c
* Brief			:	EEPROM存储模块
* Date           Author       Version
* 2017-05-25     DangQiang    V1.1
******************************************************/

/* Includes */
#include "eeprom.h"

rt_sem_t	sem_eeprom_rw_data;									//EEPROM读写数据信号量
/*******************************************************************************
* Function Name  : IIC1_Init
* Description    : IIC1_初始化
* Input          : null
*******************************************************************************/
void IIC1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(iic1_rcc,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = iic1_scl_pin | iic1_sda_pin;	
	GPIO_Init(iic1_gpio, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : IIC1_Init
* Description    : IIC1_起始
* Input          : null
*******************************************************************************/
void IIC1_Start(void)
{
	IIC1_SDA_OUT;     												/*sda线输出*/
	IIC1_SDA=1;	  	  
	IIC1_SCL=1;
	delay_us(4);
	IIC1_SDA=0;																/*START:when CLK is high,DATA change form high to low*/
	delay_us(4);
	IIC1_SCL=0;																/*钳住I2C总线，准备发送或接收数据*/
}	  
/*******************************************************************************
* Function Name  : IIC1_Stop
* Description    : IIC1_停止
* Input          : null
*******************************************************************************/
void IIC1_Stop(void)
{
	IIC1_SDA_OUT;															/*sda线输出*/
	IIC1_SCL=0;
	IIC1_SDA=0;																/*STOP:when CLK is high DATA change form low to high*/
	delay_us(4);
	IIC1_SCL=1; 
	IIC1_SDA=1;																/*发送I2C总线结束信号*/
	delay_us(4);							   	
}

/*******************************************************************************
* Function Name  : IIC1_Wait_Ack
* Description    : IIC1_等待应答
* Input          : null
* Retrun         : 1:成功 0:失败
*******************************************************************************/
uint8_t IIC1_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	IIC1_SDA_IN;      												/*SDA设置为输入*/
	IIC1_SDA=1;
	delay_us(1);	   
	IIC1_SCL=1;
	delay_us(1);	 
	while(READ1_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC1_Stop();
			return 0;															/*如果从机没把SDA线拉低 则输出0*/
		}
	}
	IIC1_SCL=0;																/*如果从机把SDA线拉低 则输出1*/
	return 1;  
} 
/*******************************************************************************
* Function Name  : IIC1_Ack
* Description    : IIC1_等待应答
* Input          : null
*******************************************************************************/
void IIC1_Ack(void)
{
	IIC1_SCL=0;
	IIC1_SDA_OUT;
	IIC1_SDA=0;
	delay_us(2);
	IIC1_SCL=1;
	delay_us(2);
	IIC1_SCL=0;
}
/*******************************************************************************
* Function Name  : IIC1_NAck
* Description    : IIC1_不产生ACK应答
* Input          : null
*******************************************************************************/
void IIC1_NAck(void)
{
	IIC1_SCL=0;
	IIC1_SDA_OUT;
	IIC1_SDA=1;
	delay_us(2);
	IIC1_SCL=1;
	delay_us(2);
	IIC1_SCL=0;
}	
/*******************************************************************************
* Function Name  : IIC1_Send_Byte
* Description    : IIC1_发送一个字节
* Input          : tx_data发送的字节
*******************************************************************************/
void IIC1_Send_Byte(uint8_t tx_data)
{                        
	uint8_t t;   
	IIC1_SDA_OUT; 	    
	IIC1_SCL=0;																/*拉低时钟开始数据传输*/
	for(t=0;t<8;t++)
	{              
		if((tx_data&0x80)>>7)
		IIC1_SDA=1;
		else
		IIC1_SDA=0;
		tx_data<<=1; 	  
		delay_us(2);   													/*对TEA5767这三个延时都是必须的*/
		IIC1_SCL=1;
		delay_us(2); 
		IIC1_SCL=0;	
		delay_us(2);
	}	 
}
/*******************************************************************************
* Function Name  : IIC1_Read_Byte
* Description    : IIC1_读取一个字节
* Input          : 
* Return				 : 返回读取的字节
*******************************************************************************/
uint8_t IIC1_Read_Byte(uint8_t ack)
{
	uint8_t i=0,receive=0;
	IIC1_SDA_IN;															/*SDA设置为输入*/
	for(i=0;i<8;i++ )
	{
		IIC1_SCL=0; 
		delay_us(2);
		IIC1_SCL=1;
		receive<<=1;
		if(READ1_SDA)	receive++;   
		delay_us(1);  
	}					 
	if (ack)
	IIC1_Ack();																/*发送ACK*/
	else
	IIC1_NAck(); 															/*发送NACK*/  
	return receive;
}

/*******************************************************************************
* Function Name  : AT24C02_ReadOneByte
* Description    : AT24C02_读取一个字节
* Input          : ReadAddr：地址
* Return				 : 返回读取的字节
*******************************************************************************/
uint8_t AT24C02_ReadOneByte(uint8_t ReadAddr)
{				  
	uint8_t temp=0;	

	rt_sem_take(sem_eeprom_rw_data, RT_WAITING_FOREVER);//持有读取信号量，等待读取完成。	
	
	IIC1_Start();  
	IIC1_Send_Byte(0XA0);   														/*发送器件地址0XA0,写数据*/ 	 

	if( IIC1_Wait_Ack() )
	{		
		IIC1_Send_Byte(ReadAddr%256);   									/*发送地址*/
		if( IIC1_Wait_Ack() )
		{
			IIC1_Start();  	 	   
			IIC1_Send_Byte(0XA1);           								/*进入接收模式*/			   
			if( IIC1_Wait_Ack() )
			{
				temp=IIC1_Read_Byte(0);									/*读取数据  不发送应答信号*/		   
				IIC1_Stop();														/*产生一个停止条件*/	    

			}
			else
				temp	= 0;
		}
		else
			temp	= 0;
	}
	else 
		temp	= 0;
	
	rt_sem_release(sem_eeprom_rw_data);	//读取完成，释放信号量			
	return temp;	
}
/*******************************************************************************
* Function Name  : AT24C02_WriteOneByte
* Description    : AT24C02_读取一个字节
* Input          : WriteAddr  :写入数据的目的地址 DataToWrite：写入的字节
* Return				 : 返回读取的字节
*******************************************************************************/
uint8_t AT24C02_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite)
{
	uint8_t result;	

	rt_sem_take(sem_eeprom_rw_data, RT_WAITING_FOREVER);//持有发送信号量，等待发送完成。	
	
	IIC1_Start();  
	IIC1_Send_Byte(0XA0);   									/*发送器件地址0XA0,写数据*/
	if( IIC1_Wait_Ack() )
	{
		IIC1_Send_Byte(WriteAddr%256);   				/*发送低地址*/
		if( IIC1_Wait_Ack() )
		{
			IIC1_Send_Byte(DataToWrite);     			/*发送字节*/							   
			if( IIC1_Wait_Ack() )
			{
				IIC1_Stop();												/*产生一个停止条件*/
				rt_thread_delay(2);		
				result	= 1;
			}
			else
				result	= 0;
		}
		else
			result	= 0;	
	}
	else
		result	= 0;
		
	rt_sem_release(sem_eeprom_rw_data);	//发送完成，释放发送信号量		
	
	return result;
}
/*******************************************************************************
* Function Name  : AT24C02_Check
* Description    : AT24C02_检查
* Input          : null
* Return				 : 返回1成功，返回0失败
*******************************************************************************/
uint8_t AT24C02_Check(void)
{
	uint8_t temp = 0;
	
	temp=AT24C02_ReadOneByte(255);									/*避免每次开机都写AT24CXX*/			   
	if(temp==0x55)return 1;		   
	else																						/*排除第一次初始化的情况*/
	{
		AT24C02_WriteOneByte(255,0x55);
		temp=AT24C02_ReadOneByte(255);	  
		if(temp==0x55)return 1;
	}
	return 0;											  
}
/*******************************************************************************
* Function Name  : AT24C02_Init
* Description    : AT24C02_初始化
* Return				 : 返回1成功，返回0失败
*******************************************************************************/
uint8_t AT24C02_Init(void)
{
	sem_eeprom_rw_data	=	rt_sem_create("eeprom_rw",1,RT_IPC_FLAG_FIFO);//EEPROM读写数据信号量初始化
	
	IIC1_Init();
	return AT24C02_Check();
}
