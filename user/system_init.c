#include "system_init.h"
#include "config.h"

//ϵͳ��ʼ������ʼ��˳��Ҫ�����޸�
void System_Init()
{

	NVIC_Configuration();	 
	delay_init();	    					//��ʱ������ʼ��
	
	
	
	
	USART1_Init(115200);				//RF������Ϣ����
	AS32_Param_Init( );					//����ģ���ʼ��
	fats_init();
}



