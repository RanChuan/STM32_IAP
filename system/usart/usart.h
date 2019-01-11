#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
#include "delay.h"
#define USART_REC_LEN  			100  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����


#define  USART_ERR  0x2000
#define  USART_DONE 0x8000



					//������ʽ�����ַ���
u16 USART1_tx_bytes( uint8_t* TxBuffer, uint8_t Length );
					//������ʽ�����ַ���
					//���ַ�ʽ���յ����ݲ����뻺���
uint8_t USART1_rx_bytes( uint8_t* RxBuffer );


//�����յ������������Ƶ��ⲿ�ڴ���
//buff ,�ⲿ�ڴ������׵�ַ��len����Ҫ��ȡ�ĳ��ȣ�return ʵ�ʶ�ȡ��ʱ����������
u16 USART1_GetCmd_MODE2_1 (uint8_t *buff,u16 len);



extern u16 (* USART1_GetCmd) (uint8_t *buff,u16 len);
extern void (*RECV_IRQHandler) (void);


						//ģʽ1�Ĵ��ڽ����ж�
void RECV_MODE1_IRQ (void);



//��ȡ����״̬,1��δ��ɣ�0�����
u16 USART1_GetFlag (void);

void USART1_Init(u32 bound);

#endif


