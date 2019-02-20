#include "malloc.h"
//#include "includes.h"
//�ڴ��(4�ֽڶ���)
//__align(4) u8 membase[MEM_MAX_SIZE];													//�ڲ�SRAM�ڴ��
//�ڴ�����
//u16 memmapbase[MEM_ALLOC_TABLE_SIZE];													//�ڲ�SRAM�ڴ��MAP


#define OS_ENTER_ONLYME()
#define OS_EXIT_ONLYME();

#define MEM_BASE ((u8 *)(0x20000000+SYS_MEM_SIZE+MEM_ALLOC_TABLE_SIZE*2))
#define MEMMAP_BASE ((u16 *)(0x20000000+SYS_MEM_SIZE))

//�ڴ�������	   
u32 memtblsize ;		//�ڴ���С
u32 memblksize;					//�ڴ�ֿ��С
u32 memsize    ;							//�ڴ��ܴ�С


//�ڴ���������
struct _m_mallco_dev mallco_dev;

//�����ڴ�
//*des:Ŀ�ĵ�ַ
//*src:Դ��ַ
//n:��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
void mymemcpy(void *des,void *src,u32 n)  
{  
  u8 *xdes=des;
	u8 *xsrc=src; 
  while(n--)*xdes++=*xsrc++;  
}  

//�����ڴ�
//*s:�ڴ��׵�ַ
//c :Ҫ���õ�ֵ
//count:��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
void mymemset(void *s,u8 c,u32 count)  
{  
    u8 *xs = s;  
    while(count--)*xs++=c;  
}	






//�ڴ�����ʼ��  
//memx:�����ڴ��
void mem_init(void)  
{  
	memtblsize = MEM_ALLOC_TABLE_SIZE;		//�ڴ���С
	memblksize = MEM_BLOCK_SIZE;					//�ڴ�ֿ��С
	memsize    = MEM_MAX_SIZE;							//�ڴ��ܴ�С

	mallco_dev.init=mem_init;				//�ڴ��ʼ��
	mallco_dev.perused=mem_perused;	  //�ڴ�ʹ����
	mallco_dev.membase=MEM_BASE;	    //�ڴ��
	mallco_dev.memmap=MEMMAP_BASE;     //�ڴ����״̬��
	mallco_dev.memrdy=0; 					  //�ڴ����δ����

	
  mymemset(mallco_dev.memmap, 0,memtblsize*2);//�ڴ�״̬����������  
	mymemset(mallco_dev.membase, 0,memsize);	//�ڴ��������������  
	mallco_dev.memrdy=1;								//�ڴ�����ʼ��OK  
} 
 
//��ȡ�ڴ�ʹ����
//memx:�����ڴ��
//����ֵ:ʹ����(0~100)
u8 mem_perused(void)  
{  
    u32 used=0;  
    u32 i;  
    for(i=0;i<memtblsize;i++)  
    {  
        if(mallco_dev.memmap[i])used++; 
    } 
    return (used*100)/(memtblsize);  
}  

//�ڴ����(�ڲ�����)
//memx:�����ڴ��
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:0XFFFFFFFF,�������;����,�ڴ�ƫ�Ƶ�ַ 
u32 mem_malloc(u32 size)  
{  
    signed long offset=0;  
    u16 nmemb;	//��Ҫ���ڴ����  
	u16 cmemb=0;//�������ڴ����
    u32 i;  
    if(!mallco_dev.memrdy)mallco_dev.init();//δ��ʼ��,��ִ�г�ʼ�� 
    if(size==0)return 0XFFFFFFFF;//����Ҫ����

    nmemb=size/memblksize;  	//��ȡ��Ҫ����������ڴ����
    if(size%memblksize)nmemb++;  
    for(offset=memtblsize-1;offset>=0;offset--)//���������ڴ������  
    {     
		if(!mallco_dev.memmap[offset])cmemb++;//�������ڴ��������
		else cmemb=0;								//�����ڴ������
		if(cmemb==nmemb)							//�ҵ�������nmemb�����ڴ��
		{
            for(i=0;i<nmemb;i++)  					//��ע�ڴ��ǿ� 
            {  
                mallco_dev.memmap[offset+i]=nmemb;  
            }  
            return (offset*memblksize);//����ƫ�Ƶ�ַ  
		}
    }  
    return 0XFFFFFFFF;//δ�ҵ����Ϸ����������ڴ��  
}  

//�ͷ��ڴ�(�ڲ�����) 
//memx:�����ڴ��
//offset:�ڴ��ַƫ��
//����ֵ:0,�ͷųɹ�;1,�ͷ�ʧ��;  
u8 mem_free(u32 offset)  
{  
    int i;  
    if(!mallco_dev.memrdy)//δ��ʼ��,��ִ�г�ʼ��
	{
		mallco_dev.init();    
        return 1;//δ��ʼ��  
    }  
    if(offset<memsize)//ƫ�����ڴ����. 
    {  
        int index=offset/memblksize;			//ƫ�������ڴ�����  
        int nmemb=mallco_dev.memmap[index];	//�ڴ������
        for(i=0;i<nmemb;i++)  						//�ڴ������
        {  
            mallco_dev.memmap[index+i]=0;  
        }  
        return 0;  
    }else return 2;//ƫ�Ƴ�����.  
}  

//�ͷ��ڴ�(�ⲿ����) 
//memx:�����ڴ��
//ptr:�ڴ��׵�ַ 
void myfree(void *ptr)  
{  
	OS_ENTER_ONLYME();
	u32 offset;  
    if(ptr==NULL)
		{
			OS_EXIT_ONLYME();
			return;//��ַΪ0.  
		}
 	offset=(u32)ptr-(u32)mallco_dev.membase;  
    mem_free(offset);//�ͷ��ڴ�   
	OS_EXIT_ONLYME();
}

//�����ڴ�(�ⲿ����)
//memx:�����ڴ��
//size:�ڴ��С(�ֽ�)
//����ֵ:���䵽���ڴ��׵�ַ.
void *mymalloc(u32 size)  
{  
	OS_ENTER_ONLYME();
    u32 offset;  									      
	offset=mem_malloc(size);  	   				   
    if(offset==0XFFFFFFFF)
		{
			OS_EXIT_ONLYME();
			return NULL;  
		}
    else 
		{
			OS_EXIT_ONLYME();
			return (void*)((u32)mallco_dev.membase+offset);  
		}
}  

//���·����ڴ�(�ⲿ����)
//memx:�����ڴ��
//*ptr:���ڴ��׵�ַ
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:�·��䵽���ڴ��׵�ַ.
void *myrealloc(void *ptr,u32 size)  
{  
	OS_ENTER_ONLYME();
    u32 offset;  
    offset=mem_malloc(size);  
    if(offset==0XFFFFFFFF)
		{
			OS_EXIT_ONLYME();
			return NULL;     
		}
    else  
    {  									   
	    mymemcpy((void*)((u32)mallco_dev.membase+offset),ptr,size);	//�������ڴ����ݵ����ڴ�   
        myfree(ptr);  											  		//�ͷž��ڴ�
			OS_EXIT_ONLYME();
        return (void*)((u32)mallco_dev.membase+offset);  				//�������ڴ��׵�ַ
    }  
} 












