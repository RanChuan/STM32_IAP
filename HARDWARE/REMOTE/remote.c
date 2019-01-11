#include "remote.h"
#include "receive.h"
#include "stmflash.h"
//���ⷢ��ʵ���ļ�


static u8  TIME_BUZZ=0;//æ��־��0��æ��1æ
static vu16 TIME_US=0;//��¼�ж��ٸ�10us��ÿ���жϾ�����һ��10us
static u16 RETOME_TIMES=0;//���͵��������,����λ�Ĵ���
static u8  BUFF_DATA[9];//��Ҫ���͵�����ͷ��ַ




															//���ⷢ���жϺ���ָ��
static void 	(*Remote_IRQHandler) (void);
void 	Remote_GREE (void );//�����յ��ı����жϺ���
void 	Remote_MIDEA (void);//���Ŀյ��ı����жϺ���
void  Remote_USER  (void);//�û���ѧϰ�ı����жϺ���






/*
	��ʱ��1�����ز���ȵ��ƣ���ʱ��3����pwm�ز����ɣ���ʱ��2���ڲ�������ź�
	
*/

void Remote_init (u16 hz)
{
	u16 arr=72000000/(hz)-1;//����������ز�Ƶ�ʼ������Ƶϵ��
	u16 psc=0;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//����ָ���ʼ��
	Remote_IRQHandler=Remote_GREE;//��ʼ���ж�ָ�룬�����յ�
	
	//ʱ�ӳ�ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  , ENABLE);  
	
	//������ų�ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //TIM_CH4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	

/**********************************************************************************************	
	
																		��ʱ��3��ʼ��

***************************************************************************************************/
	//�ز����ö�ʱ��
//��ʱ��3��ͨ��4��PB1

	//psc=0����Ƶ PWMƵ��=72000000/��arr+1��=Khz
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
	
	//ͨ��4 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //�������ģʽ
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //��Ե����ģʽ
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 
	TIM_OCInitStructure.TIM_Pulse = (arr+1)/3;//ռ�ձ�1/3
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);  

	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  //�Զ�����
	TIM_Cmd(TIM3, DISABLE); 
//	TIM_Cmd(TIM3, ENABLE); 
	
/***************************************************************************************	
								
													��ʱ��1��ʼ��
									
				//�Զ���װ��Ϊ720-1��ÿ10us����1���ж�
				//���ж�����������峤�ȼ���
****************************************************************************************/
	//ͨѶ��ʽ���붨ʱ��������Ϣ

	TIM_TimeBaseStructure.TIM_Period = 720-1; //72000000/720��ÿ10us��������һ
	TIM_TimeBaseStructure.TIM_Prescaler =0; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 

	//��ʱ��1�����������ж�
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);  
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM1, DISABLE); 
//	TIM_Cmd(TIM1, ENABLE); 

	
}



void Remote_Start (void )
{
	u16 temp=GPIOB->CRL&0xffffff3f;
	GPIOB->CRL=temp|0x00000080;//�üĴ�����ʽ����PB1Ϊ�������
	TIM_Cmd(TIM3, ENABLE); //�򿪶�ʱ������ʼ�����ز�����
}

void Remote_End (void )
{
	TIM_Cmd(TIM3, DISABLE); 
	u16 temp=GPIOB->CRL&0xffffff3f;
	GPIOB->CRL=temp|0x00000000;//�üĴ�����ʽ����PB1Ϊͨ��IO
	PBout(1)=0;//��Ϊ��
}


//æ�ж�
u8 Remote_GetState (void )
{
	return TIME_BUZZ;
}

//����buff�еĿյ�ң�ؼ�ֵ,

void Remote_send (u8* buff,u8 brand)
{
	u8 i=0;
	for (i=0;i<9;i++)
	BUFF_DATA[i]=buff[i];
	TIME_US=0;
	TIME_BUZZ=1;//��Ϊæ
	TIM_SetCounter(TIM1,  0);//ÿ�ζ��Ǵ�0��ʼ����
	RETOME_TIMES=0;//ÿ�ο�ʼ����������
	if (brand==GREE)
	{
		Remote_IRQHandler=Remote_GREE;
	}else if (brand==MIDEA)
	{
		Remote_IRQHandler=Remote_MIDEA;
	}
	else
	{
		return;
	}
	
	TIM_Cmd(TIM1, ENABLE);  //���峤�ȼ�����ʼ
	Remote_Start(); //��ʼPWM���
	
}


//�Զ����ֵ���ͣ�1������2���أ�3�����£�4������
void Remote_send_USER (u8 keytype)
{	
	u16 i=0;
//	extern u16 cupture[200];//��receive.c�ļ���������
	Receive_Cmd(DISABLE);//����֮ǰ�رս���
		for (i=0;i<200;i++)
			cupture[i]=0;
	if (keytype==1)
	{
		Load_Key_On();
		
	}
	else if (keytype==2)
	{
		Load_Key_Off();
	}
	else if (keytype==3)
	{
		Load_Key_Up();
	}
	else if (keytype==4)
	{
		Load_Key_Down();
	}
	else if (keytype==5)
	{
	}
	TIME_US=0;
	TIME_BUZZ=1;//��Ϊæ
	TIM_SetCounter(TIM1,  0);//ÿ�ζ��Ǵ�0��ʼ����
	RETOME_TIMES=0;//ÿ�ο�ʼ����������
	Remote_IRQHandler=Remote_USER;
	TIM_Cmd(TIM1, ENABLE);  //���峤�ȼ�����ʼ
//	Remote_Start(); //��ʼPWM���
}



void Remote_USER (void)
{
	if (cupture[RETOME_TIMES]&&(cupture[RETOME_TIMES]!=0xffff))
	{
		TIME_US++;
		if (TIME_US>=cupture[RETOME_TIMES])//
		{
			if (RETOME_TIMES%2==0)//1,��PWM
				Remote_Start();//PWM
			else     //0.��PWM
				Remote_End();//��PWM
			RETOME_TIMES++;//������һ������
			TIME_US=0;//��ռ�ʱ������ʼ��һ�μ���
		}

	}
	else//���ͽ���
	{
		TIME_BUZZ=0;//���æ��־
		TIM_Cmd(TIM1, DISABLE); //������ɣ��ر�����
		Remote_End();//��PWM
//		Receive_Cmd(DISABLE);//���ͽ���
	}
//	if (RETOME_TIMES>=400)
//	{
//		TIME_BUZZ=0;//���æ��־
//		TIM_Cmd(TIM1, DISABLE); //������ɣ��ر�����
//		Remote_End();//��PWM
//	}
}


void Remote_MIDEA (void)//���Ŀյ�
{
	static u8 is_hight=0;//0��Ӧ�÷��͸ߵ�ƽ��1��Ӧ�÷��͵͵�ƽ
	u8 bit=0;//��ǰӦ�÷��͵���1����0��
	TIME_US++;
	if ((RETOME_TIMES==0)		//���͵�һ��
			||RETOME_TIMES==53)		//���͵ڶ���
	{
		if (TIME_US>=440)//������4.4ms�ߵ�ƽ
		{
			RETOME_TIMES++;//������һ������
			Remote_End();//��PWM
			TIME_US=0;//��ռ�ʱ������ʼ��һ�μ���
		}
	}
	else if ((RETOME_TIMES==1)
			||(RETOME_TIMES==54))
	{
		if (TIME_US>=440)//������4.4ms�͵�ƽ���
		{
			Remote_Start();//��PWM
			RETOME_TIMES++;
			is_hight=0;
			TIME_US=0;//��ռ�ʱ������ʼ��һ�μ���
		}
		
	}
	else if ((RETOME_TIMES>=2&&RETOME_TIMES<=50)//�����﷢�͵�һ�����ݶ�6���ֽ�48λ
				||(RETOME_TIMES>=55&&RETOME_TIMES<=103))//�����﷢�͵�2�����ݶ�6���ֽ�48λ
	{
		if (is_hight)//���͵͵�ƽ
		{
			if (RETOME_TIMES<55)
				bit=(BUFF_DATA[(RETOME_TIMES-3)/8]>>(7-(RETOME_TIMES-3)%8))&0x01;//
			else
				bit=(BUFF_DATA[(RETOME_TIMES-56)/8]>>(7-(RETOME_TIMES-56)%8))&0x01;//
			if (bit) //����1
			{
				if (TIME_US>=164)
				{
					if ((RETOME_TIMES==50)||(RETOME_TIMES==103))

					{						
						RETOME_TIMES++;
					}
					Remote_Start();//��PWM
					TIME_US=0;//��ռ�ʱ������ʼ��һ�μ���
					is_hight=0;//�͵�ƽ������ɣ���һ�η��͸ߵ�ƽ
				}
				
			}else//����0
			{
				if (TIME_US>=54)
				{
					if ((RETOME_TIMES==50)||(RETOME_TIMES==103))

					{						
						RETOME_TIMES++;
					}
					Remote_Start();//��PWM
					TIME_US=0;//��ռ�ʱ������ʼ��һ�μ���
					is_hight=0;//�͵�ƽ������ɣ���һ�η��͸ߵ�ƽ
				}
				
			}
		}else  //���͸ߵ�ƽ
		{
			if (TIME_US>=65)//����������1����0���ߵ�ƽʱ������650ms
			{
				Remote_End();//��PWM
				RETOME_TIMES++;
				is_hight=1;//�ߵ�ƽ���ͽ�������һ�η��͵͵�ƽ
				TIME_US=0;//��ռ�ʱ������ʼ��һ�μ���
			}
		}
	}
	else if (RETOME_TIMES==51)//���ͺ������ݵļ��S
	{
		if (TIME_US>=54)//0.54ms�ߵ�ƽ
		{
			Remote_End();//��PWM
			RETOME_TIMES++;
			TIME_US=0;
		}
	}
	else if (RETOME_TIMES==52)
	{
		if (TIME_US>=522)//5.22ms�͵�ƽ
		{
			Remote_Start();//��PWM
			RETOME_TIMES++;
			TIME_US=0;
		}
	}
	else if (RETOME_TIMES==104)//�������
	{
		if (TIME_US>=54)//0.54ms�ߵ�ƽ
		{
			Remote_End();//��PWM
			TIME_BUZZ=0;//���æ��־
			TIM_Cmd(TIM1, DISABLE); //������ɣ��ر�����
			Remote_End();
		}
		
	}
			
	
}






//���ж��п����������ͣ
//�ߵ�ƽ������ʱ������
//�͵�ƽ��ʱ��ʱ������
//���Ǹ����յ��Ŀ���Э��
void Remote_GREE (void )

{
	static u8 is_hight=0;//0��Ӧ�÷��͸ߵ�ƽ��1��Ӧ�÷��͵͵�ƽ
	u8 bit=0;//��ǰӦ�÷��͵���1����0��
	TIME_US++;
	if (RETOME_TIMES==0)
	{
		if (TIME_US>=900)//������9ms
		{
			RETOME_TIMES++;//������һ������
			Remote_End();//��PWM
			TIME_US=0;//��ռ�ʱ������ʼ��һ�μ���
		}
	}
	else if (RETOME_TIMES==1)
	{
		if (TIME_US>=450)//��������
		{
			Remote_Start();//��PWM
			RETOME_TIMES++;
			is_hight=0;
			TIME_US=0;//��ռ�ʱ������ʼ��һ�μ���
		}
		
	}
	else if (RETOME_TIMES>=2&&RETOME_TIMES<=38)//�����﷢�͵�һ�����ݶ�
	{
		if (is_hight)//���͵͵�ƽ
		{
			bit=(BUFF_DATA[(RETOME_TIMES-3)/8]>>(7-(RETOME_TIMES-3)%8))&0x01;//
			if (bit) //����1
			{
				if (TIME_US>=164)
				{
					if (RETOME_TIMES!=38)
						Remote_Start();//��PWM
					else 
						RETOME_TIMES++;
					TIME_US=0;//��ռ�ʱ������ʼ��һ�μ���
					is_hight=0;//�͵�ƽ������ɣ���һ�η��͸ߵ�ƽ
				}
				
			}else//����0
			{
				if (TIME_US>=54)
				{
					if (RETOME_TIMES!=38)
						Remote_Start();//��PWM
					else 
						RETOME_TIMES++;
					TIME_US=0;//��ռ�ʱ������ʼ��һ�μ���
					is_hight=0;//�͵�ƽ������ɣ���һ�η��͸ߵ�ƽ
				}
				
			}
			
			
		}else  //���͸ߵ�ƽ
		{
			if (TIME_US>=65)//����������1����0���ߵ�ƽʱ������650ms
			{
				Remote_End();//��PWM
				RETOME_TIMES++;
				is_hight=1;//�ߵ�ƽ���ͽ�������һ�η��͵͵�ƽ
				TIME_US=0;//��ռ�ʱ������ʼ��һ�μ���
			}
		}
	}
	else if (RETOME_TIMES==39)//���ͺ������ݵļ��
	{
		if (TIME_US>=2000-54)//20ms�͵�ƽ
		{
			Remote_Start();//��PWM
			RETOME_TIMES++;
			TIME_US=0;
		}
	}
	else if (RETOME_TIMES>=40&&RETOME_TIMES<=73)
	{
		if (is_hight)//���͵͵�ƽ
		{												//Ӧ����37
			bit=(BUFF_DATA[(RETOME_TIMES-5)/8]>>(7-(RETOME_TIMES-5)%8))&0x01;//
			if (bit) //����1
			{
				if (TIME_US>=164)
				{
					if (RETOME_TIMES!=73)
						Remote_Start();//��PWM
					else
					{	
						TIME_BUZZ=0;//���æ��־
						TIM_Cmd(TIM1, DISABLE); //������ɣ��ر�����
						Remote_End();
					}
					TIME_US=0;//��ռ�ʱ������ʼ��һ�μ���
					is_hight=0;//�͵�ƽ������ɣ���һ�η��͸ߵ�ƽ
				}
				
			}else//����0
			{
				if (TIME_US>=54)
				{
					if (RETOME_TIMES!=73)
						Remote_Start();//��PWM
					else 
					{
						TIME_BUZZ=0;//���æ��־
						TIM_Cmd(TIM1, DISABLE); //������ɣ��ر�����
					}
					TIME_US=0;//��ռ�ʱ������ʼ��һ�μ���
					is_hight=0;//�͵�ƽ������ɣ���һ�η��͸ߵ�ƽ
				}
				
			}
			
			
		}else  //���͸ߵ�ƽ
		{
			if (TIME_US>=65)//����������1����0���ߵ�ƽʱ������650ms
			{
				Remote_End();//��PWM
				RETOME_TIMES++;
				is_hight=1;//�ߵ�ƽ���ͽ�������һ�η��͵͵�ƽ
				TIME_US=0;//��ռ�ʱ������ʼ��һ�μ���
			}
		}
	}
	
}

//���ж��е��ø���Ʒ�Ʋ�ͬ���жϺ���

void TIM1_UP_IRQHandler (void )
{
	Remote_IRQHandler();
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );  //��������ж�
}



