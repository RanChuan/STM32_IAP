#include "system_init.h"
#include "config.h"

u32 run_time=0;
extern u8 in_iap;			//�Ƿ���iapģʽ

int main()
{	
	u8 buff[58]={0};

	u32 i=0;
	System_Init();//��ʼ��

	while(1)
	{
		if (AUX) 													//
			USART1_GetCmd (buff,58);				//
		else buff[0]=0;										//
		if (buff[0]!=0x00)								//
		{																	//
			Get_cmd (buff);									//
		}
		delay_ms(5);
		if (in_iap==0)
			run_time++;
		if (run_time*5>5*1000) //����֮���������δ���Ӧ�ó�����ת��Ӧ�ó���
			{jump_app();}
	}
	
}





