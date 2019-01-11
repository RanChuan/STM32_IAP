#include "remote.h"
#include "receive.h"
#include "stmflash.h"
//红外发送实现文件


static u8  TIME_BUZZ=0;//忙标志，0不忙，1忙
static vu16 TIME_US=0;//记录有多少个10us，每次中断就增加一个10us
static u16 RETOME_TIMES=0;//发送的脉冲次数,数据位的次数
static u8  BUFF_DATA[9];//将要发送的数据头地址




															//红外发送中断函数指针
static void 	(*Remote_IRQHandler) (void);
void 	Remote_GREE (void );//格力空调的编码中断函数
void 	Remote_MIDEA (void);//美的空调的编码中断函数
void  Remote_USER  (void);//用户自学习的编码中断函数






/*
	定时器1用于载波宽度调制，定时器3用于pwm载波生成，定时器2用于捕获红外信号
	
*/

void Remote_init (u16 hz)
{
	u16 arr=72000000/(hz)-1;//根据需求的载波频率计算出分频系数
	u16 psc=0;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//函数指针初始化
	Remote_IRQHandler=Remote_GREE;//初始化中断指针，格力空调
	
	//时钟初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  , ENABLE);  
	
	//输出引脚初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //TIM_CH4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	

/**********************************************************************************************	
	
																		定时器3初始化

***************************************************************************************************/
	//载波配置定时器
//定时器3，通道4，PB1

	//psc=0不分频 PWM频率=72000000/（arr+1）=Khz
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
	
	//通道4 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //中央对齐模式
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //边缘对齐模式
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 
	TIM_OCInitStructure.TIM_Pulse = (arr+1)/3;//占空比1/3
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);  

	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  //自动重载
	TIM_Cmd(TIM3, DISABLE); 
//	TIM_Cmd(TIM3, ENABLE); 
	
/***************************************************************************************	
								
													定时器1初始化
									
				//自动重装载为720-1，每10us产生1次中断
				//在中断里面进行脉冲长度计数
****************************************************************************************/
	//通讯格式编码定时器配置信息

	TIM_TimeBaseStructure.TIM_Period = 720-1; //72000000/720，每10us计数器加一
	TIM_TimeBaseStructure.TIM_Prescaler =0; 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 

	//定时器1计数器更新中断
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
	GPIOB->CRL=temp|0x00000080;//用寄存器方式设置PB1为复用输出
	TIM_Cmd(TIM3, ENABLE); //打开定时器，开始发送载波脉冲
}

void Remote_End (void )
{
	TIM_Cmd(TIM3, DISABLE); 
	u16 temp=GPIOB->CRL&0xffffff3f;
	GPIOB->CRL=temp|0x00000000;//用寄存器方式设置PB1为通用IO
	PBout(1)=0;//设为低
}


//忙判断
u8 Remote_GetState (void )
{
	return TIME_BUZZ;
}

//发送buff中的空调遥控键值,

void Remote_send (u8* buff,u8 brand)
{
	u8 i=0;
	for (i=0;i<9;i++)
	BUFF_DATA[i]=buff[i];
	TIME_US=0;
	TIME_BUZZ=1;//设为忙
	TIM_SetCounter(TIM1,  0);//每次都是从0开始计数
	RETOME_TIMES=0;//每次开始清空脉冲次数
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
	
	TIM_Cmd(TIM1, ENABLE);  //脉冲长度计数开始
	Remote_Start(); //开始PWM输出
	
}


//自定义键值发送，1，开，2，关，3，升温，4，降温
void Remote_send_USER (u8 keytype)
{	
	u16 i=0;
//	extern u16 cupture[200];//在receive.c文件中声明的
	Receive_Cmd(DISABLE);//发送之前关闭接收
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
	TIME_BUZZ=1;//设为忙
	TIM_SetCounter(TIM1,  0);//每次都是从0开始计数
	RETOME_TIMES=0;//每次开始清空脉冲次数
	Remote_IRQHandler=Remote_USER;
	TIM_Cmd(TIM1, ENABLE);  //脉冲长度计数开始
//	Remote_Start(); //开始PWM输出
}



void Remote_USER (void)
{
	if (cupture[RETOME_TIMES]&&(cupture[RETOME_TIMES]!=0xffff))
	{
		TIME_US++;
		if (TIME_US>=cupture[RETOME_TIMES])//
		{
			if (RETOME_TIMES%2==0)//1,开PWM
				Remote_Start();//PWM
			else     //0.关PWM
				Remote_End();//关PWM
			RETOME_TIMES++;//发送了一个脉冲
			TIME_US=0;//清空计时器，开始下一次计数
		}

	}
	else//发送结束
	{
		TIME_BUZZ=0;//清除忙标志
		TIM_Cmd(TIM1, DISABLE); //发送完成，关闭自身
		Remote_End();//关PWM
//		Receive_Cmd(DISABLE);//发送接收
	}
//	if (RETOME_TIMES>=400)
//	{
//		TIME_BUZZ=0;//清除忙标志
//		TIM_Cmd(TIM1, DISABLE); //发送完成，关闭自身
//		Remote_End();//关PWM
//	}
}


void Remote_MIDEA (void)//美的空调
{
	static u8 is_hight=0;//0，应该发送高电平，1，应该发送低电平
	u8 bit=0;//当前应该发送的是1还是0；
	TIME_US++;
	if ((RETOME_TIMES==0)		//发送第一遍
			||RETOME_TIMES==53)		//发送第二遍
	{
		if (TIME_US>=440)//对齐码4.4ms高电平
		{
			RETOME_TIMES++;//发送了一个脉冲
			Remote_End();//关PWM
			TIME_US=0;//清空计时器，开始下一次计数
		}
	}
	else if ((RETOME_TIMES==1)
			||(RETOME_TIMES==54))
	{
		if (TIME_US>=440)//对齐码4.4ms低电平间隔
		{
			Remote_Start();//开PWM
			RETOME_TIMES++;
			is_hight=0;
			TIME_US=0;//清空计时器，开始下一次计数
		}
		
	}
	else if ((RETOME_TIMES>=2&&RETOME_TIMES<=50)//在这里发送第一个数据段6个字节48位
				||(RETOME_TIMES>=55&&RETOME_TIMES<=103))//在这里发送第2个数据段6个字节48位
	{
		if (is_hight)//发送低电平
		{
			if (RETOME_TIMES<55)
				bit=(BUFF_DATA[(RETOME_TIMES-3)/8]>>(7-(RETOME_TIMES-3)%8))&0x01;//
			else
				bit=(BUFF_DATA[(RETOME_TIMES-56)/8]>>(7-(RETOME_TIMES-56)%8))&0x01;//
			if (bit) //发送1
			{
				if (TIME_US>=164)
				{
					if ((RETOME_TIMES==50)||(RETOME_TIMES==103))

					{						
						RETOME_TIMES++;
					}
					Remote_Start();//开PWM
					TIME_US=0;//清空计时器，开始下一次计数
					is_hight=0;//低电平发送完成，下一次发送高电平
				}
				
			}else//发送0
			{
				if (TIME_US>=54)
				{
					if ((RETOME_TIMES==50)||(RETOME_TIMES==103))

					{						
						RETOME_TIMES++;
					}
					Remote_Start();//开PWM
					TIME_US=0;//清空计时器，开始下一次计数
					is_hight=0;//低电平发送完成，下一次发送高电平
				}
				
			}
		}else  //发送高电平
		{
			if (TIME_US>=65)//无论是数字1还是0，高电平时长都是650ms
			{
				Remote_End();//关PWM
				RETOME_TIMES++;
				is_hight=1;//高电平发送结束，下一次发送低电平
				TIME_US=0;//清空计时器，开始下一次计数
			}
		}
	}
	else if (RETOME_TIMES==51)//发送后半段数据的间隔S
	{
		if (TIME_US>=54)//0.54ms高电平
		{
			Remote_End();//关PWM
			RETOME_TIMES++;
			TIME_US=0;
		}
	}
	else if (RETOME_TIMES==52)
	{
		if (TIME_US>=522)//5.22ms低电平
		{
			Remote_Start();//开PWM
			RETOME_TIMES++;
			TIME_US=0;
		}
	}
	else if (RETOME_TIMES==104)//发送完成
	{
		if (TIME_US>=54)//0.54ms高电平
		{
			Remote_End();//关PWM
			TIME_BUZZ=0;//清除忙标志
			TIM_Cmd(TIM1, DISABLE); //发送完成，关闭自身
			Remote_End();
		}
		
	}
			
	
}






//在中断中控制脉冲的启停
//高电平计数到时关脉冲
//低电平计时到时开脉冲
//这是格力空调的控制协议
void Remote_GREE (void )

{
	static u8 is_hight=0;//0，应该发送高电平，1，应该发送低电平
	u8 bit=0;//当前应该发送的是1还是0；
	TIME_US++;
	if (RETOME_TIMES==0)
	{
		if (TIME_US>=900)//对齐码9ms
		{
			RETOME_TIMES++;//发送了一个脉冲
			Remote_End();//关PWM
			TIME_US=0;//清空计时器，开始下一次计数
		}
	}
	else if (RETOME_TIMES==1)
	{
		if (TIME_US>=450)//对齐码间隔
		{
			Remote_Start();//开PWM
			RETOME_TIMES++;
			is_hight=0;
			TIME_US=0;//清空计时器，开始下一次计数
		}
		
	}
	else if (RETOME_TIMES>=2&&RETOME_TIMES<=38)//在这里发送第一个数据段
	{
		if (is_hight)//发送低电平
		{
			bit=(BUFF_DATA[(RETOME_TIMES-3)/8]>>(7-(RETOME_TIMES-3)%8))&0x01;//
			if (bit) //发送1
			{
				if (TIME_US>=164)
				{
					if (RETOME_TIMES!=38)
						Remote_Start();//开PWM
					else 
						RETOME_TIMES++;
					TIME_US=0;//清空计时器，开始下一次计数
					is_hight=0;//低电平发送完成，下一次发送高电平
				}
				
			}else//发送0
			{
				if (TIME_US>=54)
				{
					if (RETOME_TIMES!=38)
						Remote_Start();//开PWM
					else 
						RETOME_TIMES++;
					TIME_US=0;//清空计时器，开始下一次计数
					is_hight=0;//低电平发送完成，下一次发送高电平
				}
				
			}
			
			
		}else  //发送高电平
		{
			if (TIME_US>=65)//无论是数字1还是0，高电平时长都是650ms
			{
				Remote_End();//关PWM
				RETOME_TIMES++;
				is_hight=1;//高电平发送结束，下一次发送低电平
				TIME_US=0;//清空计时器，开始下一次计数
			}
		}
	}
	else if (RETOME_TIMES==39)//发送后半段数据的间隔
	{
		if (TIME_US>=2000-54)//20ms低电平
		{
			Remote_Start();//开PWM
			RETOME_TIMES++;
			TIME_US=0;
		}
	}
	else if (RETOME_TIMES>=40&&RETOME_TIMES<=73)
	{
		if (is_hight)//发送低电平
		{												//应该是37
			bit=(BUFF_DATA[(RETOME_TIMES-5)/8]>>(7-(RETOME_TIMES-5)%8))&0x01;//
			if (bit) //发送1
			{
				if (TIME_US>=164)
				{
					if (RETOME_TIMES!=73)
						Remote_Start();//开PWM
					else
					{	
						TIME_BUZZ=0;//清除忙标志
						TIM_Cmd(TIM1, DISABLE); //发送完成，关闭自身
						Remote_End();
					}
					TIME_US=0;//清空计时器，开始下一次计数
					is_hight=0;//低电平发送完成，下一次发送高电平
				}
				
			}else//发送0
			{
				if (TIME_US>=54)
				{
					if (RETOME_TIMES!=73)
						Remote_Start();//开PWM
					else 
					{
						TIME_BUZZ=0;//清除忙标志
						TIM_Cmd(TIM1, DISABLE); //发送完成，关闭自身
					}
					TIME_US=0;//清空计时器，开始下一次计数
					is_hight=0;//低电平发送完成，下一次发送高电平
				}
				
			}
			
			
		}else  //发送高电平
		{
			if (TIME_US>=65)//无论是数字1还是0，高电平时长都是650ms
			{
				Remote_End();//关PWM
				RETOME_TIMES++;
				is_hight=1;//高电平发送结束，下一次发送低电平
				TIME_US=0;//清空计时器，开始下一次计数
			}
		}
	}
	
}

//在中断中调用各个品牌不同的中断函数

void TIM1_UP_IRQHandler (void )
{
	Remote_IRQHandler();
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update  );  //清除更新中断
}



