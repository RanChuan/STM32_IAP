#ifndef __SYSTEM_INIT_H__
#define __SYSTEM_INIT_H__
#include "delay.h"
#include "sys.h"
#include "drv_AS62.h"
#include "usart.h"
#include "control.h"
#include "remote.h"
#include "receive.h"
#include "stm32f10x.h"
#include "hmi_usart.h"
#include "cmd.h"
#include  "uart_485.h"

void System_Init(void);
#endif
