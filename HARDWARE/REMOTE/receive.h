#include "remote.h"


#define CUPTURE_NUM 500
	extern u16 cupture[CUPTURE_NUM];//在receive.c文件中声明的

void Receive_Clear (void );//接收模式用于清除脉冲计数器

void Receive_GREE (void );//格力接收中断函数


void Receive_GetKey (u8 *buff,u16 num);
u8 Receive_GetState (void);//1,有可读键值，0，没有



void Receive_SaveKey (u8 keytype);

// buff 数据段，空调的数据接收，brand，品牌
void Remote_receive (u8 * buff,u8 brand );
//停止接收
void Remote_stoprec (void  );

void Receive_init (void);


//接收使能
void Receive_Cmd (FunctionalState t);


