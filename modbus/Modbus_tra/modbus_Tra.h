#ifndef __MODBUS_TRA_H__
#define __MODBUS_TRA_H__
#include "sys.h"
#include "timer.h"
#include "crc_check.h"
#include "conmunication.h"
void Modbus_Tra(void);
void Pack_Data(my_frame_typedef *frame);
void UnPack_Data(my_frame_typedef *frame);

#endif
