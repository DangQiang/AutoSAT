/*****************************************************
* File			:	eeprom.h
* Brief			:	EEPROM存储模块头文件
* Date           Author       Version
* 2015-01-14     DangQiang    V1.0
******************************************************/#ifndef __EEPROM_H
#define __EEPROM_H
 
/* Includes */
#include "eeprom.h"
#include "sys.h"
#include "delay.h"
	 
/********引脚配置********/ 
#define IIC1_SDA_IN  {GPIOE->MODER&=0xFCFFFFFF;}	/*SDA为输入口*/
#define IIC1_SDA_OUT {GPIOE->MODER&=0xFCFFFFFF;GPIOE->MODER|=0x01000000;}/*SDA为输出口*/

#define IIC1_SCL		PEout(11)		/*SCL*/
#define IIC1_SDA		PEout(12)		/*SDA*/ 
#define READ1_SDA		PEin(12)			/*输入SDA*/ 

#define iic1_rcc					RCC_AHB1Periph_GPIOE
#define iic1_gpio					GPIOE
#define iic1_scl_pin				(GPIO_Pin_11)
#define iic1_sda_pin				(GPIO_Pin_12)

/********数据存储地址定义0~254********/

#define PROD_NUM   0	//仪器编号
#define HARD_REV   1	//下位机硬件版本号


#define X_96_DIFF_L			11	//X96原点偏移值L
#define X_96_DIFF_H			12	//X96原点偏移值H
#define Y_96_DIFF_L			13	//Y96原点偏移值L
#define Y_96_DIFF_H			14	//Y96原点偏移值H

#define X_384_DIFF_L			15	//X384原点偏移值L
#define X_384_DIFF_H			16	//X384原点偏移值H
#define Y_384_DIFF_L			17	//Y384原点偏移值L
#define Y_384_DIFF_H			18	//Y384原点偏移值H
//#define X_HOLE_DIST			15	//X孔距
//#define Y_HOLE_DIST			16	//Y孔距

//#define PLAT_DIFF				21	//平台推出偏移值

//#define WAVE_LOW_L   		33	//低波长低位
//#define WAVE_LOW_H   		34	//低波长高位
//#define WAVE_HIGH_L  	 	35	//高波长低位
//#define WAVE_HIGH_H   	36	//高波长高位

#define EXC_DIFF				25   //激发单色仪波长矫正
#define EMI_DIFF				26   //发射单色仪波长矫正

#define HEAT_BTM_P			51	//加热PID参数P，底部
#define HEAT_BTM_I			52	//加热PID参数I
#define HEAT_BTM_D			53	//加热PID参数D

#define HEAT_TOP_P			54	//加热PID参数P,顶部
#define HEAT_TOP_I			55	//加热PID参数I
#define HEAT_TOP_D			56	//加热PID参数D

#define	BUBBLE1_VOL			30	//气泡1分压系数 0x1e
#define	BUBBLE2_VOL			31	//气泡2分压系数
#define	PRESSU1_VOL			32	//压力1分压系数
#define	PRESSU2_VOL			33	//压力2分压系数
#define	PRESSU3_VOL			34	//压力3分压系数

#define	BUBBLE1_ADJ			35	//气泡1矫正系数
#define	BUBBLE2_ADJ			36	//气泡2矫正系数
#define	PRESSU1_ADJ			37	//压力1矫正系数
#define	PRESSU2_ADJ			38	//压力2矫正系数
#define	PRESSU3_ADJ			39	//压力3矫正系数

/********开放函数接口********/
uint8_t AT24C02_Init(void);																		/*初始化24C02的IIC接口，并自检，通过返回1，失败返回0*/
uint8_t AT24C02_ReadOneByte(uint8_t ReadAddr);								/*在AT24C02指定地址读出一个数据*/
uint8_t AT24C02_WriteOneByte(uint8_t WriteAddr,uint8_t DataToWrite);	/*在AT24C02指定地址写入一个数据*/

#endif 

/************************************END OF FILE********************************/
