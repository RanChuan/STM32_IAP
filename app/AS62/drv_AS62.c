#include "drv_AS62.h"


//模块配置参数数组
//改变模块参数，只需改变参数数组值，然后初始化即可
uint8_t AS32_Param_Config[ 6 ] = { 0xC0, 0x04, 0x34, 0x3A, 0x17, 0x40 };			//定点模式


/**
  * @brief :ASxx模块初始化
  * @param :无
  * @note  :按照默认参数初始化，修改默认参数表即可改变模块初始化参数
  * @retval:
  *        @ASxx_Write_OK 写入成功
  *        @ASxx_Write_ERROR 写入失败
  */
//void AS32_Param_Init( uint8_t AS_P_Config[6] )//通过地址的奇偶选择相应的通道
void AS32_Param_Init( void )//通过地址的奇偶选择相应的通道
{

	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PB,PE端口时钟


	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	 
			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	
	GPIO_Init(GPIOA, &GPIO_InitStructure);	 
			
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	
	
	
	
	
	
//	M0=1;
//	M1=1;
	
	
//	delay_ms(1000);
//USART1_tx_bytes((uint8_t *)AS32_Param_Config, 6 );
//	delay_ms(1000);
	M1=0;
	M0=0;
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	AUX=1;//关闭指示灯，在有数据时无线模块控制这个灯亮

}



u16 AS32_TX (u16 addr,u8 com,u8 *buff,u16 len)
{
	u16 i=1000;

	while (i--&&!AUX) delay_ms(1);
	if (i==0) return -1;
	while( RESET == USART_GetFlagStatus( USART1, USART_FLAG_TXE ));
	USART1->DR = addr>>8;
	while( RESET == USART_GetFlagStatus( USART1, USART_FLAG_TXE ));
	USART1->DR = addr&0xff;
	while( RESET == USART_GetFlagStatus( USART1, USART_FLAG_TXE ));
	USART1->DR = com;

	USART1_tx_bytes((uint8_t *)buff, len );

}



//无线单次发送最长为58字节
u16 AS32_TX_none (u8 *buff,u16 len)

{
	u16 i=1000;

	while (i--&&!AUX) 
		delay_ms(1);
	if (i==0) return -1;

	USART1_tx_bytes((uint8_t *)buff, len );

}





