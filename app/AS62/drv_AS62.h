#ifndef __DRV_AS62_H__
#define __DRV_AS62_H__

#include "delay.h"
#include "usart.h"
//#include "led.h"

#define AUX 		PAin(8)
#define M0			PAout(12)
#define M1			PAout(11)



extern uint8_t AS32_Param_Config[ 6 ];



				//无线初始化，不具备配置无线的功能
void AS32_Param_Init( void );

				//带地址的无线发送，点对点模式
u16 AS32_TX (u16 addr,u8 com,u8 *buff,u16 len);

				//不带地址的无线发送，透传模式
u16 AS32_TX_none (u8 *buff,u16 len);


#endif
