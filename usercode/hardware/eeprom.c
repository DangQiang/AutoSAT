/*****************************************************
* File			:	eeprom.c
* Brief			:	EEPROM�洢ģ��
* Date           Author       Version
* 2017-05-25     DangQiang    V1.1
******************************************************/

/* Includes */
#include "eeprom.h"

rt_sem_t	sem_eeprom_rw_data;									//EEPROM��д�����ź���
/*******************************************************************************
* Function Name  : IIC1_Init
* Description    : IIC1_��ʼ��
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
* Description    : IIC1_��ʼ
* Input          : null
*******************************************************************************/
void IIC1_Start(void)
{
	IIC1_SDA_OUT;     												/*sda�����*/
	IIC1_SDA=1;	  	  
	IIC1_SCL=1;
	delay_us(4);
	IIC1_SDA=0;																/*START:when CLK is high,DATA change form high to low*/
	delay_us(4);
	IIC1_SCL=0;																/*ǯסI2C���ߣ�׼�����ͻ��������*/
}	  
/*******************************************************************************
* Function Name  : IIC1_Stop
* Description    : IIC1_ֹͣ
* Input          : null
*******************************************************************************/
void IIC1_Stop(void)
{
	IIC1_SDA_OUT;															/*sda�����*/
	IIC1_SCL=0;
	IIC1_SDA=0;																/*STOP:when CLK is high DATA change form low to high*/
	delay_us(4);
	IIC1_SCL=1; 
	IIC1_SDA=1;																/*����I2C���߽����ź�*/
	delay_us(4);							   	
}

/*******************************************************************************
* Function Name  : IIC1_Wait_Ack
* Description    : IIC1_�ȴ�Ӧ��
* Input          : null
* Retrun         : 1:�ɹ� 0:ʧ��
*******************************************************************************/
uint8_t IIC1_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	IIC1_SDA_IN;      												/*SDA����Ϊ����*/
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
			return 0;															/*����ӻ�û��SDA������ �����0*/
		}
	}
	IIC1_SCL=0;																/*����ӻ���SDA������ �����1*/
	return 1;  
} 
/*******************************************************************************
* Function Name  : IIC1_Ack
* Description    : IIC1_�ȴ�Ӧ��
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
* Description    : IIC1_������ACKӦ��
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
* Description    : IIC1_����һ���ֽ�
* Input          : tx_data���͵��ֽ�
*******************************************************************************/
void IIC1_Send_Byte(uint8_t tx_data)
{                        
	uint8_t t;   
	IIC1_SDA_OUT; 	    
	IIC1_SCL=0;																/*����ʱ�ӿ�ʼ���ݴ���*/
	for(t=0;t<8;t++)
	{              
		if((tx_data&0x80)>>7)
		IIC1_SDA=1;
		else
		IIC1_SDA=0;
		tx_data<<=1; 	  
		delay_us(2);   													/*��TEA5767��������ʱ���Ǳ����*/
		IIC1_SCL=1;
		delay_us(2); 
		IIC1_SCL=0;	
		delay_us(2);
	}	 
}
/*******************************************************************************
* Function Name  : IIC1_Read_Byte
* Description    : IIC1_��ȡһ���ֽ�
* Input          : 
* Return				 : ���ض�ȡ���ֽ�
*******************************************************************************/
uint8_t IIC1_Read_Byte(uint8_t ack)
{
	uint8_t i=0,receive=0;
	IIC1_SDA_IN;															/*SDA����Ϊ����*/
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
	IIC1_Ack();																/*����ACK*/
	else
	IIC1_NAck(); 															/*����NACK*/  
	return receive;
}

/*******************************************************************************
* Function Name  : AT24C02_ReadOneByte
* Description    : AT24C02_��ȡһ���ֽ�
* Input          : ReadAddr����ַ
* Return				 : ���ض�ȡ���ֽ�
*******************************************************************************/
uint8_t AT24C02_ReadOneByte(uint8_t ReadAddr)
{				  
	uint8_t temp=0;	

	rt_sem_take(sem_eeprom_rw_data, RT_WAITING_FOREVER);//���ж�ȡ�ź������ȴ���ȡ��ɡ�	
	
	IIC1_Start();  
	IIC1_Send_Byte(0XA0);   														/*����������ַ0XA0,д����*/ 	 

	if( IIC1_Wait_Ack() )
	{		
		IIC1_Send_Byte(ReadAddr%256);   									/*���͵�ַ*/
		if( IIC1_Wait_Ack() )
		{
			IIC1_Start();  	 	   
			IIC1_Send_Byte(0XA1);           								/*�������ģʽ*/			   
			if( IIC1_Wait_Ack() )
			{
				temp=IIC1_Read_Byte(0);									/*��ȡ����  ������Ӧ���ź�*/		   
				IIC1_Stop();														/*����һ��ֹͣ����*/	    

			}
			else
				temp	= 0;
		}
		else
			temp	= 0;
	}
	else 
		temp	= 0;
	
	rt_sem_release(sem_eeprom_rw_data);	//��ȡ��ɣ��ͷ��ź���			
	return temp;	
}
/*******************************************************************************
* Function Name  : AT24C02_WriteOneByte
* Description    : AT24C02_��ȡһ���ֽ�
* Input          : WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ DataToWrite��д����ֽ�
* Return				 : ���ض�ȡ���ֽ�
*******************************************************************************/
uint8_t AT24C02_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite)
{
	uint8_t result;	

	rt_sem_take(sem_eeprom_rw_data, RT_WAITING_FOREVER);//���з����ź������ȴ�������ɡ�	
	
	IIC1_Start();  
	IIC1_Send_Byte(0XA0);   									/*����������ַ0XA0,д����*/
	if( IIC1_Wait_Ack() )
	{
		IIC1_Send_Byte(WriteAddr%256);   				/*���͵͵�ַ*/
		if( IIC1_Wait_Ack() )
		{
			IIC1_Send_Byte(DataToWrite);     			/*�����ֽ�*/							   
			if( IIC1_Wait_Ack() )
			{
				IIC1_Stop();												/*����һ��ֹͣ����*/
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
		
	rt_sem_release(sem_eeprom_rw_data);	//������ɣ��ͷŷ����ź���		
	
	return result;
}
/*******************************************************************************
* Function Name  : AT24C02_Check
* Description    : AT24C02_���
* Input          : null
* Return				 : ����1�ɹ�������0ʧ��
*******************************************************************************/
uint8_t AT24C02_Check(void)
{
	uint8_t temp = 0;
	
	temp=AT24C02_ReadOneByte(255);									/*����ÿ�ο�����дAT24CXX*/			   
	if(temp==0x55)return 1;		   
	else																						/*�ų���һ�γ�ʼ�������*/
	{
		AT24C02_WriteOneByte(255,0x55);
		temp=AT24C02_ReadOneByte(255);	  
		if(temp==0x55)return 1;
	}
	return 0;											  
}
/*******************************************************************************
* Function Name  : AT24C02_Init
* Description    : AT24C02_��ʼ��
* Return				 : ����1�ɹ�������0ʧ��
*******************************************************************************/
uint8_t AT24C02_Init(void)
{
	sem_eeprom_rw_data	=	rt_sem_create("eeprom_rw",1,RT_IPC_FLAG_FIFO);//EEPROM��д�����ź�����ʼ��
	
	IIC1_Init();
	return AT24C02_Check();
}
