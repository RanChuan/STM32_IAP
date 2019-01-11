/*****************************************************************************


			控制器对集中器发来的命令的解析逻辑
			作者：冉川
			完成时间：2018.3.15


*******************************************************************************/

#ifndef  __CMD_H
#define  __CMD_H
#include "usart.h"
#include "delay.h"
#include "control.h"
#include "remote.h"
#include "crc8_16.h"

//按键长按短按区分
#define PRESS_SHORT 1
#define PRESS_LONG 2
#define PRESS_NONE 0



//模式2中的错误类型定义

#define ERR_SUCCESS         0x0000			//成功
#define ERR_FAIL						0x0001			//未知错误
#define ERR_DATAVALE				0x0002			//数据错误
#define ERR_DATALENGTH			0x0003			//数据长度错误
#define ERR_WANTLENGTH			0x0004			//想要读取的数据长度错误
#define ERR_NULLCMD					0x0005			//不支持的命令类型，只支持06,03
#define ERR_NOCONNECT				0x0006			//没有连接，一切操作都要在通信开始之后
#define ERR_NOCJADDR				0x0007			//采集器的地址不是本机绑定的采集器地址
#define ERR_CANNOTCFG				0x0008			//在不是可配置状态下收到了配置命令
#define ERR_NOTMYTYPE				0x0009			//所要控制的设备与本机不符
#define ERR_TIMEOUT					0x000a  		//超时
#define ERR_FLASHADDR				0x000b			//FLASH操作时地址出错
#define ERR_FLASHWRITE			0x000c			//FLASH写入时包出错





void cmd_0x05 (u8 *buff);
void cmd_return (u8 * buff,u16 err);


void write_flash (u8 *buff);
void read_flash (u8 *buff);
void writting_flash (u8 *buff);


				//执行用户程序
void jump_app(void);






void Get_cmd (u8 *buff);

#endif

