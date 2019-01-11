#ifndef __HMI_USART_H
#define __HMI_USART_H


#include "sys.h"


#define USART2_REC_LEN 50


void HMI_USART2_Init(u32 bound);

void HMI_tx_cmd( uint8_t* TxBuffer );



#endif