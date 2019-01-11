#include "usart.h"	  
#include "control.h"
#include "config.h"
#include "drv_as62.h"

/***************************************************************************

静态全局变量RET_NUM在读取函数和中断函数都有操作
有可能出现在读取函数中操作的时候进入中断，这时候RET_NUM的值就不准确
例如当RET_NUM==1的时候正在进行RET_NUM--操作，这时候进入中断，并且执行了RET_NUM++操作
最终RET_NUM应该等于0，但是值不对，应该是1；这时候就会出错，出现接收数据丢失
在执行RET_NUM--操作的时候加入中断临界保护，暂时禁止进入串口中断，解决这个问题

在写操作时需要进行中断临界保护的全局变量有：
		RET_NUM
		USART_RX_BUF[USART_REC_LEN]
		RET_START
		

 *((volatile unsigned long  *)(0x42000000 + (((u32)(&USART1->CR1))*32) + (5*4)))=1
*******************************************************************************/


#define USART1_RX_ENABLE  *((volatile unsigned long  *)(0x42000000 + (((u32)(&USART1->CR1))*32) + (5*4)))=1
#define USART1_RX_DISABLE  *((volatile unsigned long  *)(0x42000000 + (((u32)(&USART1->CR1))*32) + (5*4)))=0


#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
int _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

 

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//u8 USART_RX_BUF2[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.

static u16 RX_START=0;		//记录读取结束的位置，实现数据队列
static u16 RET_START=0;	//接收结束的位置

static u8 MODE2_IF_RECV=0;//模式2接收完成标志位


//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13，	接收出错，缓冲池已满
u16 USART_RX_STA=0;       //接收状态标记
static u16 RET_NUM=0;//，	接收到的有效字节数目

static u8 USART1_IDE=1;//空闲

u16 (* USART1_GetCmd) (uint8_t *buff,u16 len);
void (*RECV_IRQHandler) (void);



//初始化IO 串口1 
//bound:波特率
void USART1_Init(u32 bound){
    //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RECV_IRQHandler=RECV_MODE1_IRQ;				//初始化串口中断函数
	USART1_GetCmd=USART1_GetCmd_MODE2_1;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART1);  //复位串口1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10
		 

   //Usart1 NVIC 配置

		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
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

		USART_Cmd( USART1, DISABLE );								
		USART_Init(USART1, &USART_InitStructure); //初始化串口
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启接收中断
		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//开启空闲中断
//		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);//开启发送中断
		USART_Cmd(USART1, ENABLE);                    //使能串口 
}




u16 USART1_GetFlag (void)
{
			return RET_NUM;

}


//获取Buff的第一个数据
//自动在Buff中循环
//调用之后会清除当前位并自动加一
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
	if (RET_NUM)RET_NUM--;//每读取一次就减少一个接收数
	USART1_RX_ENABLE;
	RX_START++;
	
	return temp;
}

//查看Buff当前位的数据
//不会清除当前位，不会自动加一
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
//把中断接收的数据移出
//在中断接收的时候需要用到缓冲池 USART_RX_BUF

控制器能接受集中器的数据，采集器的数据，其他控制器的数据
命令获取函数无法区分集中器和其他控制器的数据
根据通讯协议得到的数据长度：
		采集器发出的数据固定为10位；
		其他控制器发出的数据固定为14位；
		控制器会把所有数据当成是集中器发来的，这样会产生无效数据
		每次获取命令时查询数据帧头解决这个问题
		

*************************************************************************************/


			//这个模式2的串口读取函数是和模式1的中断函数相配的，目前用这个
u16 USART1_GetCmd_MODE2_1 (uint8_t *buff,u16 len)
{
	u16 getlen=0;//记录已经读取到的字节个数
	u16 i=0;//临时循环变量
	u16 want_length;
	for (getlen=0;getlen<len;getlen++)
		buff[getlen]=0;
	while(USART1_IDE==0)//等串口把数据接收完
	{
		i++;if (i>10000) break;
	}
	if (AUX)
	{
		getlen=0;
		for (i=0;i<USART_REC_LEN;i++)//查询整个缓冲区，有没有命令
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
			RX_START=0;							//如果全部读取了，重新开始接收
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
	if (RET_NUM<USART_REC_LEN)//如果缓冲池没满才接收
	{
		USART_RX_BUF[RET_START]= (uint8_t)USART1->DR;//执行读操作自动清零中断标记

		RET_NUM++;
		RET_START++;
		if (RET_START>=USART_REC_LEN) RET_START=0;
	}else		//缓冲池不为空，标记为错误
	{
		USART_ClearFlag(USART1, USART_IT_RXNE);//没有执行读操作，需要程序中清除标志位
	}

}




void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 t;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
	{
		
		RECV_IRQHandler();//调用串口接收中断函数
	}
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  //空闲中断
	{
		USART1_IDE=1;
		t=USART1->SR;
		t=USART1->DR;

	}
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)  //发送中断
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
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//关闭接收中断
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
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启接收中断
	return l_RxLength;					
}





