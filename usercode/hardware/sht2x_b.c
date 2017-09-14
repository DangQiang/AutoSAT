/*****************************************************
* File			:	sht2x_b.c
* Brief			:	sht2x_b检测模块
* Date           Author       Version
* 2017-05-24     DangQiang    V1.0
******************************************************/
#include "sht2x_b.h"

void SHT2X_B_Soft_Reset(void);//SHT2X_B系统复位
/*******************************************************************************
* Function Name  : SHT2X_B_CRC_Check
* Description    : SHT2X_B_数据校验
* Return         : 2数据CRC校验值
*******************************************************************************/
uint8_t SHT2X_B_CRC_Check(uint8_t data1,uint8_t data2)
{
		uint8_t crc_result,i;
	
		crc_result = 0;		
		crc_result ^= data1;//第一个数据校验
		for(i=0;i<8;i++)
		{
			if(crc_result&0x80)
			{
				crc_result = (crc_result<<1)^0x31;//P(x)=x^8+x^5+x^4+1 = 100110001
			}
			else
			{
				crc_result = crc_result<<1;
			}
		}
		crc_result ^= data2;//第二个数据校验
		for(i=0;i<8;i++)
		{
			if(crc_result&0x80)
			{
				crc_result = (crc_result<<1)^0x31;
			}
			else
			{
				crc_result = crc_result<<1;
			}
		}	
		return crc_result;
}
/*******************************************************************************
* Function Name  : SHT2X_B_Init
* Description    : SHT2X_B_初始化
* Input          : null
*******************************************************************************/
void SHT2X_B_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		RCC_AHB1PeriphClockCmd(SHT2X_B_GPIO_RCC,ENABLE);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = SHT2X_B_SCL_PIN | SHT2X_B_SDA_PIN;	
		GPIO_Init(SHT2X_B_I2C_PORT, &GPIO_InitStructure);
		
		SHT2X_B_SCL = 1;	//初始化高电平
		SHT2X_B_SDA = 1;	//初始化高电平

		rt_thread_delay(RT_TICK_PER_SECOND/50);	//延时20ms	
		SHT2X_B_Soft_Reset();//复位
		g_sht2x_result[1].ErrorCount = 0;
}
/*******************************************************************************
* Function Name  : SHT2X_B_I2c_Start
* Description    : SHT2X_B_I2c开始时序
* Input          : null
*******************************************************************************/
void SHT2X_B_I2c_Start(void)
{
		SHT2X_B_SCL_OUT;//输出模式
    SHT2X_B_SDA_OUT;//输出模式
	
    SHT2X_B_SDA = 1;
    SHT2X_B_SCL = 1;
    delay_us(10);	
    SHT2X_B_SDA = 0;
    delay_us(10);
    SHT2X_B_SCL = 0;
    delay_us(10);	
}
/*******************************************************************************
* Function Name  : SHT2X_B_I2c_Stop
* Description    : SHT2X_B_I2c停止时序
* Input          : null
*******************************************************************************/
void SHT2X_B_I2c_Stop(void)
{
		SHT2X_B_SCL_OUT;//输出模式
    SHT2X_B_SDA_OUT;//输出模式
	
    SHT2X_B_SDA = 0;
    SHT2X_B_SCL = 0;
    delay_us(10);	
    SHT2X_B_SCL = 1;
    delay_us(10);
    SHT2X_B_SDA = 1;
    delay_us(10);
}
/*******************************************************************************
* Function Name  : SHT2X_B_I2c_Send_Ack
* Description    : SHT2X_B_I2c MCU发送ACK给从设备
* Input          : null
*******************************************************************************/
void SHT2X_B_I2c_Send_Ack(void)
{
		SHT2X_B_SCL_OUT;//输出模式
    SHT2X_B_SDA_OUT;//输出模式

    SHT2X_B_SDA = 0;
    delay_us(10);    
    SHT2X_B_SCL = 1;
    delay_us(20);
    SHT2X_B_SCL = 0;   
    delay_us(10);
    SHT2X_B_SDA = 1;		
}
/*******************************************************************************
* Function Name  : SHT2X_B_I2c_Wait_Ack
* Description    : SHT2X_B_I2c MCU等待从设备应答
* Input          : null
*******************************************************************************/
uint8_t SHT2X_B_I2c_Wait_Ack(void)
{
		uint8_t	sda_state;
	
		SHT2X_B_SCL_OUT;//输出模式
    SHT2X_B_SDA_IN;//输入模式
	
    delay_us(10);   		
    SHT2X_B_SCL = 1;
    delay_us(10);	
		
    if(Bit_SET == SHT2X_B_SDA_STATE()) 
    {
        sda_state = ACK_ERROR;
    }
    else
    {
        sda_state = ACK_OK;
    }	
		
    delay_us(10);		
    SHT2X_B_SCL = 0;

//    delay_us(100);//用于调试时拉开字节间隔
		
		return sda_state;
}
/*******************************************************************************
* Function Name  : SHT2X_B_I2c_Send_NAck
* Description    : SHT2X_B_I2cMCU发送NACK给从设备
* Input          : null
*******************************************************************************/
void SHT2X_B_I2c_Send_NAck(void)
{
		SHT2X_B_SCL_OUT;//输出模式
    SHT2X_B_SDA_OUT;//输出模式

    SHT2X_B_SDA = 1;
    delay_us(10);    
    SHT2X_B_SCL = 1;
    delay_us(20);
    SHT2X_B_SCL = 0;   
    delay_us(10);
    SHT2X_B_SDA = 1;
}
/*******************************************************************************
* Function Name  : SHT2X_B_Read_Byte
* Description    : SHT2X_B读取字节
* Input          : ACK:1发送应答，0不需应答
*******************************************************************************/
uint8_t SHT2X_B_Read_Byte(uint8_t ACK)
{
    uint8_t i, data = 0;

		SHT2X_B_SCL = SHT2X_B_SCL_STATE();
		SHT2X_B_SCL_OUT;//输出模式	
	
    SHT2X_B_SDA = 1;	
    SHT2X_B_SDA_IN;//输入模式

    for(i = 0; i < 8; i++)
    {
      data <<= 1; 
			
			delay_us(10);  		
			SHT2X_B_SCL = 1;
			delay_us(10);	
      if(Bit_SET == SHT2X_B_SDA_STATE()) 
				{data |= 0x01;}  				
			delay_us(10);	   
      SHT2X_B_SCL = 0;  
			delay_us(10); 				
    }
		
		if(ACK)
			SHT2X_B_I2c_Send_Ack();
		else
			SHT2X_B_I2c_Send_NAck();
		
    return (data);
}
/*******************************************************************************
* Function Name  : SHT2X_B_Send_Byte
* Description    : SHT2X_B发送字节
* Input          : data:发送的字节数据
*******************************************************************************/
uint8_t SHT2X_B_Send_Byte(uint8_t data)
{
    uint8_t i, ack;

		SHT2X_B_SCL_OUT;//输出模式
    SHT2X_B_SDA_OUT;//输出模式
    
    for(i = 0; i < 8; i++)
		{
        if(data & 0x80) 
        {
            SHT2X_B_SDA = 1;
        }
        else 
        {
            SHT2X_B_SDA = 0;
        }
 
				delay_us(10);	//10:46添加延时				
        SHT2X_B_SCL = 1;
        delay_us(20);
        SHT2X_B_SCL = 0;			
        delay_us(10);
        
        data <<= 1;
    }
	
		ack = SHT2X_B_I2c_Wait_Ack();

		return (ack);
}
/*******************************************************************************
* Function Name  : SHT2X_B_Soft_Reset
* Description    : SHT2X_B软复位
* Input          : null
*******************************************************************************/
void SHT2X_B_Soft_Reset(void)
{
    SHT2X_B_I2c_Start();	//启动
    SHT2X_B_Send_Byte(I2C_ADR_W);
    SHT2X_B_Send_Byte(SOFT_RESET);
    SHT2X_B_I2c_Stop();		//停止

    rt_thread_delay(RT_TICK_PER_SECOND/50);	//延时20ms
}
/*******************************************************************************
* Function Name  : SHT2X_B_Read_SerialNumber
* Description    : SHT2X_B读取序列号
* Input          : sht2x_data接收缓冲区
*******************************************************************************/
void SHT2X_B_Read_SerialNumber(SHT2X_PARAM *sht2x_data)
{
		uint8_t i,result;
	
    /* Read from memory location 1 */
    SHT2X_B_I2c_Start();
    SHT2X_B_Send_Byte(I2C_ADR_W); //I2C address
    SHT2X_B_Send_Byte(0xFA); //Command for readout on-chip memory
    SHT2X_B_Send_Byte(0x0F); //on-chip memory address
		SHT2X_B_I2c_Start();
		SHT2X_B_Send_Byte(I2C_ADR_R); //I2C address
    sht2x_data->SerialNumber[5] = SHT2X_B_Read_Byte(1); //Read SNB_3
						 SHT2X_B_Read_Byte(1); //Read CRC SNB_3 (CRC is not analyzed)

    sht2x_data->SerialNumber[4] = SHT2X_B_Read_Byte(1); //Read SNB_2
						 SHT2X_B_Read_Byte(1); //Read CRC SNB_2 (CRC is not analyzed)

    sht2x_data->SerialNumber[3] = SHT2X_B_Read_Byte(1); //Read SNB_1
						 SHT2X_B_Read_Byte(1); //Read CRC SNB_1 (CRC is not analyzed)

    sht2x_data->SerialNumber[2] = SHT2X_B_Read_Byte(1); //Read SNB_0
						 SHT2X_B_Read_Byte(0); //Read CRC SNB_0 (CRC is not analyzed)

    SHT2X_B_I2c_Stop();

    /* Read from memory location 2 */
    SHT2X_B_I2c_Start();
    SHT2X_B_Send_Byte(I2C_ADR_W); //I2C address
    SHT2X_B_Send_Byte(0xFC); //Command for readout on-chip memory
    SHT2X_B_Send_Byte(0xC9); //on-chip memory address
    SHT2X_B_I2c_Start();
    SHT2X_B_Send_Byte(I2C_ADR_R); //I2C address
		
    sht2x_data->SerialNumber[1] = SHT2X_B_Read_Byte(1); //Read SNC_1
    sht2x_data->SerialNumber[0] = SHT2X_B_Read_Byte(1); //Read SNC_0
						 SHT2X_B_Read_Byte(1); //Read CRC SNC0/1 (CRC is not analyzed)

    sht2x_data->SerialNumber[7] = SHT2X_B_Read_Byte(1); //Read SNA_1
    sht2x_data->SerialNumber[6] = SHT2X_B_Read_Byte(1); //Read SNA_0
						 SHT2X_B_Read_Byte(0); //Read CRC SNA0/1 (CRC is not analyzed)
 
    SHT2X_B_I2c_Stop();
		
		result = 0;//用于校验传感器是否存在，不存在序列号异或值为0
		for(i=0;i<8;i++)
		{result ^=sht2x_data->SerialNumber[i];}
		if(!result)
		{sht2x_data->Exist_flag = 1;sht2x_data->ErrorCount=0;}//清错误计数，传感器丢失优先级高
		else
		{sht2x_data->Exist_flag = 0;}
}
/*******************************************************************************
* Function Name  : SHT2X_B_Measure_TempHM
* Description    : SHT2X_B读取温度值，主机模式
* Return         : 0:错误值
*******************************************************************************/
float SHT2X_B_Measure_TempHM(void)
{
    float			temp;
    uint8_t		tmp1, tmp2, crc;
    uint16_t	st;
		uint8_t		timeout;
     
    SHT2X_B_I2c_Start();                            
    SHT2X_B_Send_Byte(I2C_ADR_W);
    SHT2X_B_Send_Byte(TRIG_TEMP_MEASUREMENT_HM);

    SHT2X_B_I2c_Start();
    SHT2X_B_Send_Byte(I2C_ADR_R);

    SHT2X_B_SCL_IN;

		timeout = 0;
    while(Bit_RESET == SHT2X_B_SCL_STATE())
    {
        rt_thread_delay(RT_TICK_PER_SECOND/100);//延时10ms,Tmax85ms from datasheet
				timeout ++;
				if(timeout>10)
				{
					g_sht2x_result[1].ErrorCount++;//通信错误
					break;
				}
    }
		
		temp = 0;	//默认值0		
		if(timeout<10)
		{    
			tmp1 = SHT2X_B_Read_Byte(1);
			tmp2 = SHT2X_B_Read_Byte(1);
			crc	 = SHT2X_B_Read_Byte(0);
						SHT2X_B_I2c_Stop();
						SHT2X_B_SCL_OUT;		
			
			if(crc == SHT2X_B_CRC_Check(tmp1,tmp2))//CRC校验    
			{
				st = (tmp1 << 8) | (tmp2 << 0);
				st &= ~0x0003;
				temp = ((float)st * 0.00268127) - 46.85;
			}
			else
			{
				g_sht2x_result[1].ErrorCount++;//CRC校验错误
			}
		}
    return (temp);	  
}
/*******************************************************************************
* Function Name  : SHT2X_B_MeasureHumiHM
* Description    : SHT2X_B读取湿度值，主机模式
* Return         : 0:错误值
*******************************************************************************/
float SHT2X_B_MeasureHumiHM(void)
{
    float			humi;
    uint8_t		tmp1, tmp2, crc;
    uint16_t	srh;
		uint8_t		timeout;	
     
    SHT2X_B_I2c_Start();                            
    SHT2X_B_Send_Byte(I2C_ADR_W);
    SHT2X_B_Send_Byte(TRIG_HUMI_MEASUREMENT_HM);

    SHT2X_B_I2c_Start();
    SHT2X_B_Send_Byte(I2C_ADR_R);

    SHT2X_B_SCL_IN;

		timeout = 0;
    while(Bit_RESET == SHT2X_B_SCL_STATE())
    {
        rt_thread_delay(RT_TICK_PER_SECOND/100);//延时10ms,Tmax29ms from datasheet
				timeout ++;
				if(timeout>5)
				{
					g_sht2x_result[1].ErrorCount++;//通信错误
					break;
				}
    }

		humi = 0;		//默认值0
		if(timeout<5)
		{
			tmp1 = SHT2X_B_Read_Byte(1);
			tmp2 = SHT2X_B_Read_Byte(1);
			crc	 = SHT2X_B_Read_Byte(0);
						SHT2X_B_I2c_Stop();
						SHT2X_B_SCL_OUT;		
			
			if(crc == SHT2X_B_CRC_Check(tmp1,tmp2))//CRC校验    
			{    
				srh = (tmp1 << 8) | (tmp2 << 0);
				srh &= ~0x0003;
				humi = ((float)srh * 0.00190735) - 6;
			}
			else
			{
				g_sht2x_result[1].ErrorCount++;//CRC校验错误			
			}
		}
    return (humi);
}
/*******************************************************************************
* Function Name  : SHT2X_B_Measure
* Description    : SHT2X_B读取温湿度值，主机模式
* Return         : null
*******************************************************************************/
void SHT2X_B_Measure(void)
{
		g_sht2x_result[1].Temperature_HM = SHT2X_B_Measure_TempHM();//读取温度值
		g_sht2x_result[1].Humidity_HM = SHT2X_B_MeasureHumiHM();//读取湿度值
    SHT2X_B_Read_SerialNumber(&g_sht2x_result[1]);//读取序列号
}

