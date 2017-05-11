/*****************************************************
* File			:	eeprom.h
* Brief			:	EEPROM�洢ģ��ͷ�ļ�
* Date           Author       Version
* 2015-01-14     DangQiang    V1.0
******************************************************/#ifndef __EEPROM_H
#define __EEPROM_H
 
/* Includes */
#include "eeprom.h"
#include "sys.h"
#include "delay.h"
	 
/********��������********/ 
#define IIC1_SDA_IN  {GPIOE->MODER&=0xFCFFFFFF;}	/*SDAΪ�����*/
#define IIC1_SDA_OUT {GPIOE->MODER&=0xFCFFFFFF;GPIOE->MODER|=0x01000000;}/*SDAΪ�����*/

#define IIC1_SCL		PEout(11)		/*SCL*/
#define IIC1_SDA		PEout(12)		/*SDA*/ 
#define READ1_SDA		PEin(12)			/*����SDA*/ 

#define iic1_rcc					RCC_AHB1Periph_GPIOE
#define iic1_gpio					GPIOE
#define iic1_scl_pin				(GPIO_Pin_11)
#define iic1_sda_pin				(GPIO_Pin_12)

/********���ݴ洢��ַ����0~254********/

#define PROD_NUM   0	//�������
#define HARD_REV   1	//��λ��Ӳ���汾��


#define X_96_DIFF_L			11	//X96ԭ��ƫ��ֵL
#define X_96_DIFF_H			12	//X96ԭ��ƫ��ֵH
#define Y_96_DIFF_L			13	//Y96ԭ��ƫ��ֵL
#define Y_96_DIFF_H			14	//Y96ԭ��ƫ��ֵH

#define X_384_DIFF_L			15	//X384ԭ��ƫ��ֵL
#define X_384_DIFF_H			16	//X384ԭ��ƫ��ֵH
#define Y_384_DIFF_L			17	//Y384ԭ��ƫ��ֵL
#define Y_384_DIFF_H			18	//Y384ԭ��ƫ��ֵH
//#define X_HOLE_DIST			15	//X�׾�
//#define Y_HOLE_DIST			16	//Y�׾�

//#define PLAT_DIFF				21	//ƽ̨�Ƴ�ƫ��ֵ

//#define WAVE_LOW_L   		33	//�Ͳ�����λ
//#define WAVE_LOW_H   		34	//�Ͳ�����λ
//#define WAVE_HIGH_L  	 	35	//�߲�����λ
//#define WAVE_HIGH_H   	36	//�߲�����λ

#define EXC_DIFF				25   //������ɫ�ǲ�������
#define EMI_DIFF				26   //���䵥ɫ�ǲ�������

#define HEAT_BTM_P			51	//����PID����P���ײ�
#define HEAT_BTM_I			52	//����PID����I
#define HEAT_BTM_D			53	//����PID����D

#define HEAT_TOP_P			54	//����PID����P,����
#define HEAT_TOP_I			55	//����PID����I
#define HEAT_TOP_D			56	//����PID����D

#define	BUBBLE1_VOL			30	//����1��ѹϵ�� 0x1e
#define	BUBBLE2_VOL			31	//����2��ѹϵ��
#define	PRESSU1_VOL			32	//ѹ��1��ѹϵ��
#define	PRESSU2_VOL			33	//ѹ��2��ѹϵ��
#define	PRESSU3_VOL			34	//ѹ��3��ѹϵ��

#define	BUBBLE1_ADJ			35	//����1����ϵ��
#define	BUBBLE2_ADJ			36	//����2����ϵ��
#define	PRESSU1_ADJ			37	//ѹ��1����ϵ��
#define	PRESSU2_ADJ			38	//ѹ��2����ϵ��
#define	PRESSU3_ADJ			39	//ѹ��3����ϵ��

/********���ź����ӿ�********/
uint8_t AT24C02_Init(void);																		/*��ʼ��24C02��IIC�ӿڣ����Լ죬ͨ������1��ʧ�ܷ���0*/
uint8_t AT24C02_ReadOneByte(uint8_t ReadAddr);								/*��AT24C02ָ����ַ����һ������*/
uint8_t AT24C02_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite);	/*��AT24C02ָ����ַд��һ������*/

#endif 

/************************************END OF FILE********************************/
