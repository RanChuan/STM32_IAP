#ifndef  __CONFIG_H
#define  __CONFIG_H
#include "stm32f10x.h"
#include "stmflash.h"
/**************************************************

系统配置文件
配置控制器类型、控制器地址



*************************************************/
#define MYSTYLE_KT 0x02					//空调
#define MYSTYLE_CS 0x03					//除湿机
#define MYSTYLE_JH 0x04					//空气净化机




				//这个全局变量数组定义在stmflash.c文件里
				//由于这个变量数组保存了系统的配置信息，在这里引用
				//由于这个变量数组需要保存在flash里，在stmflash.c文件声明
extern u16 MY_CONFIG[100];//配置信息


				//为了使用方便，用宏改名

#define MY_ADDR MY_CONFIG[0]

#define MY_STYLE MY_CONFIG[1]

#define KT_STATE MY_CONFIG[2]

#define KT_STATE1 MY_CONFIG[5]//空调升降温位

#define CS_STATE MY_CONFIG[3]

#define JH_STATE MY_CONFIG[4]



#define MY_MODEL MY_CONFIG[6]  //记录当前工作模式,配置模式还是正常工作模式

#define KT_STYLE MY_CONFIG[7]		//空调类型，格力还是美的

#define WORK_MODE MY_CONFIG[8]		//工作模式单纯控制模式还是数据解析模式

#define CJ_ADDR   MY_CONFIG[9]		//所属采集器的地址，因为变量是u16，所以无需改动

#define IF_WORK   MY_CONFIG[10]  //是否在工作，1，在工作，0，没工作

#define IF_HAND   MY_CONFIG[11]    //手动模式还是自动模式，1，手动，2，自动



#define WARN_WATER MY_CONFIG[12]   //漏水报警

#define WARN_WENDU MY_CONFIG[13]   //温度报警

#define WARN_SHIDU MY_CONFIG[14]   //湿度报警
#define WARN_TVOC 	MY_CONFIG[15]   //TVOC报警
#define WARN_PM2_5 MY_CONFIG[16]   //PM2.5报警



									

												//环境变量的上下限，环境变量上下限没有小数部分

#define LIMIT_WENDU_H		MY_CONFIG[17]			//温度上限，
#define LIMIT_WENDU_L		MY_CONFIG[18]			//温度下限
#define LIMIT_SHIDU_H		MY_CONFIG[19]			//湿度上限
#define LIMIT_SHIDU_L   MY_CONFIG[20]			//湿度下限
#define LIMIT_TVOC			MY_CONFIG[21]			//TVOC只有上限
#define LIMIT_PM2_5			MY_CONFIG[22]			//PM2_5只有上限



#define CS_IN_CD				MY_CONFIG[23]			//除湿机处于CD状态，不可开关


#define KT_USER_CMD    MY_CONFIG[24]	//配置时的空调自定义的红外命令，升温降温等






#define WORK_MODE1 0x01					//单纯控制模式
#define WORK_MODE2 0x02					//数据解析模式


void Config_Init(void);

//设置本机控制器类型，返回0xff失败,0成功
u8 Set_MyStyle (u8 style);

u8 Set_CJ_Addr (u16 addr);

void Set_Addr (u8 addr);
#endif

