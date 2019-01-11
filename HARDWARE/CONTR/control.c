#include "control.h"


#define CONTR_1  PBout(13)//�յ�����
#define CONTR_2  PBout(14)//��ʪ������
#define CONTR_3  PBout(15)//��������������
#define CONTR_4  PAout(5)//�豸������
#define CONTR_5  PAout(6)//�������������
#define CONTR_6  PAout(4)//�������������
#define CONTR_7  PAout(7)//�̵���1
#define CONTR_8  PBout(0)//�̵���2
#define CONTR_9  PAout(0)//ģʽ��






//���������ʼ��
//����������LED���̵���
void control_init (void)
{
	u8 i=0;
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //??PB,PE????
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //??PB,PE????
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		 //????
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO????50MHz

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_0|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;				 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_15|GPIO_Pin_13|GPIO_Pin_14;				 
	GPIO_Init(GPIOB, &GPIO_InitStructure);	 


	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 
	GPIO_Init(GPIOB, &GPIO_InitStructure);	 


	//�ر����еĵƣ��̵���
	for (i=0;i<9;i++)
		control_run (1+i,0);
	
}


void control_run (u8 dot,u8 swit )
{
	u8 switch_=!swit;
	switch (dot)
	{
		case 1:
			CONTR_1=switch_;
			break;
		case 2:
			CONTR_2=switch_;
			break;
		case 3:
			CONTR_3=switch_;
			break;
		case 4:
			CONTR_4=switch_;
			break;
		case 5:
			CONTR_5=switch_;
			break;
		case 6:
			CONTR_6=switch_;
			break;
		case 7:
			CONTR_7=switch_;
			break;
		case 8:
			CONTR_8=switch_;
			break;
		case 9:
			CONTR_9=switch_;
			break;
		default:
			break;
		
	}
}



