#ifndef __CRC_CHECK_H__
#define __CRC_CHECK_H__
#include "sys.h"

u8 check_sum(u8* buf,u8 len);  	 	//和校验 所有字节之和为0
u8 check_xor(u8* buf, u8 len);		//异或校验，所有字节异或
u8 check_crc8(u8 *buf,u8 len);		//CRC8校验
u16 check_crc16(u8 *buf,u8 len);	//CRC16校验


#endif
