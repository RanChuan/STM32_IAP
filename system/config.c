#include "config.h"
#include "control.h"
#include "usart.h"
#include "cmd.h"

//����������ϵͳ����

void Set_Addr (u8 addr)
{
	MY_ADDR=addr;
}

//���ñ������������ͣ�����0xffʧ��,0�ɹ�
u8 Set_MyStyle (u8 style)
{
	if(style>4||style<2)
		return 0xff;
	else
		MY_STYLE=style;
	return 0;
}




//���ñ��������ɼ���id������0xffʧ��,0�ɹ�
u8 Set_CJ_Addr (u16 addr)
{
	if ((addr&0xff00)==0xff00||(addr&0x00ff)==0x00ff||(addr==0x0000))
	{
		return 0xff;
	}
	else
		CJ_ADDR=addr;
	return 0;
}




//���ó�ʼ����Ĭ���ǿյ�
void Config_Init(void)
{
	
									//����flash�� ������
	Load_Config( );
	
									//�����ַû�б����ù�����ʼ��Ϊ1
	if (MY_ADDR>=0xffff)
	{
		MY_ADDR=0x01;
	}
	
									//�������������ֵ�ڿ�ѡֵ֮�⣬��ʼ��Ϊ�յ�
	if (MY_STYLE>4||MY_STYLE<2)
	{
		MY_STYLE=MYSTYLE_KT;
	}
	
									//����ģʽ����ȷ����ʼ��Ϊģʽ2
	if (WORK_MODE<=0||WORK_MODE>=3)
	{
		WORK_MODE=WORK_MODE2;					//Ĭ���ǵ�������ģʽ
	}
	
	
													
																	//�����ɼ�����ַ���ԣ���ʼ��Ϊ1
	if ((CJ_ADDR&0xff00)==0xff00||(CJ_ADDR&0x00ff)==0x00ff||(CJ_ADDR==0x0000))
	{
		CJ_ADDR=0x01;
	}
	
	
	
	
//		MY_ADDR=0x03;			//������
//		WORK_MODE=WORK_MODE1;					//�����ã�ǿ��ģʽ2
//		CJ_ADDR=0x02;//������
	
	
	
	
	
							//���ݹ���ģʽ��ʼ��һЩ����
							//���а��������ȡ������ָ�룬�������������ָ��
	if (WORK_MODE==WORK_MODE1)
	{
		RECV_IRQHandler=RECV_MODE1_IRQ;
		USART1_GetCmd=USART1_GetCmd_MODE1;
		Get_cmd=Get_cmd_MODE1;
	}
	else if (WORK_MODE==WORK_MODE2)
	{
		RECV_IRQHandler=RECV_MODE1_IRQ;
				USART1_GetCmd=USART1_GetCmd_MODE2_1;
		Get_cmd=Get_cmd_MODE2;
	}
	
	
			//�������³�ʼ���ı���
	
						//�豸״̬��ȫ��Ϊ��
	KT_STATE=0;
	KT_STATE1=0;
	CS_STATE=0;
	JH_STATE=0;
	
						//����״̬����������
	MY_MODEL=0;
	
						//һЩ����
	IF_WORK=1;
	IF_HAND=2;
	
						//������־λ
	WARN_WATER=0;
	WARN_WENDU=0;
	WARN_SHIDU=0;
	WARN_TVOC=0;
	WARN_PM2_5=0;
	CS_IN_CD=0;
	
}

