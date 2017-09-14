/*****************************************************
* File      	:	can1.h
* Brief				:	模块通信主控制函数
* Date           Author       Version
* 2017-05-25     DangQiang    V1.0
******************************************************/
#ifndef __CAN1_H
#define __CAN1_H
 
/* Includes	*/
#include <rtthread.h>
#include "stm32f2xx.h"
#include "process.h"

#define		GROUP_IDENTIFIER		0x480	//PC->组
#define		GLOBAL_IDENTIFIER		0x400	//PC->全局

#define		MSHEL_IDENTIFIER		0x084	//外罩设备ID

#define	MID_MsgAck			0x00	//应答
#define	MID_Param				0x01	//设置参数
#define	MID_Cmd					0x02	//设置命令
#define	MID_ThgDetect		0x10	//查询或自动回传温湿度值

//#define		SMFAN_IDENTIFIER		0x085	//负压风扇
//#define		SMSTA_IDENTIFIER		0x086	//状态
//#define		SMDOR_IDENTIFIER		0x011	//门锁
//#define		SMLLT_IDENTIFIER		0x088	//照明灯

#define		MASK_MP_Write	0x80
#define		MP_Lamp	0x10
#define		MP_Lock	0x11
#define		MP_ThgAcquisition	0x12
#define		MP_ThgInterval	0x13

#define		LampID_LIG 0
#define		LampID_GRN 1
#define		LampID_YEL 2
#define		LampID_RED 3
#define		LampID_UVA 4

#define		LockID_Lock1	0
#define		LockID_Lock2	1
#define		LockID_Stop		2

void can1_thread_entry(void* parameter);//CAN1控制线程
extern	void CAN1_SendData(rt_uint8_t mid,rt_uint8_t* data_addr,rt_uint8_t length);//CAN1数据发送
#endif /* __CAN1_H */

/************************************END OF FILE********************************/
