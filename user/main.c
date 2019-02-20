#include "system_init.h"
#include "config.h"
#include "fats.h"
#include "program_files.h"
u32 run_time=0;
extern u8 in_iap;			//是否是iap模式

int main()
{	
	u8 buff[58]={0};

	u32 i=0;
	System_Init();//初始化

	if ((IAP_CMD&0xffffff00)!=0xf0000000)
	{
		IAP_CMD=IAP_CMD_SDCARD;
	}
	while(1)
	{
		switch (IAP_CMD)
		{
			case IAP_CMD_REBOOT:
				{jump_app();}
				break;
			case IAP_CMD_RF:		
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
					{jump_app();run_time=0;IAP_CMD=IAP_CMD_SDCARD;}
				break;
			case IAP_CMD_SDCARD:
				if (FATS_STATE==1)
				{
					if(updata("WK_JZQ/updata.bin")==0)
					{jump_app();}
					IAP_CMD=IAP_CMD_RF;
				}
				else
				{
					IAP_CMD=IAP_CMD_RF;
				}
				break;
			case IAP_CMD_INTERNET:
			case IAP_CMD_FLASH:
				break;
			default:
				{jump_app();IAP_CMD=IAP_CMD_SDCARD;}
				break;
		}
	}
	
}





