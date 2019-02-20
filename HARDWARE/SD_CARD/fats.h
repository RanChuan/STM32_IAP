#ifndef __FATS_H
#define __FATS_H
#include "sys.h"




/* File function return code (FRESULT) */

typedef enum {
	FR_OK = 0,				/* (0) Succeeded */
	FR_DISK_ERR,			/* (1) A hard error occured in the low level disk I/O layer �ͼ������ i/o ���г���Ӳ����*/
	FR_INT_ERR,				/* (2) Assertion failed ����ʧ��*/
	FR_NOT_READY,			/* (3) The physical drive cannot work �����������޷���������*/
	FR_NO_FILE,				/* (4) Could not find the file �Ҳ����ļ�*/
	FR_NO_PATH,				/* (5) Could not find the path �Ҳ���·��*/
	FR_INVALID_NAME,		/* (6) The path name format is invalid ·������ʽ��Ч*/
	FR_DENIED,				/* (7) Acces denied due to prohibited access or directory full �������ڽ�ֹ���ʻ�Ŀ¼���������ܾ�*/
	FR_EXIST,				/* (8) Acces denied due to prohibited access �������ܽ�ֹ��������ܾ�*/
	FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid �ļ�/Ŀ¼������Ч*/
	FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected ����������д����*/
	FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid �߼�����������Ч*/
	FR_NOT_ENABLED,			/* (12) The volume has no work area ��û�й�����*/
	FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume û����Ч�� FAT ��*/
	FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any parameter error �����κβ�������, f_mkfs () ����ֹ*/
	FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period �޷��������Ȩ���Է����Ѷ����ڼ��ڵľ�*/
	FR_LOCKED,				/* (16) The operation is rejected according to the file shareing policy �����ļ� shareing ���Ծܾ�����*/
	FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated �޷����� LFN ����������*/
	FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE �򿪵��ļ��� > _FS_SHARE*/
	FR_INVALID_PARAMETER	/* (19) Given parameter is invalid ����������Ч*/
} FRESULT;


/* These types must be 16-bit, 32-bit or larger integer */
typedef int				INT;
typedef unsigned int	UINT;

/* These types must be 8-bit integer */
typedef char			CHAR;
typedef unsigned char	UCHAR;
typedef unsigned char	BYTE;

/* These types must be 16-bit integer */
typedef short			SHORT;
typedef unsigned short	USHORT;
typedef unsigned short	WORD;
typedef unsigned short	WCHAR;

/* These types must be 32-bit integer */
typedef long			LONG;
typedef unsigned long	ULONG;
typedef unsigned long	DWORD;


#ifndef _INC_TCHAR
typedef char TCHAR;
#define _T(x) x
#define _TEXT(x) x
#endif



/* File system object structure (FATFS) */
#pragma pack ( )

typedef struct {
	BYTE	fs_type;		/* FAT sub-type (0:Not mounted) */
	BYTE	drv;			/* Physical drive number */
	BYTE	csize;			/* Sectors per cluster (1,2,4...128) */
	BYTE	n_fats;			/* Number of FAT copies (1,2) */
	BYTE	wflag;			/* win[] dirty flag (1:must be written back) */
	BYTE	fsi_flag;		/* fsinfo dirty flag (1:must be written back) */
	WORD	id;				/* File system mount ID */
	WORD	n_rootdir;		/* Number of root directory entries (FAT12/16) */


	DWORD	last_clust;		/* Last allocated cluster */
	DWORD	free_clust;		/* Number of free clusters */
	DWORD	fsi_sector;		/* fsinfo sector (FAT32) */

	DWORD	cdir;			/* Current directory start cluster (0:root) */

	DWORD	n_fatent;		/* Number of FAT entries (= number of clusters + 2) */
	DWORD	fsize;			/* Sectors per FAT */
	DWORD	fatbase;		/* FAT start sector */
	DWORD	dirbase;		/* Root directory start sector (FAT32:Cluster#) */
	DWORD	database;		/* Data start sector */
	DWORD	winsect;		/* Current sector appearing in the win[] */
	BYTE	win[512];	/* Disk access window for Directory, FAT (and Data on tiny cfg) */
} FATFS;
#pragma pack ( )





/* File object structure (FIL) */
#pragma pack ( )

typedef struct {
	FATFS*	fs;				/* Pointer to the owner file system object */
	WORD	id;				/* Owner file system mount ID */
	BYTE	flag;			/* File status flags */
	BYTE	pad1;
	DWORD	fptr;			/* File read/write pointer (0 on file open) */
	DWORD	fsize;			/* File size */
	DWORD	sclust;			/* File start cluster (0 when fsize==0) */
	DWORD	clust;			/* Current cluster */
	DWORD	dsect;			/* Current data sector */

	DWORD	dir_sect;		/* Sector containing the directory entry */
	BYTE*	dir_ptr;		/* Ponter to the directory entry in the window */

	DWORD*	cltbl;			/* Pointer to the cluster link map table (null on file open) */


	BYTE	buf[512];	/* File data read/write buffer */
} FIL;
#pragma pack ( )





/* Directory object structure (DIR) */
#pragma pack ( )

typedef struct {
	FATFS*	fs;				/* Pointer to the owner file system object */
	WORD	id;				/* Owner file system mount ID */
	WORD	index;			/* Current read/write index number */
	DWORD	sclust;			/* Table start cluster (0:Root dir) */
	DWORD	clust;			/* Current cluster */
	DWORD	sect;			/* Current sector */
	BYTE*	dir;			/* Pointer to the current SFN entry in the win[] */
	BYTE*	fn;				/* Pointer to the SFN (in/out) {file[8],ext[3],status[1]} */

	WCHAR*	lfn;			/* Pointer to the LFN working buffer */
	WORD	lfn_idx;		/* Last matched LFN index number (0xFFFF:No LFN) */

} DIR;
#pragma pack ( )





/* File status structure (FILINFO) */
#pragma pack ( )

typedef struct {
	DWORD	fsize;			/* File size */
	WORD	fdate;			/* Last modified date */
	WORD	ftime;			/* Last modified time */
	BYTE	fattrib;		/* Attribute */
	TCHAR	fname[13];		/* Short file name (8.3 format) */
#if _USE_LFN
	TCHAR*	lfname;			/* Pointer to the LFN buffer */
	UINT 	lfsize;			/* Size of LFN buffer in TCHAR */
#endif
} FILINFO;
#pragma pack ( )






/* File access control and file status flags (FIL.flag) */

#define	FA_READ				0x01			//ָ���Զ���Ķ�ȡ����Ȩ�ޡ����Դ��ļ��ж�ȡ����
#define	FA_OPEN_EXISTING	0x00		//���ļ�������ļ�������, ��ú���ʧ��
#define FA__ERROR			0x80			

#define	FA_WRITE			0x02			//ָ���Զ����д����Ȩ�ޡ����Խ�����д���ļ����� FA_READ ���ʹ��, ���ж�д����
#define	FA_CREATE_NEW		0x04			//����һ�����ļ�������ļ��Ѵ���, ��ú�����ʧ�� FR_EXIST��
#define	FA_CREATE_ALWAYS	0x08		//����һ�����ļ�������ļ��Ѵ���, �������ضϲ������ǡ�
#define	FA_OPEN_ALWAYS		0x10			//����Ѵ���, ��򿪸��ļ�������, ������һ�����ļ�
#define FA__WRITTEN			0x20			
#define FA__DIRTY			0x40





/* File attribute bits for directory entry */

#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define	AM_VOL	0x08	/* Volume label */
#define AM_LFN	0x0F	/* LFN entry */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */
#define AM_MASK	0x3F	/* Mask of defined bits */






typedef struct 
{
	void *(*mymalloc)(u32 size);
	void (*myfree)(void *point);
	void (*flash_read)(u8 *buff,u32 addr,u32 size);
	void (*flash_write)(u8 *buff,u32 addr,u32 size);
	u8 (*sd_read)(u8*buf,u32 sector,u8 cnt);
	u8 (*sd_write)(u8*buf,u32 sector,u8 cnt);
	
	u32 (*sd_GetSectorCount)(void);
	u8 (*sd_Initialize)(void);
	void (*sd_Reset)(void);
	u8 (*sd_WaitReady)(void);
} function ;






typedef struct
{
	u32 (*fun_init)(function *f);//��ʼ����������
	FRESULT (*f_mount) (BYTE, FATFS*);						/* Mount/Unmount a logical drive */
	FRESULT (*f_open) (FIL*, const TCHAR*, BYTE);			/* Open or create a file */
	FRESULT (*f_read) (FIL*, void*, UINT, UINT*);			/* Read data from a file */
	FRESULT (*f_lseek) (FIL*, DWORD);						/* Move file pointer of a file object */
	FRESULT (*f_close) (FIL*);								/* Close an open file object */
	FRESULT (*f_opendir) (DIR*, const TCHAR*);				/* Open an existing directory */
	FRESULT (*f_readdir) (DIR*, FILINFO*);					/* Read a directory item */
	FRESULT (*f_stat) (const TCHAR*, FILINFO*);			/* Get file status */
	FRESULT (*f_write) (FIL*, const void*, UINT, UINT*);	/* Write data to a file */
	FRESULT (*f_getfree) (const TCHAR*, DWORD*, FATFS**);	/* Get number of free clusters on the drive */
	FRESULT (*f_truncate) (FIL*);							/* Truncate file */
	FRESULT (*f_sync) (FIL*);								/* Flush cached data of a writing file */
	FRESULT (*f_unlink) (const TCHAR*);					/* Delete an existing file or directory */
	FRESULT	(*f_mkdir) (const TCHAR*);						/* Create a new directory */
	FRESULT (*f_chmod) (const TCHAR*, BYTE, BYTE);			/* Change attriburte of the file/dir */
	FRESULT (*f_utime) (const TCHAR*, const FILINFO*);		/* Change timestamp of the file/dir */
	FRESULT (*f_rename) (const TCHAR*, const TCHAR*);		/* Rename/Move a file or directory */
	FRESULT (*f_chdrive) (BYTE);							/* Change current drive */
	FRESULT (*f_chdir) (const TCHAR*);						/* Change current directory */
	FRESULT (*f_getcwd) (TCHAR*, UINT);					/* Get current directory */
	FRESULT (*f_forward) (FIL*, UINT(*)(const BYTE*,UINT), UINT, UINT*);	/* Forward data to the stream */
	FRESULT (*f_mkfs) (BYTE, BYTE, UINT);					/* Create a file system on the drive */
	FRESULT	(*f_fdisk) (BYTE, const DWORD[], void*);		/* Divide a physical drive into some partitions */
	int (*f_putc) (TCHAR, FIL*);							/* Put a character to the file */
	int (*f_puts) (const TCHAR*, FIL*);					/* Put a string to the file */
	int (*f_printf) (FIL*, const TCHAR*, ...);				/* Put a formatted string to the file */
	TCHAR* (*f_gets) (TCHAR*, int, FIL*);					/* Get a string from the file */
} fats_fun,*lpfats_fun;


#define FATS ((lpfats_fun)((u32*)(0x8040000-1024*16)))



FRESULT fats_init (void);//�ļ�ϵͳ��ʼ����0���ɹ�����0��ʧ��
FRESULT fats_state (void);//�Ƿ�֧���ļ�ϵͳ��1��֧�֣�0����֧��


						//�ļ�ϵͳʹ�õ�ȫ�ֱ���

extern FATFS *fatfs[2];  // Work area (file system object) for logical drive	 
extern FIL *file;
extern FIL *ftemp;	 

extern UINT file_br,file_bw;
extern FILINFO fileinfo;
extern DIR dir;


extern u8 *file_buf;//SD�����ݻ�����

extern u8 FATS_STATE;


#endif

