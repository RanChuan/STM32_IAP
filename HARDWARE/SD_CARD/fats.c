
#include "fats.h"
#include "malloc.h"
#include "mmc_sd.h"
//#include "beep.h"


/***********************************************

			1.�����������¿ؼ�������֧���ļ�ϵͳ����FLASH�ռ���ڵ���256KB��оƬ�Ͽ��������ļ�ϵͳ
			2.Ҫ֧���ļ�ϵͳ����Ҫ�����ļ�ϵͳ��صĴ��룬���ļ�ֻ���ṩ����صĺ������ýӿ�
			3.���ڲ��ܱ�֤оƬ��һ�����ļ�ϵͳ��ش��룬ʹ�ñ��ļ��ṩ�ĺ����ӿ�֮ǰһ��Ҫ�ж��ļ�ϵͳ���ڣ�

**********************************************/



FATFS *fatfs[2];  // Work area (file system object) for logical drive	 
FIL *file;
FIL *ftemp;	 

UINT file_br,file_bw;
FILINFO fileinfo;
DIR dir;


u8 *file_buf;//SD�����ݻ�����



u8 FATS_STATE=0;//0����֧���ļ�ϵͳ��1��֧���ļ�ϵͳ

		//�ļ�ϵͳ״̬�����ڵ��������0����֧���ļ�ϵͳ��1֧���ļ�ϵͳ����û��SD����2��SD�����ǹ���ʧ�ܣ�3֧���ļ�ϵͳ,,
u8 DBG_FATS=0;



				//�Ƿ�֧���ļ�ϵͳ��0����֧�֣�1��֧��
FRESULT fats_state (void)
{
	return FATS_STATE;
}



//fats��ʼ��
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
		return FR_NOT_READY;//��ַ�����������ش���
	}
	
	a=SD_Initialize();
	if (a!=0)//SD����ʼ��ʧ��
	{
		DBG_FATS=1;
		return FR_NOT_READY;
	}
	
	a=FATS->fun_init(&f);

	
	
	u8 ret=0;
	if (a==0xffffffff)//FATFS��ʼ��ʧ��
	{	
		DBG_FATS=2;
		return FR_NOT_READY;
	}
		
	
	
	
	
	fatfs[0]=(FATFS*)mymalloc(sizeof(FATFS));	//Ϊ����0�����������ڴ�	
	fatfs[1]=(FATFS*)mymalloc(sizeof(FATFS));	//Ϊ����1�����������ڴ�
	file=(FIL*)mymalloc(sizeof(FIL));		//Ϊfile�����ڴ�
	ftemp=(FIL*)mymalloc(sizeof(FIL));		//Ϊftemp�����ڴ�
	file_buf=(u8*)mymalloc(512);				//Ϊfatbuf�����ڴ�
	
	
	if (!(fatfs[0]&&fatfs[1]&&file&&ftemp&&file_buf))//ʧ��
	{
		myfree(fatfs[0]);
		myfree(fatfs[1]);
		myfree(file);
		myfree(ftemp);
		myfree(file_buf);
		return FR_NOT_READY;
		
	}

	ret=FATS->f_mount(0,fatfs[0]);
	
	if (ret)//�д������
	{
		DBG_FATS=2;
		return ret;	
	}
	DBG_FATS=3;
	FATS_STATE=1;
	return ret;	

}








