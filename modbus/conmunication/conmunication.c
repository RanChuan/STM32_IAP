#include "conmunication.h"

my_col_dev_typedef my_dev;


/*************************************************************************************************************************************************
��һ��ֵΪid�ţ�01��ʾ�նˣ�02��ʾ�ڵ㣬03��ʾ��������00����PC
�ڶ���ֵΪ���ܣ�01��ʾָ�02��ʾ���ݣ�03��ʾ�������
������ֵΪ��ַ�������͵���ָ���ʱ�򣬸õ�ַ��ʾΪĿ���ַ������ʱ���ʾ�����ַ
���ĸ�ֵΪ�������ݳ���
*************************************************************************************************************************************************/
//uint8_t modbus_data[23]			={0x02,0x02,0x00,0x13,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13};
uint8_t modbus_col[4]				={0x02,0x01,0x00,0x01};//�����ֵ����ִ�о���Ĳ���
uint8_t modbus_back[5]			={0x02,0x03,0x00,0x01,0x00};//�����ֵΪEF��ʾ������ȷ��FEΪ���ܴ���


void Send_Frame(my_frame_typedef *frame,uint8_t *data)
{
	u8 i;
	frame->id = data[0];
	frame->fun = data[1];
	frame->address = data[2];
	frame->datalen = data[3];
	for(i=0;i<frame->datalen;i++)
	{
		frame->data[i]=data[i+4];
	}
}


my_crc_return Pack_Frame(my_frame_typedef *frame)//����
{
	u16 rx_check_val =0;
	u16 cal_check_val =0;
	u8 datalen =0;
	frame->datalen=0;
	if(my_dev.rxlen>MAXLEN || my_dev.rxlen<MINLEN)
	{
		my_dev.rxlen=0;
		my_dev.frame_ture=0;
		return CRC_FORMAT_ERR;
	}
	datalen=my_dev.rxbuf[3];
	switch(my_dev.check_mode)
	{
		case CHECK_SUM:							//У���
			cal_check_val=check_sum(my_dev.rxbuf,datalen+4);
			rx_check_val=my_dev.rxbuf[datalen+4];
			break;
		case CHECK_XOR:							//���У��
			cal_check_val=check_xor(my_dev.rxbuf,datalen+4);
			rx_check_val=my_dev.rxbuf[datalen+4];
			break;
		case CHECK_CRC8:						//CRC8У��
			cal_check_val=check_crc8(my_dev.rxbuf,datalen+4);
			rx_check_val=my_dev.rxbuf[datalen+4];
			break;
		case CHECK_CRC16:						//CRC16У��
			cal_check_val=check_crc16(my_dev.rxbuf,datalen+4);
			rx_check_val=((u16)my_dev.rxbuf[datalen+4]<<8)+my_dev.rxbuf[datalen+5];
			break;
	}
	my_dev.rxlen=0;
	my_dev.frame_ture=0;
	if(cal_check_val==rx_check_val)
	{
		frame->id=my_dev.rxbuf[0];//��¼ID
		frame->fun=my_dev.rxbuf[1];//��¼��ַ
		frame->address=my_dev.rxbuf[2];//��¼����
		frame->datalen=my_dev.rxbuf[3];//��¼���ݳ���
		if(frame->datalen)
		{
			for(datalen=0;datalen<frame->datalen;datalen++)
			{
				frame->data[datalen]=my_dev.rxbuf[4+datalen];
			}
		}
		frame->check_val=rx_check_val;//��¼У��ֵ
	}else return CRC_CHECK_ERR;
	return CRC_OK;
}

void Send_Pack_Frame(my_frame_typedef *frame)//����
{
	u8 i;
	u16 cal_check_val=0;
	u8	frame_len=0;
	u8 sendbuf[58];
	if(my_dev.check_mode==CHECK_CRC16)
		frame_len=6+frame->datalen;
	else frame_len=5+frame->datalen;
	sendbuf[0]=frame->id;
	sendbuf[1]=frame->fun;
	sendbuf[2]=frame->address;
	sendbuf[3]=frame->datalen;
	for(i=0;i<frame->datalen;i++)
	{
		sendbuf[4+i]=frame->data[i];
	}
	switch(my_dev.check_mode)
	{
		case CHECK_SUM:
			cal_check_val=check_sum(sendbuf,frame->datalen+4);break;
		case CHECK_XOR:
			cal_check_val=check_xor(sendbuf,frame->datalen+4);break;
		case CHECK_CRC8:
			cal_check_val=check_crc8(sendbuf,frame->datalen+4);break;
		case CHECK_CRC16:
			cal_check_val=check_crc16(sendbuf,frame->datalen+4);break;
	}
	if(my_dev.check_mode==CHECK_CRC16)
	{
		sendbuf[4+frame->datalen]=(cal_check_val>>8)&0xFF;
		sendbuf[5+frame->datalen]=cal_check_val&0xFF;
	}else sendbuf[4+frame->datalen]=cal_check_val&0xFF;
	modbus_tx_packet(sendbuf,frame_len);
}

array_return Send_Pack_Array(u8 *Array,uint8_t Length)
{
	u8 i;
	u16 cal_check_val=0;
	u8	frame_len=0;
	u8 sendbuf[58]={0};
	if(Length>=4)
	{
		if(Array[3] != Length - 4)
				return ARRAY_DATA_ERR;
	}
	else
	{
		return ARRAY_LEN_ERR;
	}
	if(my_dev.check_mode==CHECK_CRC16)
		frame_len=2+Length;
	else frame_len=1+Length;
	for(i=0;i<Length;i++)
	{
		sendbuf[i]=Array[i];
	}
	switch(my_dev.check_mode)
	{
		case CHECK_SUM:
			cal_check_val=check_sum(sendbuf,Length);break;
		case CHECK_XOR:
			cal_check_val=check_xor(sendbuf,Length);break;
		case CHECK_CRC8:
			cal_check_val=check_crc8(sendbuf,Length);break;
		case CHECK_CRC16:
			cal_check_val=check_crc16(sendbuf,Length);break;
	}
	if(my_dev.check_mode==CHECK_CRC16)
	{
		sendbuf[Length]=(cal_check_val>>8)&0xFF;
		sendbuf[Length+1]=cal_check_val&0xFF;
	}else sendbuf[Length]=cal_check_val&0xFF;
	modbus_tx_packet(sendbuf,frame_len);
	return ARRAY_OK;
}


void modbus_tx_packet(uint8_t *pTxBuff, uint8_t Length )
{
	uint8_t Header[5]= { 0x04,0xD1,0x0A,0xAA,0xBB };
	uint8_t Ender[2]= { 0xEE,0xFF };
	if(Cal==1)
	{
		Header[2]=0x01;
	}
	else
	{
		Header[2]=0x02;
	}
	drv_uart_tx_bytes( Header, 5);
	drv_uart_tx_bytes( pTxBuff, Length );
	drv_uart_tx_bytes( Ender, 2 );
}
/***************************************************************************************************
�̵���
����
***************************************************************************************************/
void Send_ON_Modbus(uint8_t ADDR_1,uint8_t CHAN)
{
	u16 cal_check_val=0;
	u8	frame_len=0;
	uint8_t Header[5]= { 0x04,0xD2,0x0A,0xAA,0xBB };
	uint8_t Ender[2]	= { 0xEE,0xFF };
	uint8_t Col_ON_Modbus[7] = {0x03,0x01,0x00,0x01,0xA1,0x00,0x00};
	Header[1]=ADDR_1;
	Header[2]=CHAN;
	Col_ON_Modbus[2]=ADDR_1;
	if(my_dev.check_mode==CHECK_CRC16)
		frame_len=7;
	else frame_len=6;
	switch(my_dev.check_mode)
	{
		case CHECK_SUM:
			cal_check_val=check_sum(Col_ON_Modbus,5);break;
		case CHECK_XOR:
			cal_check_val=check_xor(Col_ON_Modbus,5);break;
		case CHECK_CRC8:
			cal_check_val=check_crc8(Col_ON_Modbus,5);break;
		case CHECK_CRC16:
			cal_check_val=check_crc16(Col_ON_Modbus,5);break;
	}
	if(my_dev.check_mode==CHECK_CRC16)
	{
		Col_ON_Modbus[5]=(cal_check_val>>8)&0xFF;
		Col_ON_Modbus[6]=cal_check_val&0xFF;
	}else Col_ON_Modbus[5]=cal_check_val&0xFF;
	drv_uart_tx_bytes( Header, 5);
	drv_uart_tx_bytes( Col_ON_Modbus, frame_len );
	drv_uart_tx_bytes( Ender, 2 );
}
void Send_OFF_Modbus(uint8_t ADDR_1,uint8_t CHAN)
{
	u16 cal_check_val=0;
	u8	frame_len=0;
	uint8_t Header[5]= { 0x04,0xD3,0x0A,0xAA,0xBB };
	uint8_t Ender[2]= { 0xEE,0xFF };
	uint8_t Col_OFF_Modbus[7] = {0x03,0x01,0x00,0x01,0xA2,0x00,0x00};
	Header[1]=ADDR_1;
	Header[2]=CHAN;
	Col_OFF_Modbus[2]=ADDR_1;
	if(my_dev.check_mode==CHECK_CRC16)
		frame_len=7;
	else frame_len=6;
	switch(my_dev.check_mode)
	{
		case CHECK_SUM:
			cal_check_val=check_sum(Col_OFF_Modbus,5);break;
		case CHECK_XOR:
			cal_check_val=check_xor(Col_OFF_Modbus,5);break;
		case CHECK_CRC8:
			cal_check_val=check_crc8(Col_OFF_Modbus,5);break;
		case CHECK_CRC16:
			cal_check_val=check_crc16(Col_OFF_Modbus,5);break;
	}
	if(my_dev.check_mode==CHECK_CRC16)
	{
		Col_OFF_Modbus[5]=(cal_check_val>>8)&0xFF;
		Col_OFF_Modbus[6]=cal_check_val&0xFF;
	}else Col_OFF_Modbus[5]=cal_check_val&0xFF;
	drv_uart_tx_bytes( Header, 5);
	drv_uart_tx_bytes( Col_OFF_Modbus, frame_len );
	drv_uart_tx_bytes( Ender, 2 );
}

/***************************************************************************************************
����
����
���ؿյ�
***************************************************************************************************/
void Send_Red_UPON_Modbus(uint8_t ADDR_1,uint8_t CHAN)
{
	u16 cal_check_val=0;
	u8	frame_len=0;
	uint8_t Header[5]= { 0x04,0xD2,0x0A,0xAA,0xBB };
	uint8_t Ender[2]	= { 0xEE,0xFF };
	uint8_t Col_ON_Modbus[7] = {0x03,0x01,0x00,0x01,0xB1,0x00,0x00};
	Header[1]=ADDR_1;
	Header[2]=CHAN;
	Col_ON_Modbus[2]=ADDR_1;
	if(my_dev.check_mode==CHECK_CRC16)
		frame_len=7;
	else frame_len=6;
	switch(my_dev.check_mode)
	{
		case CHECK_SUM:
			cal_check_val=check_sum(Col_ON_Modbus,5);break;
		case CHECK_XOR:
			cal_check_val=check_xor(Col_ON_Modbus,5);break;
		case CHECK_CRC8:
			cal_check_val=check_crc8(Col_ON_Modbus,5);break;
		case CHECK_CRC16:
			cal_check_val=check_crc16(Col_ON_Modbus,5);break;
	}
	if(my_dev.check_mode==CHECK_CRC16)
	{
		Col_ON_Modbus[5]=(cal_check_val>>8)&0xFF;
		Col_ON_Modbus[6]=cal_check_val&0xFF;
	}else Col_ON_Modbus[5]=cal_check_val&0xFF;
	drv_uart_tx_bytes( Header, 5);
	drv_uart_tx_bytes( Col_ON_Modbus, frame_len );
	drv_uart_tx_bytes( Ender, 2 );
}
void Send_Red_UPOFF_Modbus(uint8_t ADDR_1,uint8_t CHAN)
{
	u16 cal_check_val=0;
	u8	frame_len=0;
	uint8_t Header[5]= { 0x04,0xD3,0x0A,0xAA,0xBB };
	uint8_t Ender[2]= { 0xEE,0xFF };
	uint8_t Col_OFF_Modbus[7] = {0x03,0x01,0x00,0x01,0xB2,0x00,0x00};
	Header[1]=ADDR_1;
	Header[2]=CHAN;
	Col_OFF_Modbus[2]=ADDR_1;
	if(my_dev.check_mode==CHECK_CRC16)
		frame_len=7;
	else frame_len=6;
	switch(my_dev.check_mode)
	{
		case CHECK_SUM:
			cal_check_val=check_sum(Col_OFF_Modbus,5);break;
		case CHECK_XOR:
			cal_check_val=check_xor(Col_OFF_Modbus,5);break;
		case CHECK_CRC8:
			cal_check_val=check_crc8(Col_OFF_Modbus,5);break;
		case CHECK_CRC16:
			cal_check_val=check_crc16(Col_OFF_Modbus,5);break;
	}
	if(my_dev.check_mode==CHECK_CRC16)
	{
		Col_OFF_Modbus[5]=(cal_check_val>>8)&0xFF;
		Col_OFF_Modbus[6]=cal_check_val&0xFF;
	}else Col_OFF_Modbus[5]=cal_check_val&0xFF;
	drv_uart_tx_bytes( Header, 5);
	drv_uart_tx_bytes( Col_OFF_Modbus, frame_len );
	drv_uart_tx_bytes( Ender, 2 );
}
/*******************************************************************************************/
void Send_Red_DOWNON_Modbus(uint8_t ADDR_1,uint8_t CHAN)
{
	u16 cal_check_val=0;
	u8	frame_len=0;
	uint8_t Header[5]= { 0x04,0xD2,0x0A,0xAA,0xBB };
	uint8_t Ender[2]	= { 0xEE,0xFF };
	uint8_t Col_ON_Modbus[7] = {0x03,0x01,0x00,0x01,0xB3,0x00,0x00};
	Header[1]=ADDR_1;
	Header[2]=CHAN;
	Col_ON_Modbus[2]=ADDR_1;
	if(my_dev.check_mode==CHECK_CRC16)
		frame_len=7;
	else frame_len=6;
	switch(my_dev.check_mode)
	{
		case CHECK_SUM:
			cal_check_val=check_sum(Col_ON_Modbus,5);break;
		case CHECK_XOR:
			cal_check_val=check_xor(Col_ON_Modbus,5);break;
		case CHECK_CRC8:
			cal_check_val=check_crc8(Col_ON_Modbus,5);break;
		case CHECK_CRC16:
			cal_check_val=check_crc16(Col_ON_Modbus,5);break;
	}
	if(my_dev.check_mode==CHECK_CRC16)
	{
		Col_ON_Modbus[5]=(cal_check_val>>8)&0xFF;
		Col_ON_Modbus[6]=cal_check_val&0xFF;
	}else Col_ON_Modbus[5]=cal_check_val&0xFF;
	drv_uart_tx_bytes( Header, 5);
	drv_uart_tx_bytes( Col_ON_Modbus, frame_len );
	drv_uart_tx_bytes( Ender, 2 );
}
void Send_Red_DOWNOFF_Modbus(uint8_t ADDR_1,uint8_t CHAN)
{
	u16 cal_check_val=0;
	u8	frame_len=0;
	uint8_t Header[5]= { 0x04,0xD3,0x0A,0xAA,0xBB };
	uint8_t Ender[2]= { 0xEE,0xFF };
	uint8_t Col_OFF_Modbus[7] = {0x03,0x01,0x00,0x01,0xB3,0x00,0x00};
	Header[1]=ADDR_1;
	Header[2]=CHAN;
	Col_OFF_Modbus[2]=ADDR_1;
	if(my_dev.check_mode==CHECK_CRC16)
		frame_len=7;
	else frame_len=6;
	switch(my_dev.check_mode)
	{
		case CHECK_SUM:
			cal_check_val=check_sum(Col_OFF_Modbus,5);break;
		case CHECK_XOR:
			cal_check_val=check_xor(Col_OFF_Modbus,5);break;
		case CHECK_CRC8:
			cal_check_val=check_crc8(Col_OFF_Modbus,5);break;
		case CHECK_CRC16:
			cal_check_val=check_crc16(Col_OFF_Modbus,5);break;
	}
	if(my_dev.check_mode==CHECK_CRC16)
	{
		Col_OFF_Modbus[5]=(cal_check_val>>8)&0xFF;
		Col_OFF_Modbus[6]=cal_check_val&0xFF;
	}else Col_OFF_Modbus[5]=cal_check_val&0xFF;
	drv_uart_tx_bytes( Header, 5);
	drv_uart_tx_bytes( Col_OFF_Modbus, frame_len );
	drv_uart_tx_bytes( Ender, 2 );
}
