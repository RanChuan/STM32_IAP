#include "remote.h"


#define CUPTURE_NUM 500
	extern u16 cupture[CUPTURE_NUM];//��receive.c�ļ���������

void Receive_Clear (void );//����ģʽ����������������

void Receive_GREE (void );//���������жϺ���


void Receive_GetKey (u8 *buff,u16 num);
u8 Receive_GetState (void);//1,�пɶ���ֵ��0��û��



void Receive_SaveKey (u8 keytype);

// buff ���ݶΣ��յ������ݽ��գ�brand��Ʒ��
void Remote_receive (u8 * buff,u8 brand );
//ֹͣ����
void Remote_stoprec (void  );

void Receive_init (void);


//����ʹ��
void Receive_Cmd (FunctionalState t);


