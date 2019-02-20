
#include "fats.h"
#include "malloc.h"
#include "mmc_sd.h"
//#include "beep.h"


/***********************************************

			1.并不是所有温控集中器都支持文件系统，在FLASH空间大于等于256KB的芯片上可以下载文件系统
			2.要支持文件系统还需要下载文件系统相关的代码，本文件只是提供了相关的函数调用接口
			3.由于不能保证芯片中一定有文件系统相关代码，使用本文件提供的函数接口之前一定要判断文件系统存在！

**********************************************/



FATFS *fatfs[2];  // Work area (file system object) for logical drive	 
FIL *file;
FIL *ftemp;	 

UINT file_br,file_bw;
FILINFO fileinfo;
DIR dir;


u8 *file_buf;//SD卡数据缓存区



u8 FATS_STATE=0;//0，不支持文件系统，1，支持文件系统

		//文件系统状态，用于调试输出，0，不支持文件系统，1支持文件系统但是没有SD卡，2有SD卡但是挂载失败，3支持文件系统,,
u8 DBG_FATS=0;



				//是否支持文件系统，0，不支持，1，支持
FRESULT fats_state (void)
{
	return FATS_STATE;
}



//fats初始化
FRESULT fats_init (void)
{
	u32 a=0;
	function f;
	f.flash_read=0;
	f.flash_write=0;
	f.myfree=myfree;
	f.mymalloc=mymalloc;
	f.sd_GetSectorCount=SD_GetSectorCount;
	f.sd_Initialize=SD_Initialize;
	f.sd_read=SD_ReadDisk;
	f.sd_Reset=SD_Reset;
	f.sd_WaitReady=SD_WaitReady;
	f.sd_write=SD_WriteDisk;
	
	
	if (((*(u32*)(FATS))<0x08000000u)||((*(u32*)(FATS))>(0x08000000u+256*1024)))
	{
		DBG_FATS=0;
		return FR_NOT_READY;//地址不正常，返回错误
	}
	
	a=SD_Initialize();
	if (a!=0)//SD卡初始化失败
	{
		DBG_FATS=1;
		return FR_NOT_READY;
	}
	
	a=FATS->fun_init(&f);

	
	
	u8 ret=0;
	if (a==0xffffffff)//FATFS初始化失败
	{	
		DBG_FATS=2;
		return FR_NOT_READY;
	}
		
	
	
	
	
	fatfs[0]=(FATFS*)mymalloc(sizeof(FATFS));	//为磁盘0工作区申请内存	
	fatfs[1]=(FATFS*)mymalloc(sizeof(FATFS));	//为磁盘1工作区申请内存
	file=(FIL*)mymalloc(sizeof(FIL));		//为file申请内存
	ftemp=(FIL*)mymalloc(sizeof(FIL));		//为ftemp申请内存
	file_buf=(u8*)mymalloc(512);				//为fatbuf申请内存
	
	
	if (!(fatfs[0]&&fatfs[1]&&file&&ftemp&&file_buf))//失败
	{
		myfree(fatfs[0]);
		myfree(fatfs[1]);
		myfree(file);
		myfree(ftemp);
		myfree(file_buf);
		return FR_NOT_READY;
		
	}

	ret=FATS->f_mount(0,fatfs[0]);
	
	if (ret)//有错误产生
	{
		DBG_FATS=2;
		return ret;	
	}
	DBG_FATS=3;
	FATS_STATE=1;
	return ret;	

}








