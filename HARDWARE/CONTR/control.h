/*

				��16��LEDģ�⿪���������
				



*/

#ifndef __CONTROL_H
#define __CONTROL_H

#include "sys.h"



//���ƴ򿪹رղ����ĺ�
#define  STYLE_KT 1
#define  STYLE_CS 2
#define  STYLE_JH 3
#define  DEVICE_ON 4
#define  CMD_UP 5
#define  CMD_DOWN 6
#define  POWER_1  7
#define  POWER_2  8
#define  MODEL    9


//���ƿ����������dot,���Ƶĵ㣬switch_��1������0����
void control_run (u8 dot,u8 swit );


void control_init (void);



#endif

