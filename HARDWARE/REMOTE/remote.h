/*****************************************************************
ʹ�ö�ʱ����PWM������ͺ���ң��
��ʱ��1,38k�ز�Ƶ��,��ʱ��2���������

�����յ�����������������ݶ�
��һ��36bit�����һλΪֹͣλ���ڶ���33bit�����һλΪֹͣλ
��u8��¼��һ��69λ��9���ֽ�

���жϷ�ʽ���ͺ����źţ�
ʹ��ʱ������æ����ȷ�����ⷢ��Ϊ���м��ɷ���

���ߣ�Ƚ��
����ʱ�䣺2018.3.7
�������ʱ�䣺2018.3.7


���Ŀյ�һ�����ֻ�����ֽ����ݣ�������
���ͷ�ʽΪLAA~BB~CC~SLAA~BB~CC~
�ػ�ʱ���ͷ�ʽΪLAA~BB~CC~SLAA~BB~CC~SLAA~QQ~YY~
Q�ǹػ�Сʱ����Y�ǹػ��������������ػ���ӦΪ0

ʵ�ָ����յ��ļ�ֵѧϰ��ң�ط��ͣ�ʾ������������ȷ���ͽ��յ��ı��룬û��ʵ�ʲ���
���ʱ�䣺2018.3.12


***********************************************************************/
#include "sys.h"
#include "stm32f10x_tim.h"


				//�յ����Ͷ���
#define GREE 0x01
#define MIDEA 0x02
#define USER	0x03 //ʹ��ѧϰ��ң����


//extern u8  TIME_BUZZ;//æ��־��0��æ��1æ
//extern vu16 TIME_US;//��¼�ж��ٸ�10us��ÿ���жϾ�����һ��10us
//extern u16 RETOME_TIMES;//���͵��������,����λ�Ĵ���
//extern u8  BUFF_DATA[9];//��Ҫ���͵�����ͷ��ַ
//extern void (*Remote_IRQHandler) (void);
//extern void (*Receive_IRQHandler) (void );




//����ң�س�ʼ����hz��������ز�Ƶ��
void Remote_init (u16 hz);
// buff ���ݶΣ��յ������ݷ��ͣ�brand��Ʒ��
void Remote_send (u8* buff,u8 brand);

// buff ���ݶΣ��յ������ݽ��գ�brand��Ʒ��
void Remote_receive (u8 * buff,u8 brand );
//ֹͣ����
void Remote_stoprec (void  );
u8 Remote_GetState (void );//1��æ��0����æ


//�Զ����ֵ���ͣ�1������2���أ�3�����£�4������
void Remote_send_USER (u8 keytype);






