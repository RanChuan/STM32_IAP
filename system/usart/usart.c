#include "usart.h"	  
#include "control.h"
#include "config.h"
#include "drv_as62.h"

/***************************************************************************

��̬ȫ�ֱ���RET_NUM�ڶ�ȡ�������жϺ������в���
�п��ܳ����ڶ�ȡ�����в�����ʱ������жϣ���ʱ��RET_NUM��ֵ�Ͳ�׼ȷ
���統RET_NUM==1��ʱ�����ڽ���RET_NUM--��������ʱ������жϣ�����ִ����RET_NUM++����
����RET_NUMӦ�õ���0������ֵ���ԣ�Ӧ����1����ʱ��ͻ�������ֽ������ݶ�ʧ
��ִ��RET_NUM--������ʱ������ж��ٽ籣������ʱ��ֹ���봮���жϣ�����������

��д����ʱ��Ҫ�����ж��ٽ籣����ȫ�ֱ����У�
		RET_NUM
		USART_RX_BUF[USART_REC_LEN]
		RET_START
		

 *((volatile unsigned long  *)(0x42000000 + (((u32)(&USART1->CR1))*32) + (5*4)))=1
*******************************************************************************/


#define USART1_RX_ENABLE  *((volatile unsigned long  *)(0x42000000 + (((u32)(&USART1->CR1))*32) + (5*4)))=1
#define USART1_RX_DISABLE  *((volatile unsigned long  *)(0x42000000 + (((u32)(&USART1->CR1))*32) + (5*4)))=0


#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
int _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//u8 USART_RX_BUF2[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.

static u16 RX_START=0;		//��¼��ȡ������λ�ã�ʵ�����ݶ���
static u16 RET_START=0;	//���ս�����λ��

static u8 MODE2_IF_RECV=0;//ģʽ2������ɱ�־λ


//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13��	���ճ������������
u16 USART_RX_STA=0;       //����״̬���
static u16 RET_NUM=0;//��	���յ�����Ч�ֽ���Ŀ

static u8 USART1_IDE=1;//����

u16 (* USART1_GetCmd) (uint8_t *buff,u16 len);
void (*RECV_IRQHandler) (void);



//��ʼ��IO ����1 
//bound:������
void USART1_Init(u32 bound){
    //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RECV_IRQHandler=RECV_MODE1_IRQ;				//��ʼ�������жϺ���
	USART1_GetCmd=USART1_GetCmd_MODE2_1;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART1);  //��λ����1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10
		 

   //Usart1 NVIC ����

		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

		USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

		USART_Cmd( USART1, DISABLE );								
		USART_Init(USART1, &USART_InitStructure); //��ʼ������
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���������ж�
		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//���������ж�
//		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);//���������ж�
		USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 
}




u16 USART1_GetFlag (void)
{
			return RET_NUM;

}


//��ȡBuff�ĵ�һ������
//�Զ���Buff��ѭ��
//����֮��������ǰλ���Զ���һ
static u8 Get_BuffData(void)
{
	u8 temp;
	if (RX_START<USART_REC_LEN)
	{
		temp=USART_RX_BUF[RX_START];
	}
	else
	{
		RX_START=0;
		temp=USART_RX_BUF[RX_START];
	}

	USART1_RX_DISABLE;
	USART_RX_BUF[RX_START]=0;
	if (RET_NUM)RET_NUM--;//ÿ��ȡһ�ξͼ���һ��������
	USART1_RX_ENABLE;
	RX_START++;
	
	return temp;
}

//�鿴Buff��ǰλ������
//���������ǰλ�������Զ���һ
static u8 See_BuffData(void)
{
	u8 temp;
	if (RX_START<USART_REC_LEN)
	{
		temp=USART_RX_BUF[RX_START];
	}
	else
	{
		RX_START=0;
		temp=USART_RX_BUF[RX_START];
	}
	return temp;
	
}
static u8 See_BuffNextData(void)
{
	u8 temp;
	u8 i=RX_START+1;
	if (i<USART_REC_LEN)
	{
		temp=USART_RX_BUF[i];
	}
	else
	{
		i=0;
		temp=USART_RX_BUF[i];
	}
	return temp;
	
}


/***********************************************************************************
//���жϽ��յ������Ƴ�
//���жϽ��յ�ʱ����Ҫ�õ������ USART_RX_BUF

�������ܽ��ܼ����������ݣ��ɼ��������ݣ�����������������
�����ȡ�����޷����ּ�����������������������
����ͨѶЭ��õ������ݳ��ȣ�
		�ɼ������������ݹ̶�Ϊ10λ��
		�������������������ݹ̶�Ϊ14λ��
		����������������ݵ����Ǽ����������ģ������������Ч����
		ÿ�λ�ȡ����ʱ��ѯ����֡ͷ����������
		

*************************************************************************************/


			//���ģʽ2�Ĵ��ڶ�ȡ�����Ǻ�ģʽ1���жϺ�������ģ�Ŀǰ�����
u16 USART1_GetCmd_MODE2_1 (uint8_t *buff,u16 len)
{
	u16 getlen=0;//��¼�Ѿ���ȡ�����ֽڸ���
	u16 i=0;//��ʱѭ������
	u16 want_length;
	for (getlen=0;getlen<len;getlen++)
		buff[getlen]=0;
	while(USART1_IDE==0)//�ȴ��ڰ����ݽ�����
	{
		i++;if (i>10000) break;
	}
	if (AUX)
	{
		getlen=0;
		for (i=0;i<USART_REC_LEN;i++)//��ѯ��������������û������
		{
			if (See_BuffData()==0xff&&See_BuffNextData()==0xff)
				break;
			if (See_BuffData()==0xf0&&See_BuffNextData()==0xfe)
				break;
			else
			{
				Get_BuffData();
			}
			
		}
		getlen=0;
		while(RET_NUM)
		{
			buff[getlen] =Get_BuffData();
			getlen++;
			if (getlen==len)
				break;
//			if (See_BuffData()==0xff&&See_BuffNextData()==0xff)
//				break;
//			if (See_BuffData()==0xfe&&See_BuffNextData()==0xf0)
//				break;
		}
	
		if (RET_NUM==0) 
		{
			RX_START=0;							//���ȫ����ȡ�ˣ����¿�ʼ����
			USART1_RX_DISABLE;
			RET_START=0;
			USART1_RX_ENABLE;
		}
	}
		return getlen;
	
}





















void RECV_MODE1_IRQ (void)
{
	USART1_IDE=0;	
	if (RET_NUM<USART_REC_LEN)//��������û���Ž���
	{
		USART_RX_BUF[RET_START]= (uint8_t)USART1->DR;//ִ�ж������Զ������жϱ��

		RET_NUM++;
		RET_START++;
		if (RET_START>=USART_REC_LEN) RET_START=0;
	}else		//����ز�Ϊ�գ����Ϊ����
	{
		USART_ClearFlag(USART1, USART_IT_RXNE);//û��ִ�ж���������Ҫ�����������־λ
	}

}




void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 t;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
	{
		
		RECV_IRQHandler();//���ô��ڽ����жϺ���
	}
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  //�����ж�
	{
		USART1_IDE=1;
		t=USART1->SR;
		t=USART1->DR;

	}
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)  //�����ж�
	{
		USART_ClearFlag(USART1, USART_IT_TXE);
		
	}
	
} 
#endif

u16 USART1_tx_bytes( uint8_t* TxBuffer, uint8_t Length )
{

	if (!Length) return 0;

	while( Length-- )
	{
		while( RESET == USART_GetFlagStatus( USART1, USART_FLAG_TXE ));
		USART1->DR = *TxBuffer;
		TxBuffer++;
	}
}



uint8_t USART1_rx_bytes( uint8_t* RxBuffer )
{
	uint8_t l_RxLength = 0;
	uint16_t l_UartRxTimOut = 0x7FFF;
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//�رս����ж�
	while( l_UartRxTimOut-- )			
	{
		if( RESET != USART_GetFlagStatus( USART1, USART_FLAG_RXNE ))
		{
			*RxBuffer = (uint8_t)USART1->DR;
			RxBuffer++;
			l_RxLength++;
			l_UartRxTimOut = 0x7FFF;	
		}
		if( 100 == l_RxLength )
		{
			break;		
		}
	}
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���������ж�
	return l_RxLength;					
}





