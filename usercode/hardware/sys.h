#ifndef __SYS_H
#define __SYS_H	
#include "stm32f2xx.h"
#include <rtthread.h>
/********************************************************************************/ 	 
//化学发光 系统定义 by:dq 2015.09.09
/********************************************************************************/ 

//IO地址映射
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+0x14)	//ODR偏移0x14  
#define GPIOB_ODR_Addr    (GPIOB_BASE+0x14)  
#define GPIOC_ODR_Addr    (GPIOC_BASE+0x14)  
#define GPIOD_ODR_Addr    (GPIOD_BASE+0x14)  
#define GPIOE_ODR_Addr    (GPIOE_BASE+0x14)  
#define GPIOF_ODR_Addr    (GPIOF_BASE+0x14)     
#define GPIOG_ODR_Addr    (GPIOG_BASE+0x14)     

#define GPIOA_IDR_Addr    (GPIOA_BASE+0x10)  //IDR偏移0x10  
#define GPIOB_IDR_Addr    (GPIOB_BASE+0x10)  
#define GPIOC_IDR_Addr    (GPIOC_BASE+0x10)  
#define GPIOD_IDR_Addr    (GPIOD_BASE+0x10)  
#define GPIOE_IDR_Addr    (GPIOE_BASE+0x10)  
#define GPIOF_IDR_Addr    (GPIOF_BASE+0x10)  
#define GPIOG_IDR_Addr    (GPIOG_BASE+0x10)  
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入



//开放接口
extern	void	ISP_Model_Init(void);	//ISP功能初始化
extern	void	ISP_Handler(void);		//ISP功能处理函数
#endif
