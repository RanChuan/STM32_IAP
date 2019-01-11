#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
#include "delay.h"
#define USART_REC_LEN  			100  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收


#define  USART_ERR  0x2000
#define  USART_DONE 0x8000



					//阻塞方式发送字符串
u16 USART1_tx_bytes( uint8_t* TxBuffer, uint8_t Length );
					//阻塞方式接收字符串
					//这种方式接收的数据不进入缓冲池
uint8_t USART1_rx_bytes( uint8_t* RxBuffer );


//将接收到的数据流复制到外部内存区
//buff ,外部内存区的首地址，len，想要读取的长度，return 实际读取到时数据流长度
u16 USART1_GetCmd_MODE2_1 (uint8_t *buff,u16 len);



extern u16 (* USART1_GetCmd) (uint8_t *buff,u16 len);
extern void (*RECV_IRQHandler) (void);


						//模式1的串口接收中断
void RECV_MODE1_IRQ (void);



//获取接收状态,1，未完成，0，完成
u16 USART1_GetFlag (void);

void USART1_Init(u32 bound);

#endif


