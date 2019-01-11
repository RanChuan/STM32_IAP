#include "receive.h"
#include "config.h"
#define CUPTRE PAin(1)


#define CHANG_RISE TIM2->CCER&=0xffdf		//切换为上升沿
#define CHANG_FALL TIM2->CCER|=0x0020		//切换为下降沿


u16 cupture[CUPTURE_NUM];//
void 	(*Receive_IRQHandler) (void );

#define K_R  20  //包容值
#define K_L  (20)  //包容值

static u8 RECEIVE_ENABLE=1;//允许接收
//获取捕获到的红外键码

u8 Receive_GetState (void)//1,有可读键值，0，没有
{
	return !RECEIVE_ENABLE;
}

void Receive_GetKey (u8 *buff,u16 num)
{
	u16 i=0;
	if (num>9) num=9;
	if (!RECEIVE_ENABLE)
	{
		for (i=0;i<9;i++)
		{
			buff[i]=0;
		}
		for (i=2;i<70;i++)
		{
				if ((cupture[i]>(0xe5-K_L))&&(cupture[i]<(0xe5+K_R)))
					buff[(i-2)/8]|=0x80>>((i-2)%8);
		}
			for (i=0;i<CUPTURE_NUM;i++)
				cupture[i]=0;
	RECEIVE_ENABLE=1;
	}
}





//存储空调遥控键值，1，开，2，关，3，升温，4，降温
void Receive_SaveKey (u8 keytype)
{
	u16 i=0;
	if (!RECEIVE_ENABLE)
	{
		if (keytype==1)
		{
			Save_Key_On();
			
		}
		else if (keytype==2)
		{
			Save_Key_Off();
		}
		else if (keytype==3)
		{
			Save_Key_Up();
		}
		else if (keytype==4)
		{
			Save_Key_Down();
		}
		else if (keytype==5)
		{
		}
		for (i=0;i<CUPTURE_NUM;i++)
			cupture[i]=0;
		RECEIVE_ENABLE=1;
	}
}








u16 RCV_NUM=0;


void Receive_GREE (void )
{
				//切换捕获边沿
	if (CUPTRE)
		CHANG_FALL;
	else
		CHANG_RISE;
	if (RECEIVE_ENABLE) cupture[RCV_NUM]= TIM_GetCapture2(TIM2);//读取捕获值
	TIM_SetCounter(TIM2,0);//计数器设置为0
	RCV_NUM++;
	if (RCV_NUM==140)
	{
		RCV_NUM=0;
		RECEIVE_ENABLE=0;//不再允许接收
	}
	else if (RCV_NUM==0)
	{
	}
	
}



void Receive_USER (void )
{
				//切换捕获边沿
	if (CUPTRE)
		CHANG_FALL;
	else
		CHANG_RISE;
	if (RECEIVE_ENABLE) cupture[RCV_NUM]= TIM_GetCapture2(TIM2);//读取捕获值
	TIM_SetCounter(TIM2,0);//计数器设置为0
	RCV_NUM++;
}





void Receive_init (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef  TIM2_ICInitStructure;
	
	//函数指针初始化
	Receive_IRQHandler=Receive_GREE;
	if (KT_STYLE==GREE)
	{
		Receive_IRQHandler=Receive_GREE;
	}else if (KT_STYLE==MIDEA)
	{
		Receive_IRQHandler=Receive_GREE;
	}
	else if (KT_STYLE==USER)
	{
		Receive_IRQHandler=Receive_USER;
	}
	
	//时钟启动
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	

  //输入引脚初始化
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;  //PA0 清除之前设置  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //PA0 输入  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);						 //PA0 
	
	
/*******************************************************************************************


													定时器2初始化


*********************************************************************************************/
	//定时器2配置信息
	TIM_TimeBaseStructure.TIM_Period = 4000; //最大表示40ms
	TIM_TimeBaseStructure.TIM_Prescaler =720-1; //每10us计数器加1 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 

	//初始化TIM5输入捕获参数
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	选择输入端 IC2映射到TI2上
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	//上升沿捕获
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI2上
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);

	//定时器2中断
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM5中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
	TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//打开捕获定时器
	TIM_Cmd(TIM2,ENABLE );

}


void Receive_Cmd (FunctionalState t)
{
		TIM_Cmd(TIM2,t );

}


//40ms超时后
void ReceiveEnd_IRQHandler (void)
{
	if (RCV_NUM)
		RECEIVE_ENABLE=0;//不再允许接收
	RCV_NUM=0;
}




void TIM2_IRQHandler (void )
{
	if (TIM_GetITStatus(TIM2, TIM_IT_CC2))
	{
		Receive_IRQHandler();
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
	}
	
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update))
	{
		ReceiveEnd_IRQHandler();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}



