#include "cmd.h"
#include "config.h"
#include "drv_as62.h"
#include "uart_485.h"
#include "sys.h"

#define FLASH_SIZE ((*(u32 *)0x1FFFF7E0)&0x0000ffff)
#define IAP_SIZE  10

u16 flash_size=0;					//可以操作的flash大小
u8 data_buff[2048]={456%256,456/256};		//flash缓冲池
u16 data_offset=0;		//缓冲池偏移量
u8 in_iap=0;			//是否是iap模式
u32 flash_addr=0;//要操作的flash地址


//这些是flash操作时的变量
u16 data_num=0;//当前数据包
u16 data_all=0;//总数据包

//定义用户程序的起始地址
void (*user_main)(void);



//运行时间，在main.c中定义的
extern u32 run_time;




void Get_cmd (u8 *buff)
{
	u16 length=0;
	u8 crc[2]={0};
	u8 is_cmd=0;
	if (flash_size==0)
	{
		flash_size=FLASH_SIZE-IAP_SIZE;
	}
	if (buff[0]==0xff&&buff[1]==0xff)
	{
		length=((buff[5]<<8)|buff[6])+7;
		if (length>56) return;
		Get_Crc16(buff,length,crc);
		if (buff[length]==crc[0]&&buff[length+1]==crc[1])
		{
			is_cmd=1;
		}
	}
	
	if (is_cmd)
	{
		switch(buff[4])
		{
			case 0x05:
				run_time=0;//收到有效命令，清零运行时间计数
				cmd_0x05 (buff);
				break;
			default:
				cmd_return(buff,ERR_NULLCMD);
				break;
		}
	}
}



void cmd_0x05 (u8 *buff)
{
	switch (buff[7])
	{
		case 0x00://读
			read_flash(buff);
			break;
		case 0x01://写
			write_flash(buff);
			break;
		case 0x02://写进行中
			break;
		case 0x03:
			writting_flash(buff);
			break;
		default:
			cmd_return(buff,ERR_DATAVALE);
			break;
	}
}



void write_flash (u8 *buff)
{
	u16 size=0;
	u16 i=0;
	size=(buff[8]<<8)|buff[9];
	flash_addr=(buff[10]<<24)|(buff[11]<<16)|(buff[12]<<8)|buff[13];
	if ((flash_addr<0x8000000+IAP_SIZE*1024)||(flash_addr>0x8000000+FLASH_SIZE*1024))//写程序空间不能重写IAP所在的程序段
	{
		cmd_return(buff,ERR_FLASHADDR);
		return ;
	}
	if (size>flash_size*1024-(flash_addr-0x8002800))//确保数据都在在可操作地址范围内2018.10.8
	{
		cmd_return(buff,ERR_DATALENGTH);
		return ;
	}
	for (i=0;i<2048;i++){data_buff[i]=0;}
	data_num=0;
	data_all=0;
	data_offset=0;
	in_iap=1;//进入iap模式
	cmd_return(buff,ERR_SUCCESS);
}

void read_flash (u8 *buff)
{
}


			//返回是否有数据，1，有数据
u8 data_real(void)
{
	u16 i=0;
	for (i=0;i<2048;i++)
	{
		if (data_buff[i]!=0)
		{
			return 1;
		}
	}
	return 0;
}



void writting_flash (u8 *buff)
{
	u16 i_num=0;
	u16 datalength=0;
	u16 i=0;
	u16 j=0;
	if (in_iap==1)
	{
		data_num++;
		i_num=(buff[8]<<8)|buff[9];
		data_all=(buff[10]<<8)|buff[11];
		if (i_num>data_num)
		{
			cmd_return(buff,ERR_FLASHWRITE);
			in_iap=0;
			return;
		}
		if (i_num<data_num)
		{
			cmd_return(buff,ERR_SUCCESS);//主机没有收到上次的成功返回，补发一次
			data_num--;
			return;
		}
		datalength=((buff[5]<<8)|buff[6])-5;
		for (i=0;i<datalength;i++)
		{
			data_buff[data_offset]=buff[12+i];
			data_offset++;
			if (FLASH_SIZE<256)
			{
				if (data_offset>1023)
				{
					data_offset=0;
					STMFLASH_Write(flash_addr,(u16 *)data_buff,512);
					flash_addr+=1024;
					for (j=0;j<2048;j++){data_buff[j]=0;}
				}
			}
			else 
			{
				if (data_offset>2047)
				{
					data_offset=0;
					STMFLASH_Write(flash_addr,(u16 *)data_buff,1024);
					flash_addr+=2048;
					for (j=0;j<2048;j++){data_buff[j]=0;}
				}
			}
		}
		if (data_num==data_all)//最后一个数据包
		{
			if (data_real) 
			{
				STMFLASH_Write(flash_addr,(u16 *)data_buff,data_offset/2);
				data_offset=0;
				flash_addr=0;
				for (j=0;j<2048;j++){data_buff[j]=0;}
			}
			in_iap=0;//退出iap
		}
		cmd_return(buff,ERR_SUCCESS);
		delay_ms(50);//防止数据发送出去之前就跳转到APP
		if (in_iap==0)
		{
			jump_app();		//程序更新完成之后尝试进入应用程序
		}
	}
	else
	{
		cmd_return(buff,ERR_NOCONNECT);
	}
}


void cmd_return (u8 * buff,u16 err)
{
	u8 data[20]={0};
	u8 crc[2]={0};
	data[0]=0xff;
	data[1]=0xff;
	data[2]=buff[2];//地址和主机发送来的地址一致
	data[3]=buff[3];
	data[4]=0x80|buff[4];
	data[5]=0x00;
	data[6]=0x02;
	data[7]=err>>8;
	data[8]=err;
	Get_Crc16(data,(data[6])+7,crc);
	data[(data[6])+7]=crc[0];
	data[(data[6])+7+1]=crc[1];
	AS32_TX_none(data,(data[6])+7+2);
}





void jump_app(void)
{
	if(((*(vu32*)0x8002800)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		user_main=(void (*)(void))*(vu32*)(0x8002800+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		__set_MSP(*(vu32*)0x8002800);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		user_main();									//跳转到APP.
	}
	
}







