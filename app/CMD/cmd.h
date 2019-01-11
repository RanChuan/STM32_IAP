/*****************************************************************************


			�������Լ���������������Ľ����߼�
			���ߣ�Ƚ��
			���ʱ�䣺2018.3.15


*******************************************************************************/

#ifndef  __CMD_H
#define  __CMD_H
#include "usart.h"
#include "delay.h"
#include "control.h"
#include "remote.h"
#include "crc8_16.h"

//���������̰�����
#define PRESS_SHORT 1
#define PRESS_LONG 2
#define PRESS_NONE 0



//ģʽ2�еĴ������Ͷ���

#define ERR_SUCCESS         0x0000			//�ɹ�
#define ERR_FAIL						0x0001			//δ֪����
#define ERR_DATAVALE				0x0002			//���ݴ���
#define ERR_DATALENGTH			0x0003			//���ݳ��ȴ���
#define ERR_WANTLENGTH			0x0004			//��Ҫ��ȡ�����ݳ��ȴ���
#define ERR_NULLCMD					0x0005			//��֧�ֵ��������ͣ�ֻ֧��06,03
#define ERR_NOCONNECT				0x0006			//û�����ӣ�һ�в�����Ҫ��ͨ�ſ�ʼ֮��
#define ERR_NOCJADDR				0x0007			//�ɼ����ĵ�ַ���Ǳ����󶨵Ĳɼ�����ַ
#define ERR_CANNOTCFG				0x0008			//�ڲ��ǿ�����״̬���յ�����������
#define ERR_NOTMYTYPE				0x0009			//��Ҫ���Ƶ��豸�뱾������
#define ERR_TIMEOUT					0x000a  		//��ʱ
#define ERR_FLASHADDR				0x000b			//FLASH����ʱ��ַ����
#define ERR_FLASHWRITE			0x000c			//FLASHд��ʱ������





void cmd_0x05 (u8 *buff);
void cmd_return (u8 * buff,u16 err);


void write_flash (u8 *buff);
void read_flash (u8 *buff);
void writting_flash (u8 *buff);


				//ִ���û�����
void jump_app(void);






void Get_cmd (u8 *buff);

#endif

