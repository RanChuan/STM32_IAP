#include "string.h"
#include "stdio.h"
#include "ctype.h"
#include "fats.h"
#include "malloc.h"
#include "stmflash.h"
#include "program_files.h"

#define FLASH_SIZE ((*(u32 *)0x1FFFF7E0)&0x0000ffff)
#define IAP_SIZE  20
#define FATS_SIZE 11



u8 updata(char const *file_name)
{
	FRESULT ret;
	u32 real_num=0;
	u32 file_size=0;
	u32 flash_addr=0x08005000;
	u8 *databuff=mymalloc(2048);
	ret=FATS->f_open(file,file_name,FA_OPEN_EXISTING);
	if (ret==FR_OK)
	{
		file_size=file->fsize;
		if (file_size>(FLASH_SIZE-IAP_SIZE-FATS_SIZE)*1024)
		{
			FATS->f_close(file);//程序太大
			return 0xff;
		}
		while(1)
		{
			ret=FATS->f_read(file,databuff,2048,&real_num);
			if (real_num==0) break;
			if (ret==FR_OK)
			{
				STMFLASH_Write(flash_addr,(u16 *)databuff,real_num/2);
				flash_addr+=real_num;
			}
		}
		FATS->f_close(file);
	}
	else
	{
		return ret;
	}
	return 0;
}







//////////////////////一些辅助函数//////////////////////

		//把hex的字符串转化为num,最大32位
u32 hexstr2num (char *hexstr)
{
	char *buff=hexstr;
	u32 ret=0;
	while (*buff)
	{
		ret = ret<<4;
		if (*buff>='a'&&*buff<='z')
		{
			ret |= buff[0] - 'a'+10;
		}
		else if (*buff>='A'&&*buff<='Z')
		{
			ret |= buff[0] - 'A' + 10;
		}
		else if (*buff>='0'&&*buff<='9')
		{
			ret |= buff[0] - '0';
		}
		else
		{
			return 0;
		}
		buff++;
	}

	return ret;
}


/*
*			函数作用：把hex转化为bin
*			函数行为：把同一存储区的hex文件转化为bin，如果这一存储区的数据在中途结束，返回.....
*			参数：
*
*
*/
u32 hex2bin(u8 *binbuff,char *hexbuff,u32 size)
{
	u32 addr=0;
	char *phexbuff=hexbuff;
	while(*phexbuff)
	{
	//	if (cmpstr)
	}
	return 0;
}



