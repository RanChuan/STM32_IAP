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

#define CHECK_SUM		0		//У���
#define CHECK_XOR		1		//���У��
#define CHECK_CRC8	2		//CRC8У��
#define CHECK_CRC16	3		//CRC16У��



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

//��modbus֡����
typedef  struct
{
	u8 id;						//ID��
	u8 address;				//��ַ
	u8 fun;						//����			
	u8 datalen;				//���ݳ���
	u8 data[80];					//����
	u16 check_val;		//У��ֵ
}my_frame_typedef;

//��modbusЭ�������
typedef struct
{
	u8 rxbuf[89];					//���մ���
	u8 rxlen;				  //����
	u8 frame_ture;   		//0��������У� 1����������
	u8 check_mode;   		//У��ģʽ
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
