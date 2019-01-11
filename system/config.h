#ifndef  __CONFIG_H
#define  __CONFIG_H
#include "stm32f10x.h"
#include "stmflash.h"
/**************************************************

ϵͳ�����ļ�
���ÿ��������͡���������ַ



*************************************************/
#define MYSTYLE_KT 0x02					//�յ�
#define MYSTYLE_CS 0x03					//��ʪ��
#define MYSTYLE_JH 0x04					//����������




				//���ȫ�ֱ������鶨����stmflash.c�ļ���
				//��������������鱣����ϵͳ��������Ϣ������������
				//�����������������Ҫ������flash���stmflash.c�ļ�����
extern u16 MY_CONFIG[100];//������Ϣ


				//Ϊ��ʹ�÷��㣬�ú����

#define MY_ADDR MY_CONFIG[0]

#define MY_STYLE MY_CONFIG[1]

#define KT_STATE MY_CONFIG[2]

#define KT_STATE1 MY_CONFIG[5]//�յ�������λ

#define CS_STATE MY_CONFIG[3]

#define JH_STATE MY_CONFIG[4]



#define MY_MODEL MY_CONFIG[6]  //��¼��ǰ����ģʽ,����ģʽ������������ģʽ

#define KT_STYLE MY_CONFIG[7]		//�յ����ͣ�������������

#define WORK_MODE MY_CONFIG[8]		//����ģʽ��������ģʽ�������ݽ���ģʽ

#define CJ_ADDR   MY_CONFIG[9]		//�����ɼ����ĵ�ַ����Ϊ������u16����������Ķ�

#define IF_WORK   MY_CONFIG[10]  //�Ƿ��ڹ�����1���ڹ�����0��û����

#define IF_HAND   MY_CONFIG[11]    //�ֶ�ģʽ�����Զ�ģʽ��1���ֶ���2���Զ�



#define WARN_WATER MY_CONFIG[12]   //©ˮ����

#define WARN_WENDU MY_CONFIG[13]   //�¶ȱ���

#define WARN_SHIDU MY_CONFIG[14]   //ʪ�ȱ���
#define WARN_TVOC 	MY_CONFIG[15]   //TVOC����
#define WARN_PM2_5 MY_CONFIG[16]   //PM2.5����



									

												//���������������ޣ���������������û��С������

#define LIMIT_WENDU_H		MY_CONFIG[17]			//�¶����ޣ�
#define LIMIT_WENDU_L		MY_CONFIG[18]			//�¶�����
#define LIMIT_SHIDU_H		MY_CONFIG[19]			//ʪ������
#define LIMIT_SHIDU_L   MY_CONFIG[20]			//ʪ������
#define LIMIT_TVOC			MY_CONFIG[21]			//TVOCֻ������
#define LIMIT_PM2_5			MY_CONFIG[22]			//PM2_5ֻ������



#define CS_IN_CD				MY_CONFIG[23]			//��ʪ������CD״̬�����ɿ���


#define KT_USER_CMD    MY_CONFIG[24]	//����ʱ�Ŀյ��Զ���ĺ���������½��µ�






#define WORK_MODE1 0x01					//��������ģʽ
#define WORK_MODE2 0x02					//���ݽ���ģʽ


void Config_Init(void);

//���ñ������������ͣ�����0xffʧ��,0�ɹ�
u8 Set_MyStyle (u8 style);

u8 Set_CJ_Addr (u16 addr);

void Set_Addr (u8 addr);
#endif

