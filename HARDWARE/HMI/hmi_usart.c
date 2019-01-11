#include "hmi_usart.h"


#define USART2_RX_ENABLE  *((volatile unsigned long  *)(0x42000000 + (((u32)(&USART2->CR1))*32) + (5*4)))=1
#define USART2_RX_DISABLE  *((volatile unsigned long  *)(0x42000000 + (((u32)(&USART2->CR1))*32) + (5*4)))=0



u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.

static u16 RX2_START=0;		//��¼��ȡ������λ�ã�ʵ�����ݶ���
static u16 RET2_START=0;	//���ս�����λ��

static u16 RET2_NUM=0;//��	���յ�����Ч�ֽ���Ŀ


//��ʼ��IO ����2 
//bound:������
void HMI_USART2_Init(u32 bound){
    //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART2);  //��λ����2
	 //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.3
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3
		 

   //Usart2 NVIC ����

		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
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

		USART_Cmd( USART2, DISABLE );								
		USART_Init(USART2, &USART_InitStructure); //��ʼ������
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���������ж�
//		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//���������ж�
//		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);//���������ж�
		USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 
}


void HMI_tx_cmd( uint8_t* TxBuffer )
{

	while( * TxBuffer )
	{
		while( RESET == USART_GetFlagStatus( USART2, USART_FLAG_TXE ));
		USART2->DR = *TxBuffer;
		TxBuffer++;
	}
	while( RESET == USART_GetFlagStatus( USART2, USART_FLAG_TXE ));
	USART2->DR = 0xff;
	while( RESET == USART_GetFlagStatus( USART2, USART_FLAG_TXE ));
	USART2->DR =0xff;
	while( RESET == USART_GetFlagStatus( USART2, USART_FLAG_TXE ));
	USART2->DR = 0xff;

}


void USART2_IRQHandler(void)                	//����1�жϷ������
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�
	{
		
		/*
							ͨѶЭ������
		
		
		*/
		
		if (RET2_NUM<USART2_REC_LEN)//��������û���Ž���
		{
			USART2_RX_BUF[RET2_START]= (uint8_t)USART2->DR;//ִ�ж������Զ������жϱ��

			RET2_NUM++;
			RET2_START++;
			if (RET2_START>=USART2_REC_LEN) RET2_START=0;
		}else		//����ز�Ϊ�գ����Ϊ����
		{
			USART_ClearFlag(USART2, USART_IT_RXNE);//û��ִ�ж���������Ҫ�����������־λ
		}
	}
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  //�����ж�
	{
		RET2_NUM=0;//һ�����У���ս��ռ�����
		USART_ClearFlag(USART2, USART_IT_IDLE);

	}
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)  //�����ж�
	{
		USART_ClearFlag(USART2, USART_IT_TXE);
		
	}
	
} 

//----------------------------------------------�������ڲ����õĺ���----------------------------

//��ȡBuff�ĵ�һ������
//�Զ���Buff��ѭ��
//����֮��������ǰλ���Զ���һ
static u8 Get_BuffData(void)
{
	u8 temp;
	if (RX2_START<USART2_REC_LEN)
	{
		temp=USART2_RX_BUF[RX2_START];
	}
	else
	{
		RX2_START=0;
		temp=USART2_RX_BUF[RX2_START];
	}

	RX2_START++;
	USART2_RX_DISABLE;
	USART2_RX_BUF[RX2_START]=0;
	RET2_NUM--;//ÿ��ȡһ�ξͼ���һ��������
	USART2_RX_ENABLE;
	
	return temp;

}

//�鿴Buff��ǰλ������
//���������ǰλ�������Զ���һ
static u8 See_BuffData(void)
{
	u8 temp;
	if (RX2_START<USART2_REC_LEN)
	{
		temp=USART2_RX_BUF[RX2_START];
	}
	else
	{
		RX2_START=0;
		temp=USART2_RX_BUF[RX2_START];
	}
	return temp;
	
}


static u8 See_BuffNextData(void)
{
	u8 temp;
	u8 i=RX2_START+1;
	if (i<USART2_REC_LEN)
	{
		temp=USART2_RX_BUF[i];
	}
	else
	{
		i=0;
		temp=USART2_RX_BUF[i];
	}
	return temp;
	
}

//--------------------------------�������ڲ����õĺ���--------------------------------


//��������������ݲ�С����Ҫ��ȡ�����ݾͽ����ȡѭ��
u16 HMI_GetCmd (u8 *buff,u16 len)
{
	u16 length=len;
	if (RET2_NUM>=length)
	{
		for (;length;length--)
			{
				*buff=Get_BuffData();
				buff++;
			}
			return len;
	}
	else return 0;
}





