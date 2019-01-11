#ifndef __USART2_H
#define __USART2_H
#include "sys.h"
#include "delay.h"
#include "usart.h"
extern u8 RS485_RX_BUF[100]; 		//接收缓冲,最大64个字节
extern u8 RS485_RX_CNT;   			//接收到的数据长度

//模式控制
#define USART_485_TX_EN		PBout(12)	//485模式控制.0,接收;1,发送.
//如果想串口中断接收，请不要注释以下宏定义
#define USART_485_RX_EN 	1			//0,不接收;1,接收.




void USART3_Init(u32 bound);
void RS485_Send_Data(u8 *buf,u8 len);
void RS485_Receive_Data(u8 *buf,u8 *len);


//漏水报警，1，报警，0，不报警
u8 water_err (void);


#endif
