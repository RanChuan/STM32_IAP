#ifndef _crc16_H
#define _crc16_H
#include "stm32f10x.h"
void Get_Crc16(u8 *puchMsg,u16 usDataLen,u8 result[]);
u8 Get_Crc8(u8 *ptr,u16 len);
#endif
