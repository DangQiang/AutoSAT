/*****************************************************
* File			:	sht2x_b.c
* Brief			:	sht2x_b���ģ��
* Date           Author       Version
* 2017-05-24     DangQiang    V1.0
******************************************************/
#include "sht2x_b.h"

void SHT2X_B_Soft_Reset(void);//SHT2X_Bϵͳ��λ
/*******************************************************************************
* Function Name  : SHT2X_B_CRC_Check
* Description    : SHT2X_B_����У��
* Return         : 2����CRCУ��ֵ
*******************************************************************************/
uint8_t SHT2X_B_CRC_Check(uint8_t data1,uint8_t data2)
{
		uint8_t crc_result,i;
	
		crc_result = 0;		
		crc_result ^= data1;//��һ������У��
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
		crc_result ^= data2;//�ڶ�������У��
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
* Description    : SHT2X_B_��ʼ��
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
		
		SHT2X_B_SCL = 1;	//��ʼ���ߵ�ƽ
		SHT2X_B_SDA = 1;	//��ʼ���ߵ�ƽ

		rt_thread_delay(RT_TICK_PER_SECOND/50);	//��ʱ20ms	
		SHT2X_B_Soft_Reset();//��λ
		g_sht2x_result[1].ErrorCount = 0;
}
/*******************************************************************************
* Function Name  : SHT2X_B_I2c_Start
* Description    : SHT2X_B_I2c��ʼʱ��
* Input          : null
*******************************************************************************/
void SHT2X_B_I2c_Start(void)
{
		SHT2X_B_SCL_OUT;//���ģʽ
    SHT2X_B_SDA_OUT;//���ģʽ
	
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
* Description    : SHT2X_B_I2cֹͣʱ��
* Input          : null
*******************************************************************************/
void SHT2X_B_I2c_Stop(void)
{
		SHT2X_B_SCL_OUT;//���ģʽ
    SHT2X_B_SDA_OUT;//���ģʽ
	
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
* Description    : SHT2X_B_I2c MCU����ACK�����豸
* Input          : null
*******************************************************************************/
void SHT2X_B_I2c_Send_Ack(void)
{
		SHT2X_B_SCL_OUT;//���ģʽ
    SHT2X_B_SDA_OUT;//���ģʽ

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
* Description    : SHT2X_B_I2c MCU�ȴ����豸Ӧ��
* Input          : null
*******************************************************************************/
uint8_t SHT2X_B_I2c_Wait_Ack(void)
{
		uint8_t	sda_state;
	
		SHT2X_B_SCL_OUT;//���ģʽ
    SHT2X_B_SDA_IN;//����ģʽ
	
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

//    delay_us(100);//���ڵ���ʱ�����ֽڼ��
		
		return sda_state;
}
/*******************************************************************************
* Function Name  : SHT2X_B_I2c_Send_NAck
* Description    : SHT2X_B_I2cMCU����NACK�����豸
* Input          : null
*******************************************************************************/
void SHT2X_B_I2c_Send_NAck(void)
{
		SHT2X_B_SCL_OUT;//���ģʽ
    SHT2X_B_SDA_OUT;//���ģʽ

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
* Description    : SHT2X_B��ȡ�ֽ�
* Input          : ACK:1����Ӧ��0����Ӧ��
*******************************************************************************/
uint8_t SHT2X_B_Read_Byte(uint8_t ACK)
{
    uint8_t i, data = 0;

		SHT2X_B_SCL = SHT2X_B_SCL_STATE();
		SHT2X_B_SCL_OUT;//���ģʽ	
	
    SHT2X_B_SDA = 1;	
    SHT2X_B_SDA_IN;//����ģʽ

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
* Description    : SHT2X_B�����ֽ�
* Input          : data:���͵��ֽ�����
*******************************************************************************/
uint8_t SHT2X_B_Send_Byte(uint8_t data)
{
    uint8_t i, ack;

		SHT2X_B_SCL_OUT;//���ģʽ
    SHT2X_B_SDA_OUT;//���ģʽ
    
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
 
				delay_us(10);	//10:46�����ʱ				
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
* Description    : SHT2X_B��λ
* Input          : null
*******************************************************************************/
void SHT2X_B_Soft_Reset(void)
{
    SHT2X_B_I2c_Start();	//����
    SHT2X_B_Send_Byte(I2C_ADR_W);
    SHT2X_B_Send_Byte(SOFT_RESET);
    SHT2X_B_I2c_Stop();		//ֹͣ

    rt_thread_delay(RT_TICK_PER_SECOND/50);	//��ʱ20ms
}
/*******************************************************************************
* Function Name  : SHT2X_B_Read_SerialNumber
* Description    : SHT2X_B��ȡ���к�
* Input          : sht2x_data���ջ�����
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
		
		result = 0;//����У�鴫�����Ƿ���ڣ����������к����ֵΪ0
		for(i=0;i<8;i++)
		{result ^=sht2x_data->SerialNumber[i];}
		if(!result)
		{sht2x_data->Exist_flag = 1;sht2x_data->ErrorCount=0;}//������������������ʧ���ȼ���
		else
		{sht2x_data->Exist_flag = 0;}
}
/*******************************************************************************
* Function Name  : SHT2X_B_Measure_TempHM
* Description    : SHT2X_B��ȡ�¶�ֵ������ģʽ
* Return         : 0:����ֵ
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
        rt_thread_delay(RT_TICK_PER_SECOND/100);//��ʱ10ms,Tmax85ms from datasheet
				timeout ++;
				if(timeout>10)
				{
					g_sht2x_result[1].ErrorCount++;//ͨ�Ŵ���
					break;
				}
    }
		
		temp = 0;	//Ĭ��ֵ0		
		if(timeout<10)
		{    
			tmp1 = SHT2X_B_Read_Byte(1);
			tmp2 = SHT2X_B_Read_Byte(1);
			crc	 = SHT2X_B_Read_Byte(0);
						SHT2X_B_I2c_Stop();
						SHT2X_B_SCL_OUT;		
			
			if(crc == SHT2X_B_CRC_Check(tmp1,tmp2))//CRCУ��    
			{
				st = (tmp1 << 8) | (tmp2 << 0);
				st &= ~0x0003;
				temp = ((float)st * 0.00268127) - 46.85;
			}
			else
			{
				g_sht2x_result[1].ErrorCount++;//CRCУ�����
			}
		}
    return (temp);	  
}
/*******************************************************************************
* Function Name  : SHT2X_B_MeasureHumiHM
* Description    : SHT2X_B��ȡʪ��ֵ������ģʽ
* Return         : 0:����ֵ
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
        rt_thread_delay(RT_TICK_PER_SECOND/100);//��ʱ10ms,Tmax29ms from datasheet
				timeout ++;
				if(timeout>5)
				{
					g_sht2x_result[1].ErrorCount++;//ͨ�Ŵ���
					break;
				}
    }

		humi = 0;		//Ĭ��ֵ0
		if(timeout<5)
		{
			tmp1 = SHT2X_B_Read_Byte(1);
			tmp2 = SHT2X_B_Read_Byte(1);
			crc	 = SHT2X_B_Read_Byte(0);
						SHT2X_B_I2c_Stop();
						SHT2X_B_SCL_OUT;		
			
			if(crc == SHT2X_B_CRC_Check(tmp1,tmp2))//CRCУ��    
			{    
				srh = (tmp1 << 8) | (tmp2 << 0);
				srh &= ~0x0003;
				humi = ((float)srh * 0.00190735) - 6;
			}
			else
			{
				g_sht2x_result[1].ErrorCount++;//CRCУ�����			
			}
		}
    return (humi);
}
/*******************************************************************************
* Function Name  : SHT2X_B_Measure
* Description    : SHT2X_B��ȡ��ʪ��ֵ������ģʽ
* Return         : null
*******************************************************************************/
void SHT2X_B_Measure(void)
{
		g_sht2x_result[1].Temperature_HM = SHT2X_B_Measure_TempHM();//��ȡ�¶�ֵ
		g_sht2x_result[1].Humidity_HM = SHT2X_B_MeasureHumiHM();//��ȡʪ��ֵ
    SHT2X_B_Read_SerialNumber(&g_sht2x_result[1]);//��ȡ���к�
}

