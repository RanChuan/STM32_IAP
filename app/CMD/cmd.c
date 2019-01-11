#include "cmd.h"
#include "config.h"
#include "drv_as62.h"
#include "uart_485.h"
#include "sys.h"

#define FLASH_SIZE ((*(u32 *)0x1FFFF7E0)&0x0000ffff)
#define IAP_SIZE  10

u16 flash_size=0;					//���Բ�����flash��С
u8 data_buff[2048]={456%256,456/256};		//flash�����
u16 data_offset=0;		//�����ƫ����
u8 in_iap=0;			//�Ƿ���iapģʽ
u32 flash_addr=0;//Ҫ������flash��ַ


//��Щ��flash����ʱ�ı���
u16 data_num=0;//��ǰ���ݰ�
u16 data_all=0;//�����ݰ�

//�����û��������ʼ��ַ
void (*user_main)(void);



//����ʱ�䣬��main.c�ж����
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
				run_time=0;//�յ���Ч�����������ʱ�����
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
		case 0x00://��
			read_flash(buff);
			break;
		case 0x01://д
			write_flash(buff);
			break;
		case 0x02://д������
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
	if ((flash_addr<0x8000000+IAP_SIZE*1024)||(flash_addr>0x8000000+FLASH_SIZE*1024))//д����ռ䲻����дIAP���ڵĳ����
	{
		cmd_return(buff,ERR_FLASHADDR);
		return ;
	}
	if (size>flash_size*1024-(flash_addr-0x8002800))//ȷ�����ݶ����ڿɲ�����ַ��Χ��2018.10.8
	{
		cmd_return(buff,ERR_DATALENGTH);
		return ;
	}
	for (i=0;i<2048;i++){data_buff[i]=0;}
	data_num=0;
	data_all=0;
	data_offset=0;
	in_iap=1;//����iapģʽ
	cmd_return(buff,ERR_SUCCESS);
}

void read_flash (u8 *buff)
{
}


			//�����Ƿ������ݣ�1��������
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
			cmd_return(buff,ERR_SUCCESS);//����û���յ��ϴεĳɹ����أ�����һ��
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
		if (data_num==data_all)//���һ�����ݰ�
		{
			if (data_real) 
			{
				STMFLASH_Write(flash_addr,(u16 *)data_buff,data_offset/2);
				data_offset=0;
				flash_addr=0;
				for (j=0;j<2048;j++){data_buff[j]=0;}
			}
			in_iap=0;//�˳�iap
		}
		cmd_return(buff,ERR_SUCCESS);
		delay_ms(50);//��ֹ���ݷ��ͳ�ȥ֮ǰ����ת��APP
		if (in_iap==0)
		{
			jump_app();		//����������֮���Խ���Ӧ�ó���
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
	data[2]=buff[2];//��ַ�������������ĵ�ַһ��
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
	if(((*(vu32*)0x8002800)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		user_main=(void (*)(void))*(vu32*)(0x8002800+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		__set_MSP(*(vu32*)0x8002800);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		user_main();									//��ת��APP.
	}
	
}







