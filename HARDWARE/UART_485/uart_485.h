#ifndef __USART2_H
#define __USART2_H
#include "sys.h"
#include "delay.h"
#include "usart.h"
extern u8 RS485_RX_BUF[100]; 		//���ջ���,���64���ֽ�
extern u8 RS485_RX_CNT;   			//���յ������ݳ���

//ģʽ����
#define USART_485_TX_EN		PBout(12)	//485ģʽ����.0,����;1,����.
//����봮���жϽ��գ��벻Ҫע�����º궨��
#define USART_485_RX_EN 	1			//0,������;1,����.




void USART3_Init(u32 bound);
void RS485_Send_Data(u8 *buf,u8 len);
void RS485_Receive_Data(u8 *buf,u8 *len);


//©ˮ������1��������0��������
u8 water_err (void);


#endif
