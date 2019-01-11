/*****************************************************************
使用定时器的PWM输出发送红外遥控
定时器1,38k载波频率,定时器2，编码调制

格力空调红外编码有两个数据段
第一段36bit，最后一位为停止位，第二段33bit，最后一位为停止位
用u8记录，一共69位，9个字节

用中断方式发送红外信号，
使用时先用判忙函数确定红外发送为空闲即可发送

作者：冉川
创建时间：2018.3.7
初步完成时间：2018.3.7


美的空调一般情况只有三字节数据（开机）
发送方式为LAA~BB~CC~SLAA~BB~CC~
关机时发送方式为LAA~BB~CC~SLAA~BB~CC~SLAA~QQ~YY~
Q是关机小时数，Y是关机分钟数，立即关机都应为0

实现格力空调的键值学习和遥控发送，示波器分析能正确发送接收到的编码，没有实际测试
完成时间：2018.3.12


***********************************************************************/
#include "sys.h"
#include "stm32f10x_tim.h"


				//空调类型定义
#define GREE 0x01
#define MIDEA 0x02
#define USER	0x03 //使用学习的遥控码


//extern u8  TIME_BUZZ;//忙标志，0不忙，1忙
//extern vu16 TIME_US;//记录有多少个10us，每次中断就增加一个10us
//extern u16 RETOME_TIMES;//发送的脉冲次数,数据位的次数
//extern u8  BUFF_DATA[9];//将要发送的数据头地址
//extern void (*Remote_IRQHandler) (void);
//extern void (*Receive_IRQHandler) (void );




//红外遥控初始化，hz，红外的载波频率
void Remote_init (u16 hz);
// buff 数据段，空调的数据发送，brand，品牌
void Remote_send (u8* buff,u8 brand);

// buff 数据段，空调的数据接收，brand，品牌
void Remote_receive (u8 * buff,u8 brand );
//停止接收
void Remote_stoprec (void  );
u8 Remote_GetState (void );//1，忙，0，不忙


//自定义键值发送，1，开，2，关，3，升温，4，降温
void Remote_send_USER (u8 keytype);






