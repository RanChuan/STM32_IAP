#ifndef __CRC_CHECK_H__
#define __CRC_CHECK_H__
#include "sys.h"

u8 check_sum(u8* buf,u8 len);  	 	//��У�� �����ֽ�֮��Ϊ0
u8 check_xor(u8* buf, u8 len);		//���У�飬�����ֽ����
u8 check_crc8(u8 *buf,u8 len);		//CRC8У��
u16 check_crc16(u8 *buf,u8 len);	//CRC16У��


#endif
