#ifndef __CONMUNICATION_H
#define __CONMUNICATION_H	
#include "sys.h"
#include "crc_check.h"
#include "usart.h"
#include "delay.h"
#include "lcd_flash.h"
#include "drv_AS62.h"

#define MAXLEN 89
#define MINLEN 6

#define CHECK_SUM		0		//校验和
#define CHECK_XOR		1		//异或校验
#define CHECK_CRC8	2		//CRC8校验
#define CHECK_CRC16	3		//CRC16校验



typedef enum
{
	CRC_OK = 0,
	CRC_FORMAT_ERR,
	CRC_CHECK_ERR,
}my_crc_return;

typedef enum
{
	ARRAY_OK = 0,
	ARRAY_LEN_ERR,
	ARRAY_DATA_ERR,
}array_return;

//类modbus帧定义
typedef  struct
{
	u8 id;						//ID号
	u8 address;				//地址
	u8 fun;						//功能			
	u8 datalen;				//数据长度
	u8 data[80];					//数据
	u16 check_val;		//校验值
}my_frame_typedef;

//类modbus协议管理器
typedef struct
{
	u8 rxbuf[89];					//接收储存
	u8 rxlen;				  //长度
	u8 frame_ture;   		//0代表接收中， 1代表接收完成
	u8 check_mode;   		//校验模式
}my_col_dev_typedef;

extern my_col_dev_typedef my_dev;

my_crc_return Pack_Frame(my_frame_typedef *frame);
array_return Send_Pack_Array(u8 *Array,uint8_t Length);
void Send_ON_Modbus(uint8_t ADDR_1,uint8_t CHAN);
void Send_OFF_Modbus(uint8_t ADDR_1,uint8_t CHAN);
void Send_Red_UPON_Modbus(uint8_t ADDR_1,uint8_t CHAN);
void Send_Red_UPOFF_Modbus(uint8_t ADDR_1,uint8_t CHAN);
void Send_Red_DOWNON_Modbus(uint8_t ADDR_1,uint8_t CHAN);
void Send_Red_DOWNOFF_Modbus(uint8_t ADDR_1,uint8_t CHAN);
void send_Frame(my_frame_typedef *frame,uint8_t *data);
void Send_Pack_Frame(my_frame_typedef *frame);
void modbus_tx_packet(uint8_t *pTxBuff, uint8_t Length );
void modbus_rx_packet(my_frame_typedef *frame);
void Header_Init(void);
#endif
