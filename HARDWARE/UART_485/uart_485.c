#include "uart_485.h"
#include "delay.h"
#include "usart.h"
#include "config.h"

#ifdef USART_485_RX_EN   	//如果使能了接收


//接收缓存区 	
u8 RS485_RX_BUF[100];  	//接收缓冲,最大64个字节.
//接收到的数据长度
u8 RS485_RX_CNT=0;   		  
  
void USART3_IRQHandler(void)
{
	u8 res;	    
 
 	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收到数据
	{	  
		res =USART_ReceiveData(USART3); 	//读取接收到的数据
//		USART1->DR = res;
		if(RS485_RX_CNT<100)
		{
			RS485_RX_BUF[RS485_RX_CNT]=res;		//记录接收到的值
			RS485_RX_CNT++;						//接收数据增加1 
		}
	}  											 
} 
#endif	

void USART3_Init(u32 bound)
{  
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA,G时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART2时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //PG9端口配置
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	//PA2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
  GPIO_Init(GPIOB, &GPIO_InitStructure);
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);  

	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3,ENABLE);//复位串口2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3,DISABLE);//停止复位
 
	
 #ifdef USART_485_RX_EN		  	//如果使能了接收
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据长度
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;///奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式

    USART_Init(USART3, &USART_InitStructure); ; //初始化串口
  
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //使能串口2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //从优先级2级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
 
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
   
    USART_Cmd(USART3, ENABLE);                    //使能串口 

 #endif

 USART_485_TX_EN=0;			//默认为接收模式

}
//RS485发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void RS485_Send_Data(u8 *buf,u8 len)
{
	u8 t;
	USART_485_TX_EN=1;			//设置为发送模式
  	for(t=0;t<len;t++)		//循环发送数据
	{		   
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART3,buf[t]);
	}	 
 
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);		
	RS485_RX_CNT=0;	  
	USART_485_TX_EN=0;				//设置为接收模式	
}
//RS485查询接收到的数据
//buf:接收缓存首地址
//len:读到的数据长度
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=RS485_RX_CNT;
	u8 i=0;
	*len=0;				//默认为0
	delay_ms(10);
//	if(rxlen==RS485_RX_CNT&&rxlen)//接收到了数据,且接收完成了
//	{
	if (RS485_RX_CNT>=7)
	{
		for(i=0;i<RS485_RX_CNT;i++)
		{
			buf[i]=RS485_RX_BUF[i];	
		}		
		*len=RS485_RX_CNT;	//记录本次数据长度
		RS485_RX_CNT=0;		//清零
	}
}



//漏水报警，1，报警，0，不报警
u8 water_err (void)
{
	u8 length;
	u8 buff[20]={0};
	buff[0]=0x01;
	buff[1]=0x04;
	buff[2]=0x00;
	buff[3]=0x01;
	buff[4]=0x00;
	buff[5]=0x01;
	buff[6]=0x60;
	buff[7]=0x0a;
	
	RS485_Send_Data(buff,8);
	buff[0]=0;
	RS485_Receive_Data(buff,&length);
	if (buff[0])
	{
		if (!buff[4])
		{
			WARN_WATER=0;//无报警
			return 0;
		}
		else
		{
			WARN_WATER=1;//有漏水报警
			return 1;
		}
	}
	WARN_WATER=2;//没有接漏水报警
	return 2;
	
}
