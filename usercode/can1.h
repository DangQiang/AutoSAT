/*****************************************************
* File      	:	can1.h
* Brief				:	ģ��ͨ�������ƺ���
* Date           Author       Version
* 2017-05-25     DangQiang    V1.0
******************************************************/
#ifndef __CAN1_H
#define __CAN1_H
 
/* Includes	*/
#include <rtthread.h>
#include "stm32f2xx.h"
#include "process.h"

#define		GROUP_IDENTIFIER		0x480	//PC->��
#define		GLOBAL_IDENTIFIER		0x400	//PC->ȫ��

#define		MSHEL_IDENTIFIER		0x084	//�����豸ID

#define	MID_MsgAck			0x00	//Ӧ��
#define	MID_Param				0x01	//���ò���
#define	MID_Cmd					0x02	//��������
#define	MID_ThgDetect		0x10	//��ѯ���Զ��ش���ʪ��ֵ

//#define		SMFAN_IDENTIFIER		0x085	//��ѹ����
//#define		SMSTA_IDENTIFIER		0x086	//״̬
//#define		SMDOR_IDENTIFIER		0x011	//����
//#define		SMLLT_IDENTIFIER		0x088	//������

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

void can1_thread_entry(void* parameter);//CAN1�����߳�
extern	void CAN1_SendData(rt_uint8_t mid,rt_uint8_t* data_addr,rt_uint8_t length);//CAN1���ݷ���
#endif /* __CAN1_H */

/************************************END OF FILE********************************/
