#include "system_init.h"
#include "config.h"

u32 run_time=0;
extern u8 in_iap;			//是否是iap模式

int main()
{	
	u8 buff[58]={0};

	u32 i=0;
	System_Init();//初始化

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
		if (run_time*5>5*1000) //五秒之后，如果程序段存在应用程序，跳转至应用程序
			{jump_app();}
	}
	
}





