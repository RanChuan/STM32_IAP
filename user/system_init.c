#include "system_init.h"
#include "config.h"

//系统初始化，初始化顺序不要随意修改
void System_Init()
{

	NVIC_Configuration();	 
	delay_init();	    					//延时函数初始化
	
	
	
	
	USART1_Init(115200);				//RF传递信息串口
	AS32_Param_Init( );					//无线模块初始化
	fats_init();
}



