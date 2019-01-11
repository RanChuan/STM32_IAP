#include "hmi_usart.h"


#define USART2_RX_ENABLE  *((volatile unsigned long  *)(0x42000000 + (((u32)(&USART2->CR1))*32) + (5*4)))=1
#define USART2_RX_DISABLE  *((volatile unsigned long  *)(0x42000000 + (((u32)(&USART2->CR1))*32) + (5*4)))=0



u8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

static u16 RX2_START=0;		//记录读取结束的位置，实现数据队列
static u16 RET2_START=0;	//接收结束的位置

static u16 RET2_NUM=0;//，	接收到的有效字节数目


//初始化IO 串口2 
//bound:波特率
void HMI_USART2_Init(u32 bound){
    //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART2);  //复位串口2
	 //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.3
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2
   
    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3
		 

   //Usart2 NVIC 配置

		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
		NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

		USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

		USART_Cmd( USART2, DISABLE );								
		USART_Init(USART2, &USART_InitStructure); //初始化串口
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启接收中断
//		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//开启空闲中断
//		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);//开启发送中断
		USART_Cmd(USART2, ENABLE);                    //使能串口 
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


void USART2_IRQHandler(void)                	//串口1中断服务程序
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
	{
		
		/*
							通讯协议今后处理
		
		
		*/
		
		if (RET2_NUM<USART2_REC_LEN)//如果缓冲池没满才接收
		{
			USART2_RX_BUF[RET2_START]= (uint8_t)USART2->DR;//执行读操作自动清零中断标记

			RET2_NUM++;
			RET2_START++;
			if (RET2_START>=USART2_REC_LEN) RET2_START=0;
		}else		//缓冲池不为空，标记为错误
		{
			USART_ClearFlag(USART2, USART_IT_RXNE);//没有执行读操作，需要程序中清除标志位
		}
	}
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  //空闲中断
	{
		RET2_NUM=0;//一旦空闲，清空接收计数器
		USART_ClearFlag(USART2, USART_IT_IDLE);

	}
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)  //发送中断
	{
		USART_ClearFlag(USART2, USART_IT_TXE);
		
	}
	
} 

//----------------------------------------------以下是内部调用的函数----------------------------

//获取Buff的第一个数据
//自动在Buff中循环
//调用之后会清除当前位并自动加一
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
	RET2_NUM--;//每读取一次就减少一个接收数
	USART2_RX_ENABLE;
	
	return temp;

}

//查看Buff当前位的数据
//不会清除当前位，不会自动加一
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

//--------------------------------以上是内部调用的函数--------------------------------


//如果缓冲区的数据不小于需要读取的数据就进入读取循环
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





