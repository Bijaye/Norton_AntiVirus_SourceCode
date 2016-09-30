/*
 * ARCHMANAGER
 * COPYRIGHT FUJITSU DEVICES INC. 1995
 *
 */
#ifndef __AMGIMPL_H__
#define __AM_H__


#include "TypeUtil.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#if defined(_WINDOWS)
#include <share.h>
#include <io.h>
#endif

class AMGImpl;

typedef unsigned short int amg_index;	/* index(0~Max_entry-1)*/ 
typedef unsigned short int hash_num;	/* hash_table number(0~TSIZE-1)*/ 
typedef unsigned short int sword;		/* node number of code tree (0~Max_entry*4) */
typedef unsigned char byte;				/* 1byte */

typedef struct{
	word p[3];
	word odr;
	sword lroot;
} ARRAY;						/* dictionary structure */

#define ERR_HASHCNT		1
#define ERR_SEARCHCNT	2
#define ERR_LONGCODE	3
#define ERR_HASHORDER	4
#define ERR_BUFFULL		5
#define ERR_SBUFFULL	6
#define ERR_NODIC		7
#define ERR_INVPARAM	8
#define ERR_UNEXPEOF	9
#define ERR_NOMEM		10

#define COMP_NORMAL		0
#define COMP_SPACE		ERR_SBUFFULL
#define COMP_MEMORY		ERR_NOMEM
#define COMP_NORMAL_EOF (ERR_NOMEM+1)
#define COMP_REMAIN		(COMP_NORMAL_EOF+1)
#define COMP_EOB		(-1)

#define PREFETCHMASK	0x3f
#define PREFETCHSIZ		(PREFETCHMASK+1)

#define FILE_ATTRIB(c)	 (c & ATTRIB_DIR ? 1 : 0)

#define BSIZ 4096
#define EOF (-1)

#define		IBUFSIZ		0x4000	// compress input buffer size
#define		OBUFSIZ		0x8000
#define		COMPBUFSIZ	0x4000
#define		COMPRESS_FILE_BUFF_SIZE 1024
#define		ROOT_LEN   3		// Length of a drive-letter rooted name "C:\"
#define		FILEHDR_BANK		0x400


#if defined(_WIN95) && !defined(SYM_NLM)

#if !defined(EZERO)
#define EZERO		0
#endif

#if !defined(ENOTBLK)
#define ENOTBLK		15
#endif

#if !defined(ETXTBSY)
#define ETXTBSY		26
#endif

// FreeBSD's EAGAIN is equal to 35 and it does not have an EUCLEAN definition.
#if !defined(SYM_FREEBSD)
  #if !defined(EUCLEAN)
	#define EUCLEAN		35
  #endif
#endif

#endif


#if !defined(UNIX)
	#ifdef _WIN95	//WIN32
		#define HUGE
	#else
		#define HUGE	huge
	#endif
#endif

#if defined(_ARCMGR_MAIN_)
	#define VAR_ACCESS
#else
	#define VAR_ACCESS extern
#endif


#if _WIN95
#define HIGH(i)	  (i >> 16)
#define LOW(i)	 (i & 0x0000ffff)
#endif

#if _WIN95
#define MAKESTRX(hi, low)	((hi << 16) + low)
#endif


#ifdef MSDOS
#define KANJI_SJIS
#endif

/* DOS */
#define CP_JP			932
#define CP_US			437

#define MAXNFILES 16000

enum {
	CMD_NOTUSE,
	CMD_CREATE,	  /* /A */
	CMD_APPEND,	  /* /A */
	CMD_UPDATE,	  /* /U */
	CMD_EXPAND,	  /* /E */
	CMD_EXTEND,	  /* /X */
	CMD_DELETE,	  /* /D */
	CMD_LISTUP,	  /* /L */
	CMD_SELFEX,	  /* /S */
	CMD_PASSWD,	  /* /P */
	CMD_LAST
};

enum {
	MSG_NOTUSE,
/*001*/    MSG_USAGE,
/*002*/ MSG_NEWPASS,
/*003*/ MSG_RENEWPASS,
/*004*/ MSG_PASS,
/*005*/ MSG_EXISTFILE,
/*006*/ MSG_GUIDEPASS,
	MSG_LAST
};
 
enum {
/*000*/     ERR_NOTUSE,
/*001*/     ERR_INVOPT,
/*002*/     ERR_DUPCMD,
/*003*/     ERR_ABORT,
/*004*/     ERR_NOFILE,
/*005*/     ERR_BADACC,
/*006*/     ERR_ACCDENY,
/*007*/     ERR_TOOOPEN,
/*008*/     ERR_NO_SPACE,
/*009*/     ERR_IO,
/*010*/     ERR_LONGFNAME,
/*011*/     ERR_LONGPASS,
/*012*/     ERR_NOMATCHPASS,
/*013*/     ERR_NOAMG,
/*014*/     ERR_NOAMGSPEC,
/*015*/     ERR_DUPFILE,
/*016*/     ERR_NO_MEM,
/*017*/     ERR_INT01,
/*018*/     ERR_NODIR,
/*019*/     ERR_NODRIVE,
/*020*/     ERR_NOFILESPEC,
/*021*/     ERR_INT02,      /* getcwd()*/
/*022*/     ERR_MAXNFILES,
/*023*/     ERR_MAKEWORK,
/*024*/     ERR_INT03,      /* spencode error */
/*025*/     ERR_INVAMG,     /* illegal .amg format */
/*026*/     ERR_MISSIZ,

#if _WIN95
/*027*/    ERR_VER,
#endif
		ERR_LAST
};

#define	FILE_ID	long

typedef struct {
	char *j;
	char *e;
} MESSAGE;

#define MAXPASSWD 8


/*
 * .AMG
 *
 *         +-------------+
 * +0000   |    magic    |         (1byte)
 *         |-------------|
 * +0001   | passwd flag |         (1byte)    <---- TRUE/FALSE
 *         |-------------|
 * +0002   |   dic loc   | ---+    (4byte)
 *         |-------------|    |
 * +0006   |file cont.EOF|    |    (nbyte)
 *         |-------------|    |
 *         |      :   EOF|    |    (nbyte)
 *         |      :      |    |
 *         |      :   EOF|    |    (nbyte)
 *         |-------------|    |
 *         | dir dic siz |<---+    (2byte)
 *         |-------------|
 *         | dir dic  EOF|         (nbyte)
 *         |-------------|
 *         | str buf siz |         (2byte)
 *         |-------------|
 *         | str buf  EOF|         (nbyte)
 *         |-------------|
 *         | password EOF|         (nbyte)
 *         |-------------|
 *         | file hdr cnt|         (2byte)
 *         |-------------|
 *         |  file hdr   |         (nbyte)
 *         |-------------|
 *         |      :      |         (nbyte)
 *         |      :      |
 *         |      :   EOF|         (nbyte)
 *         +-------------+
 */
/* .AMG */

#if defined(IBMCXX)
#pragma option align=packed	 // AIX
#else
#pragma pack(1)				 // WIN32, AS/400, OS/390, GNU-Solaris, GNU-Linux
#endif

typedef struct {
	struct {
		unsigned short ver : 4; 
		unsigned short magic : 3;
		unsigned short passwd : 1;
		unsigned short x_flag : 1;
		unsigned short os : 4;	 /* OS
						MS-DOS : 0
						SunOS	: 1
						HP-UX	: 2
						Win/NT : 3 */
		unsigned short _reserved : 3;
	} magic;
	dword dicloc;
} AMGHDR;


#if defined(IBMCXX)
#pragma option align=reset	// AIX
#else
#pragma pack()				// WIN32, AS/400, OS/390, GNU-Solaris, GNU-Linux
#endif


#define FNAMMAX				  12
#define FILEHDRVERSION		   2
#define GAGECOLUMN			  49
#define IOBUFSIZ			4096
#define AMG_MAGIC			   5
#define EC_NONE				   0
#define EC_SPLAY_V26		   1
#define EC_STAC				 512
#define OS_DOS				   0
#define OS_SUN				   1
#define OS_HPUX				   2
#define OS_NT				   3

#if defined(IBMCXX)
#pragma option align=packed	 // AIX
#else
#pragma pack(1)				 // WIN32, AS/400, OS/390, GNU-Solaris, GNU-Linux
#endif

typedef struct {
	byte fs_year;	/* since 1900 */
	byte fs_month;	 /* 0-11 */
	byte fs_day;	/* 1-31 */
	byte fs_hour;	/* 0-23 */
	byte fs_min;	/* 0-59 */
	byte fs_sec;	/* 0-59 */
} FILESTAT;

#define FILEHDR_NAME_LEN 12 + 1

typedef struct {
	word mode;
	word dirno;
	FILESTAT ftim;
	dword org_size;
	dword en_size;
	dword loc;
	union {
		dword strx;
		char name[FILEHDR_NAME_LEN];
	} n;
	struct {
		unsigned short drive : 5;
		unsigned short root : 1;
		unsigned short hasdir : 1;
		unsigned short reserve : 1;
		unsigned short encode : 8;	 /*
					*   0
					*   1
					*   512 : Stac
					*/
	} flag;
} FILEHDR;

#if defined(IBMCXX)
#pragma option align=reset	// AIX
#else
#pragma pack()				// WIN32, AS/400, OS/390, GNU-Solaris, GNU-Linux
#endif

/*
		  usr\include\a.h
		  usr\include\b.h
		  usr\include\c.h
		  usr\include\sys\x.h
		  usr\include\sys\y.h
		  usr\include\sys\z.h
		  usr\bin\cc.exe

				  +-----------+
			  --> |   "usr"   |  +-----------+
				  |  dirno 1  -->| "include" |  +----------+
				  +-|---------+  |	dirno 2  -->|	"sys"   |
					V			 +-|---------+	|	dirno 3 --> NULL
					NULL			V			+-|--------+
								 +-----------+	  V
								 |	 "bin"   |   NULL
								 |	dirno 4  --> NULL
								 +-|---------+
									V
								  NULL

*/

typedef struct {
	word no;
	word name;
	word next;
	word child;
} DIRDIC;	

/* Used with: Interrupt 21h Function 4B00h
			 Interrupt 21h Function 4B01h
			 Interrupt 21h Function 4B03h */
typedef struct {
	word exSignature;
	word exExtraBytes;
	word exPages;
	word exRelocItems;
	word exHeaderSize;
	word exMinAlloc;
	word exMaxAlloc;
	word exInitSS;
	word exInitSP;
	word exCheckSum;
	word exInitIP;
	word exInitCS;
	word exRelocTable;
	word exOverlay;
} EXECHEADER;

#ifdef MSDOS
#define get_fileattr(name, mode) _dos_getfileattr(name, mode)
#define set_fileattr(name, mode) _dos_setfileattr(name, mode)
#endif /* MSDOS */

// AMDLL.h

typedef struct {
	LPBYTE	ptr;
	char	Title[256+8];
	BOOL	view;
} VIEW;

typedef VIEW FAR *LPVIEW;        // for win95

//--------------------------------------------------------------

#if _DEBUG
extern	 short int ResetTmpFlg;
#endif

typedef long		ARC_ID;
typedef long		STR_ID;
typedef short int	DIRNO;
typedef long		HDRNO;

#define DEF_SIZE				0x8000	 //(32K)
#define MAXNETWORKNAME			32
#define MAXFILENAME				256
#define DEF_LEN					16

#define MAXPATHNAME				MAXFILENAME
#define MAXFULLPATHNAME			MAXFILENAME

#if _WIN95
#define STR_BLOCK				0x7f00
#endif

#define NESTMAX					32

#define STRTBL_MAX				128
#define HANDLE_MAX				0x20
#define EXEHDR_SIZE				8880

#if _WIN95
#define EXEHDR32_SIZE			12800
#endif

//file name check
#define CHKNAME_NORMAL			0
#define CHKNAME_DUPFILE			1
#define CHKNAME_DUPTIME			2
#define CHKNAME_DUPDIR			4
#define CHKNAME_LONGNAME		8

// write file
#define OVERWRITE_FILE			100
#define NOTOVERWRITE_FILE		101

//read
#define AMG_RAWREAD				1
#define AMG_EXPREAD				2
#define DISK_READ				3

//Copy & Move
#define AMG_COPY				0
#define AMG_MOVE				1

// extract
#define NOTOVERWRITE_RDFILE		110
#define OVERWRITE_NEWTIMEFILE	111

// max check
#define DIR_TOTALMAX			65000
#define FILE_TOTALMAX			65000
#define OBJ_TOTALMAX			65000
#define STR_TOTALMAX			65535

#define SET						TRUE
#define RESET					FALSE
#define ASCII					0x00
#define BINARY					0x01
#define UNIX_LIKE				0x10

#if _WIN95
#define MAKELP(sel, off)		((void FAR*)MAKELONG((off), (sel)))
#endif

#define BANK_MAX				127
#define TOTAL_STRSIZE			32768
#define TOTAL_ARCFILENUM		512
#define TOTAL_DISPNUM			8192
#define FILE_MAX				8192

#define VIEW_MAX				64
#define BINLINE_MAX				400		  // 32K / 75 = 437 (line) ..... 74 = 1 line buffer size

#define BANKID(c)				((WORD)((c & 0xff000000L) >> 24))		//get bank
#define INDEXID(c)				(c & 0x00ffffffL)						//get index
#define MAKEID(b, i)			((STR_ID)b << 24 | i)


enum 
{		
	SORTNAME,
	SORTEXT,
	SORTSIZE,
	SORTDAY
};

typedef struct
{
	short int MaxNameLen;
	char *lpMaxNameStr;
} MAXNAME;


typedef struct _ARCFILEINFO 
{
	char	*lpFileName;
	char	*lpOrgFileName;
	ULONG	OrgSize;
	ULONG	PackSize;
	FILESTAT ftim;
	unsigned short Attrib;
							//    bit0 : 0x01
							//    bit1 : 0x02
							//    bit2 : 0x04
							//    bit3 : 0x08
							//    bit4 : 0x10
							//    bit5 : 0x20
							//    bit6 :
							//    bit7 :
	BOOL	Compress;
	unsigned short encode;
	BOOL	Mark;
	BOOL	deleted;
	long	ChildNum;

	struct _ARCFILEINFO	  FAR *lpNext;
	struct _ARCFILEINFO	  FAR *lpChild;
	struct _ARCFILEINFO	  FAR *lpParent;

	long			loc;
	short int		srcno;
	short int		bank;
} ARCFILEINFO;

typedef ARCFILEINFO FAR *LPFILE;


typedef struct 
{
	LPFILE			lpFile;
	unsigned long	file_count;
	unsigned long	file_next;
	unsigned long	file_deleted;
} MEMFRAME_FILE;

typedef MEMFRAME_FILE FAR *LPMEM_FILE;

typedef struct 
{
	LPFILE			*lpDisp;
	unsigned long	disp_count;
	unsigned long	disp_next;
} MEMFRAME_DISP;

typedef MEMFRAME_DISP FAR *LPMEM_DISP;

typedef struct 
{
	LPBYTE			lpStr;
	unsigned long	str_count;
	unsigned long	str_next;
} MEMFRAME_STR;

typedef MEMFRAME_STR FAR *LPMEM_STR;

typedef struct 
{
	char			*str;
	unsigned long	strlen;
} NAMESTR;

typedef NAMESTR FAR *LPNAMESTR;


typedef struct 
{
	long				nDirTotal;
	DIRDIC*				lpDirBody;
	DIRDIC**			lpUpdateDir;
	long				nFileTotal;
	FILEHDR**			lpFileHandleTbl;
	FILEHDR**			lpUpdateFile;
	unsigned long		strcnt;
	unsigned long		strlen;
	unsigned long		strsize;
	char				*lpDirStr;
} UPDATE_INFO;


#if _WIN95
typedef struct 
{
	char		*lpstr;
	short int	size;
} NAME_BLOCK;

typedef NAME_BLOCK FAR *LPBLOCK;


typedef struct 
{
	NAME_BLOCK	*lpb;
	short int	block;
	int			blocknum;
} NAME_TBL;

#endif

#define MAX_PW_LENGTH 10

#if defined(UNIX)
#define __cdecl
#endif


#ifdef _WINDOWS
		typedef short int (__cdecl *PtrToSortFunction) (LPFILE FAR *lpArcFile1, LPFILE FAR *lpArcFile2);
#else
		typedef short int (*PtrToSortFunction) (LPFILE FAR *lpArcFile1, LPFILE FAR *lpArcFile2);
#endif

typedef struct 
{
	BOOL		used;
	char		FileName[MAXFULLPATHNAME];
	char		Pass[MAX_PW_LENGTH];			// Password
	short int	encode;
	FILESTAT	ftim;
	FILE_ID		DirNum;
	FILE_ID		NewDirNum;
	FILE_ID		FileNum;
	FILE_ID		NewFileNum;
	ULONG		ArcSize;
	ULONG		OrgExeHdrSize;

#if _WIN95
	BOOL		UseLongNameForm;
#endif

	BOOL		SaveDirInfo;
	BOOL		VerboseMode;
	BOOL		AutoUpdate;
	BOOL		SelfExt;
	BOOL		OrigSelfExt;
	BOOL		Password;
	BOOL		OrgPassword;
	BOOL		ReadOnly;
	ARCNO		ArcHandle;
	FILE_ID		CurrFile;
	FILE_ID		DispFileNum;
	FILE_ID		MarkedObjNum;
	LPFILE		lpMarkObjParentTop;

	WORD		wMemFileCount;
	LPMEM_FILE	pArcMemFile;

	WORD		wMemDispCount;
	LPMEM_DISP	pArcMemDisp;
	UPDATE_INFO Update;

#if _WIN95
	NAME_TBL	NameTbl;
	BOOL		NoMessFlg;
#endif

	short int	MaxNestLvl;
	BOOL		sortorder;
	short int	sortmenu;

	PtrToSortFunction	sortfunc;

	short int	handle;
	HWND		hWnd;
	HINSTANCE	hInst;
} ARCINFO;

	
#define ATTRIB_READONLY		0x01
#define ATTRIB_HIDEN		0x02
#define ATTRIB_SYSTEM		0x04
#define ATTRIB_VOLLABEL		0x08
#define ATTRIB_DIR			0x10
#define ATTRIB_NORMAL		0x20
#define DIRBANK_IN_ARC		16
#define DIR_IN_BANK			4096
#define DIR_TOTAL			DIRBANK_IN_ARC * DIR_IN_BANK
#define FILEBANK_IN_ARC		128
#define FILE_IN_BANK		512
#define FILE_TOTAL			FILEBANK_IN_ARC * FILE_IN_BANK

//typedef    LPFILE LPDISP;


/*****************************************************/
//
// GetArcInfo_SeekInfo()
//
typedef struct	
{
	FILE_ID	  CurrFile;
	FILE_ID	  DispFileNum;
} SEEKINFO;					  


//
// GetArcInfo_MemFile()
//
typedef struct	
{
	HGLOBAL		hMemFile;
	WORD		wMemFileCount;
} MEMINFO_FILE;

typedef struct	
{
	HGLOBAL		hMemDisp;
	WORD		wMemDispCount;
} MEMINFO_DISP;					  


// AMMarkObject, AMUnmarkObject
typedef struct 
{
	FILE_ID		  DispFileNum;
	FILE_ID		  MarkedObjNum;
	ARC_ID		  MarkObjParentTop;
} ARCINFO_MARK;


typedef struct 
{
	BOOL	Mark;
	ARC_ID	NextIdx;
	ARC_ID	Child;
	BOOL	hasChild;
	BOOL	Compress;
} MARKINFO;


typedef struct 
{
	long size;			 // file size
	word mode;			 // file mode
	FILESTAT ftim;		 // file date
} FSTAT;

#define ERROR_DETAIL_LENGTH 384

typedef struct 
{
	HINSTANCE	hInst;
	short int	ErrorTbl;
	char		ErrorDetail[ERROR_DETAIL_LENGTH];
} ERRTBL;


typedef ARCINFO FAR			*LPARC;
typedef FILEHDR FAR			*LPHDR;
typedef MAXNAME FAR			*LPMAXNAME;
typedef UPDATE_INFO FAR		*LPUPDATE;
typedef SEEKINFO FAR		*LPSEEK;


#if _WIN95
#define		  __export
#endif


#if defined(_ARCMGR_MAIN_)
	#define VAR_ACCESS
#else
	#define VAR_ACCESS extern
#endif

typedef struct 
{
	char	FileName[32+256+256];
	FILESTAT ftim;
	FILE_ID	  DirNum;
	FILE_ID	  NewDirNum;
	FILE_ID	  FileNum;
	FILE_ID	  NewFileNum;
	long	ArcSize;
	BOOL	SaveDirInfo;
	BOOL	AutoUpdate;
	BOOL	SelfExt;
	BOOL	OrgSelfExt;
	BOOL	Password;

#if _WIN95
	BOOL	OrgPassword;
	BOOL	UseLongNameForm;
	long	OrgSizeT;
	long	PackSizeT;
#endif

} AMINFO;
	
char *		GetArcFileName();
void		GetCreateDay(char *buff);
void		GetFileNum(char *buff);
void		GetDirNum(char *buff);
ARCNO		GetArcHandle();
ARCNO		CreateNewArc();
short int	GetArcInfo();
void		ChangeSaveDirInfo();
short int	GetSaveDirInfo();
void		ChangeAutoUpdate();
short int	GetAutoUpdate();
void		ChangeSelfExt();
short int	GetSelfExt();
void		SetPassword(char *NewPassword);
short int	IsDeclarePassword();
void		DelPassword();
void		OpenArcFile(const char	*ArcName);
void		CloseArcFile(const char *ArcName);

/////////////////////////////////////////////////////////////////////////////
/*
 * Compress.h
 * (C) FUJTISU DEVICES INC. 1994, 1995
 *
 * '95.10.19
 *
 */

#define ISIZ 2048
#define OSIZ (ISIZ*2)

typedef struct {
	/* context tree */
	/* hash table */
	amg_index FAR *HashToIndex2;
	amg_index FAR *HashToIndex1;
	amg_index FAR *HashToIndexESC2;
	amg_index FAR *HashToIndexESC1;
#ifdef _WINDOWS
	HGLOBAL HGL_HashToIndex2;
	HGLOBAL HGL_HashToIndex1;
	HGLOBAL HGL_HashToIndexESC2;
	HGLOBAL HGL_HashToIndexESC1;
#endif /* _WINDOWS */
#ifdef USE_VMLIB
	_vmhnd_t HGL_HashToIndex2;
	_vmhnd_t HGL_HashToIndex1;
	_vmhnd_t HGL_HashToIndexESC2;
	_vmhnd_t HGL_HashToIndexESC1;
#endif /* USE_VMLIB */    

	/* dictionary */
	ARRAY FAR *array_body1;
	ARRAY FAR *array_body2;
	ARRAY FAR * FAR *array;
#ifdef _WINDOWS
	HGLOBAL HGL_array_body1;
	HGLOBAL HGL_array_body2;
	HGLOBAL HGL_array;
#endif /* _WINDOWS */
#ifdef USE_VMLIB
	_vmhnd_t HGL_array_body1;
	_vmhnd_t HGL_array_body2;
	_vmhnd_t HGL_array;
#endif /* USE_VMLIB */

	/* code tree */
	sword FAR *Up;          /* parent branches of code tree */
	sword FAR *Left;        /* children branches of code tree */
	sword FAR *Right;       /* children branches of code tree */
#ifdef _WINDOWS
	HGLOBAL HGL_Up;
	HGLOBAL HGL_Left;
	HGLOBAL HGL_Right;
#endif /* _WINDOWS */
#ifdef USE_VMLIB
	_vmhnd_t HGL_Up;
	_vmhnd_t HGL_Left;
	_vmhnd_t HGL_Right;
#endif /* USE_VMLIB */

	/* stack for input or output 1byte data */
	word BitPos;				/* current bit in byte */
	short int OutByte;			/* for encode variable output byte */
	word Input_code;			/* for decode variable input byte */

	/* enter number of code & context tree */
	amg_index Cnode;	/* number of context nodes */
	sword ISnode;	/* number of internal nodes of code tree */

	/* misc */
	word InByte;				 /* current input byte */
	word PreByte1, PreByte2;	  /* previous input byte */
	sword prefetch[PREFETCHSIZ];
	int fetchptr;
	int outsizechk;
	int dic_alloc_flag;
	LPBYTE inB, outB;
	DWORD Inlen, Outlen;
	DWORD incnt, outcnt;
	short int err;
	short int i, cont_flag;
	word inbyte;
	short int length;
} FLDCSTRUCT, FAR *LPFLDC;

typedef struct {
	FILE *handle;
	FLDCSTRUCT fldc;
	char ibuf[ISIZ];
	long itotal;	/* used bytes */
	short int icnt;
	short int iptr;
	char obuf[OSIZ];
	short int ocnt;
	short int optr;
	char  mode;
	short int amgusing;
	short int eof;
	short int rc;
	DWORD incnt;
	DWORD outcnt;
	DWORD totalsize;
	void (*hook)(short int);
	short int hook_flag;
} DCFILE;

#define MAXDCFILE 1


/*
 * End of `compress.h'
 */
//
//    ERROR TABLE
//
#define		  NORMAL				0

// param
#define		  ERR_HANDLE			30
#define		  ERR_ID				31
#define		  ERR_HANDLEMAX			32

// disk operation
#define		  ERR_DELFILE			39
#define		  ERR_NOFILE			40
#define		  ERR_FSEEK				41
#define		  ERR_FREAD				42
#define		  ERR_FWRITE			43
#define		  ERR_FCLOSE			44
#define		  ERR_EACCESS			45
#define		  ERR_CHDIR				46
#define		  ERR_GETDIR			47
#define		  ERR_ACCESS			48
#define		  ERR_MKDIR				49
#define		  ERR_CHMOD				50
#define		  ERR_NEEDNOFILE		51
#define		  ERR_DIR_FILE			52
#define		  ERR_EMPTY_ARC			53
#define		  ERR_FCREAT			54
#define		  ERR_FOPEN				55
#define		  ERR_DUPNAME			56
#define		  ERR_NOTCOMPFILE		57

// amg
#if _WIN95
#define		  ERR_LONGDIRNAME		59
#endif

#define		  ERR_NOTAMGFILE		60
#define		  ERR_WRONGNAME			61
#define		  ERR_NESTLEVEL			62
#define		  ERR_DIRDICCNT			63
#define		  ERR_FILEADD			64
#define		  ERR_ILGDATA			65
#define		  ERR_ILGPASSWD			66
#define		  ERR_VIEWMAX			67
#define		  ERR_TOOMANY_FILE		68
#define		  ERR_TOOMANY_DIR		69
#define		  ERR_TOOLONG_STR		70
#define		  ERR_FILENOTFOUND		71
#define		  ERR_NONAME			72
#define		  ERR_ILGNAME			73
#define		  ERR_TOOMANY_OBJ		74
#define		  ERR_NOTCOMPRESS		75
#define		  ERR_TOOMANYFILE_INDIR 76

// memory
#define		  ERR_ALLOC				80
#define		  ERR_LOCK				81
#define		  ERR_REALLOC			82
#define		  ERR_WORKMEM			83

// DLL error
#define		  ERR_INNER				90
#define		  ERR_API				91
#define		  ERR_OTHER				99


/////////////////////////////////////////////////////////////////////////////
//    fileinfo.h
/////////////////////////////////////////////////////////////////////////////

#if !defined(MAXFILENAME)
#define MAXFILENAME 256
#endif

typedef struct 
{
	char	FileName[MAXFILENAME];
	long	OrgSize;
	long	PackSize;
	FILESTAT ftim;
	word	Attrib;
	BOOL	Compress;
	BOOL	Empty;
	BOOL	Mark;
	short int NestLevel;
} FILEINFO;


/*
 * FLDC.h
 * V02L01
 * Copyright (C) FUJITSU DEVICES INC. 1995
 */

#ifdef USE_VMLIB
#include <vmemory.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define FUNC_ALLOC 0
#define FUNC_FREE  1
#define FUNC_INIT  2
#define FUNC_COMP  3
#define FUNC_EXP   4
#define FUNC_FLUSH 0x8000

#ifndef _WINDOWS
  #ifdef MSDOS
	#define FAR __far
  #else
	#define FAR
  #endif /* MSDOS */

  #if !defined SYM_LINUX && !defined SYM_NLM
	typedef unsigned char BYTE;
	typedef BYTE FAR *LPBYTE;
	typedef unsigned long DWORD;
	typedef DWORD FAR *LPDWORD;
  #endif /* SYM_LINUX */
#endif /* _WINDOWS */

#ifdef _WIN32
# ifdef _CONSOLE
#  define CLR_MEMORY_ALLOC(m, cast, pointer, num, size)				\
	{																\
		m->pointer = (cast)calloc(num, size);						\
		if (m->pointer == NULL)										\
			return(ERR_NOMEM);										\
	}
#  define MEMORY_ALLOC(m, cast, pointer, size)						\
	{																\
		m->pointer = (cast)malloc(size);							\
		if (m->pointer == NULL)										\
			return(ERR_NOMEM);										\
	}
#  define MEMORY_FREE(m, pointer) free(m->pointer)
#  define DECLFUNC
# else
#  define CLR_MEMORY_ALLOC(m, cast, pointer, num, size)				\
	{																\
		m->HGL_##pointer = (cast)malloc((num) * (size));            \
		if (m->HGL_##pointer == NULL)								\
			return(ERR_NOMEM);										\
		m->pointer = m->HGL_##pointer;								\
	}
#  define MEMORY_ALLOC(m, cast, pointer, size)						\
	{																\
		m->HGL_##pointer = malloc(size);							\
		if (m->HGL_##pointer == NULL)								\
			return(ERR_NOMEM);										\
		m->pointer = m->HGL_##pointer;								\
	}
#  define MEMORY_FREE(m, pointer)									\
	{																\
		if(m->HGL_##pointer)										\
		{															\
			free(m->HGL_##pointer);									\
			m->HGL_##pointer = NULL;								\
		}															\
	}
#  define DECLFUNC 
# endif /* _CONSOLE */
# define AMG_MEMSET(dest, c, count) memset(dest, c, count)
#else
# ifdef _WINDOWS
#  define CLR_MEMORY_ALLOC(m, cast, pointer, num, size)				\
	{																\
		m->HGL_##pointer = (cast)malloc(num) * (size);              \
		if (m->HGL_##pointer == NULL)								\
			return(ERR_NOMEM);										\
		m->pointer = m->HGL_##pointer;								\
	}
#  define MEMORY_ALLOC(m, cast, pointer, size)						\
	{																\
		m->HGL_##pointer = malloc(size);							\
		if (m->HGL_##pointer == NULL)								\
			return(ERR_NOMEM);										\
		m->pointer = m->HGL_##pointer;								\
	}
#  define MEMORY_FREE(m, pointer)									\
	{																\
		if(m->HGL_##pointer)										\
		{															\
			free(m->HGL_##pointer);									\
			m->HGL_##pointer = NULL;								\
		}															\
	}
#  define DECLFUNC 
# else /* _WINDOWS */
#  ifdef USE_VMLIB
#  define CLR_MEMORY_ALLOC(m, cast, pointer, num, size)				\
	{																\
		m->HGL_##pointer = _vmalloc((num) * (size));                \
		if (m->HGL_##pointer == _VM_NULL) {							\
			freedics(m);											\
			return(ERR_NOMEM);										\
		}															\
		m->pointer = (cast)_vlock(m->HGL_##pointer);				\
		if (m->pointer == NULL) {									\
			freedics(m);											\
			return(ERR_NOMEM);										\
		}															\
		_fmemset(m->pointer, 0, ((num) * (size)));                  \
	}
#  define MEMORY_ALLOC(m, cast, pointer, size)						\
	{																\
		m->HGL_##pointer = _vmalloc((size));						\
		if (m->HGL_##pointer == _VM_NULL) {							\
			freedics(m);											\
			return(ERR_NOMEM);										\
		}															\
		m->pointer = (cast)_vlock(m->HGL_##pointer);				\
		if (m->pointer == NULL) {									\
			freedics(m);											\
			return(ERR_NOMEM);										\
		}															\
	}
#  define MEMORY_FREE(m, pointer)									\
	{																\
		if (m->HGL_##pointer) {										\
			while (_vlockcnt(m->HGL_##pointer))						\
				_vunlock(m->HGL_##pointer, _VM_CLEAN);				\
			_vfree(m->HGL_##pointer);								\
		}															\
	}
#  else /* USE_VMLIB */
#	ifndef UNIX
#	define CLR_MEMORY_ALLOC(m, cast, pointer, num, size)			\
	{																\
		m->pointer = (cast)_fcalloc(num, size);						\
		if (m->pointer == NULL)										\
			return(ERR_NOMEM);										\
	}
#	define MEMORY_ALLOC(m, cast, pointer, size)						\
	{																\
		m->pointer = (cast)_fmalloc(size);							\
		if (m->pointer == NULL)										\
			return(ERR_NOMEM);										\
	}
#	define MEMORY_FREE(m, pointer) _ffree(m->pointer)
#	endif
#  endif /* USE_VMLIB */
#  define DECLFUNC
# endif /* _WINDOWS */
# define AMG_MEMSET(dest, c, count) _fmemset(dest, c, count)
#endif /* _WIN32 */

#ifdef UNIX
#  define CLR_MEMORY_ALLOC(m, cast, pointer, num, size)				\
	{																\
		m->pointer = (cast)calloc(num, size);						\
		if (m->pointer == NULL)										\
			return(ERR_NOMEM);										\
	}
#  define MEMORY_ALLOC(m, cast, pointer, size)						\
	{																\
		m->pointer = (cast)malloc(size);							\
		if (m->pointer == NULL)										\
			return(ERR_NOMEM);										\
	}
#  define MEMORY_FREE(m, pointer) free(m->pointer)
#  define DECLFUNC
#endif


typedef struct {
	long offset;
	int orgpoint;
} SEEKPOINT;
#define DCAllocDic(p) \
	FldcEntry(p, (short int)FUNC_ALLOC, (LPBYTE)0, (LPBYTE)0, (LPDWORD)0, (LPDWORD)0)
				
#define DCFreeDic(p) \
	FldcEntry(p, (short int)FUNC_FREE, (LPBYTE)0, (LPBYTE)0, (LPDWORD)0, (LPDWORD)0)

#define DCInitDic(p) \
	FldcEntry(p, (short int)FUNC_INIT, (LPBYTE)0, (LPBYTE)0, (LPDWORD)0, (LPDWORD)0)

#define DCCompress(p, InBuf, OutBuf, InSize, OutSize) \
	FldcEntry(p, (short int)FUNC_COMP, InBuf, OutBuf, InSize, OutSize)

#define DCFlushCompress(p, InBuf, OutBuf, InSize, OutSize) \
	FldcEntry(p, (short int)(FUNC_COMP|FUNC_FLUSH), InBuf, OutBuf, InSize, OutSize)

#define DCExpand(p, InBuf, OutBuf, InSize, OutSize) \
	FldcEntry(p, (short int)FUNC_EXP, InBuf, OutBuf, InSize, OutSize)

#define DCFlushExpand(p, InBuf, OutBuf, InSize, OutSize) \
	FldcEntry(p, (short int)(FUNC_EXP|FUNC_FLUSH), InBuf, OutBuf, InSize, OutSize)

#if defined(MSDOS) || defined(UNIX)
# ifndef __
#  define __(args) args
# endif
#else
# ifndef __
#  define __(args) ()
# endif
#endif

short int DECLFUNC FldcEntry (LPFLDC, short int, LPBYTE, LPBYTE, LPDWORD, LPDWORD);
	
#ifdef __cplusplus
}
#endif
/*
 * End of `fldc.h'
 */

/*
 * Installmaker
 * COPYRIGHT FUJITSU DEVICES INC. 1994
 *
 */
typedef unsigned char FAR *LPUSTR;

#if !defined(UNIX)
typedef unsigned char UCHAR;
#endif

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long dword;
typedef short int ARCNO;
typedef char *PTR;


#if defined(MSDOS) || defined(UNIX)
# ifndef __
#  define __(args) args
# endif
#else
# ifndef __
#  define __(args) ()
# endif
#endif

#if defined(_WINDOWS)
#define ASCII_DIR_SEP "\\"
#define DIR_SEP "\\"
#define DIR_SEP_CHAR '\\'
#define ALL_FILES "*.*"
#endif

#if defined(UNIX)
#define ASCII_DIR_SEP "\x2f"
#define DIR_SEP "/"
#define DIR_SEP_CHAR '/'
#define ALL_FILES "*"
#endif

#if defined(SYM_NLM)
#pragma message ("I don't know if this is correct - check it!")
#define ASCII_DIR_SEP "\\"
#define DIR_SEP "\\"
#define DIR_SEP_CHAR '\\'
#define ALL_FILES "*.*"
#endif


// This class is a wrapper around standard c functions.  The functions
// defined by the previous implementation have been added to this new class
// to address a problem with statics.  By wrapping all the functions into
// a class we automatically get a new implied parameter to each non-static  
// function(the 'this' pointer).  Having the additional parameter allows us
// to easily convert all static data to instance data.  By taking this
// approach we avoid having to change a single line of code in the
// original C functions.  The downside is the class is very ugly.

class AMGImpl {

public:
						AMGImpl();
	virtual				~AMGImpl();

	static
		void			AMG_AbortProcess(bool bAbort);
	static
		void			AMG_SetMaxExtractSize(DWORD dwMaxSize);

		LPFILE			lpAMAddObjectBody (LPARC lpArc, LPARC lpTmpArc, FILE_ID position, BOOL mv, char *DupName);
		short int		CopyTmpFromDisk(LPARC lpArctmp, char *Filename, char *FilePath);
		LPFILE			lpAddObject(LPARC lpDestArc, LPFILE lpDestParentFile, LPFILE lpTmpFile);
		short int		GetMaxNestLevel (LPARC lpArc);
		short int		_GetMaxNestLevel (LPFILE lpArcFile, short int nestlevel, short int FAR *maxnestlevel);
		short int		DCCompressWrite (FILE *fp, LPARC lpArc, char *lpUpdatePath);
		short int		_DCCompressWrite(FILE *fp, LPARC lpArc, LPFILE lpArcFile, long exehdrlen, char *lpUpdatePath, long *dummyFileNo);
		long			CopyWriteFile(LPARC lpArc, FILE *fp, LPFILE lpFile, long *filenum);
		long			_CompressFile(LPARC lpArc, LPFILE lpArcFile, FILE *hCmp, short int *flag, long *filenum, long filetotalnum);
		long			_CompressBuffer(LPARC lpArc, LPBYTE ibuf, DWORD InSize, FILE *hCmp);
		short int		IsNeedCompress (LPARC lpArc, LPFILE lpArcFile, char *lpUpdatePath);
		short int		CompressLargeBuffer (LPARC lpArc, LPBYTE FAR *tblp, DWORD tblsize, long tbltotal, FILE *fp);
		short int		_CompressLargeBuffer (LPARC lpArc, LPBYTE ibuf, DWORD InSize, LPBYTE obuf, FILE *hCmp, FLDCSTRUCT FAR *p);
		short int		CopyNonCompress (LPARC lpArc, char *infile, FILE *ofp, DWORD orgsize, long *filenum, long filetotalnum);
		LPFILE			lpAppendArcFileInfo(LPARC lpArc, LPFILE lpArcFileInfo, LPFILE lpParent);
		short int		_Alloc_ArcFileInfo (LPARC lpArc, LPMEM_FILE lpMemFile);
		void			ClearArcFileInfo (LPFILE lpArcFileInfo);
		LPFILE			_lpAppendArcFileInfo(LPARC lpArc);
		LPFILE			lpEnterRoot (LPARC lpArc);
		void			InitArcInfo(LPARC ap);
		short int		CheckArcHnd(ARCNO an);
		short int		GetStatFromObject(LPARC lpArc, char *path, FSTAT FAR *finfo);
		void			SetSeekPoint(SEEKPOINT FAR *seekpoint, long off, short int org, BOOL setflg);
		void			InitArcFileInfo(LPFILE fp, FILESTAT filestat);
		short int		SetDirDic(LPARC lpArc, LPFILE pd, DIRDIC FAR *dd, char *s);
		void			InputArcFileInfo(LPFILE pdata, FILEHDR FAR *fhdr);
		short int		_Alloc_StrBody (LPARC lpArc, LPMEM_STR lpMemStr);
		char *			lpAppendStr(LPARC lpArc, char *s);
		short int		_Alloc_ArcDispBody (LPARC lpArc, LPMEM_DISP lpMemDisp);
		LPFILE FAR *	_lpAppendDispTbl(LPARC lpArc, LPFILE lpArcFileInfo);
		short int		GetArcNestCount (LPFILE lpArcFile);
		LPFILE			lpGetDroppedParent (LPARC lpArc, FILE_ID dest);
		short int		DeleteBakFile(char *fullpath, short int delflg);
		short int		TruncateBakFile(char *fullpath);
		void			COPYLPFILE(LPFILE lpFile1, LPFILE lpFile2);
		short int		InitArcInfoAll ();
		short int		CreateArcInfo (short int no);
		short int		CreateArcInfoAll ();
		short int		DestroyArcInfoAll ();
		short int		ResetArcInfo (ARCNO an);
		void			FreeDispTbl (LPARC lpArc);
		short int		RemoveStrTbl ();
		short int		CreateStrAll ();
		short int		DestroyStrAll ();
		ARCNO			AMCreate();
		ARCNO			AMOpen(char *amgpath, short int FAR *status);
		short int		_AMOpen(ARCNO an, char *amgpath, short int FAR *status, char *password, short int password_flag);
		ARCNO			AMOpenArc(char *amgpath, short int FAR *status, char *PassWord);
		short int		AMClose(ARCNO handle, char *amgpath);
		short int		AMReadDir(ARCNO handle, FILEINFO FAR *info, short int direction);
		short int		AMSeekDir(ARCNO handle, long offs, long origin);
		FILE_ID			AMTellDir(ARCNO handle);
		short int		AMRewindDir(ARCNO handle);
		short int		AMCompressDir(ARCNO handle, FILE_ID position);
		short int		AMExpandDir(ARCNO handle, FILE_ID position);
		FILE_ID			AMAddObject(ARCNO handle, char *Filename, char *FilePath, FILE_ID position, long FileNum);
		FILE_ID			AMMakeDir(ARCNO handle, char *pDirName, FILE_ID dest);
		FILE_ID			AMRenameObject(ARCNO handle, char *NewFileName, FILE_ID dest);
		long			AMMarkObject(ARCNO handle, FILE_ID dest);
		long			AMMarkAllObjects(ARCNO handle);
		long			AMUnmarkObject(ARCNO handle, FILE_ID dest);
		long			AMUnmarkAllObjects(ARCNO handle);
		short int		AMUpdateArchives(ARCNO handle, char *amgpath,char *pszTempFile, char *AutoPath, short int modify, BOOL Form95);
		short int		AMDeleteMarkedObjects(ARCNO handle);
		short int		AMExtractFile(ARCNO handle, const char *name);
		short int		AMViewMarkedObjects(LPVIEW lpViewBuffer, ARCNO handle, short int view_max, short int view_mode);
		FILE_ID			AMCopyMarkedObjects(ARCNO dest, ARCNO src, FILE_ID position);
		FILE_ID			AMMoveMarkedObjects(ARCNO dest, ARCNO src, FILE_ID position);
		short int		AMSetEncodingMethod(ARCNO handle, short int method);
		short int		AMSetPassword(ARCNO handle, char *passwd);
		short int		AMClearPassword(ARCNO handle);
		short int		AMSetSelfExtract(ARCNO handle, short int flag);
		short int		AMSetVerboseMode(ARCNO handle, short int flag);
		short int		AMSaveWithDirInfo(ARCNO handle, short int flag);
		short int		AMSetAutoUpdate(ARCNO handle, short int flag);
		short int		AMSetSortMethod(ARCNO handle, short int method);
		short int		AMSetSortOrder(ARCNO handle, short int order);
		short int		AMGetAMInfo(ARCNO handle, AMINFO FAR *aminfo);
		long			AMMoveObject(ARCNO handle, FILE_ID dest, FILE_ID src);
		short int		AMGetMaxName(ARCNO handle, short int nestlvl);
		FILE_ID			AMGetSize(ARCNO handle);
		short int		AMGetErrorno(ARCNO an, char *ErrMess, short int FAR *ErrEp);
		long			AMGetMarkedObjNum(ARCNO handle);
		long			AMExpandDirAll(ARCNO handle);
		FILE_ID			AMSearchFile(ARCNO handle, char *FileName);
		FILE_ID			AMGetPositionF(ARCNO handle, char *FileName);
		FILE_ID			AMGetPositionR(HANDLE handle, char *FileName);
		FILE_ID			AMGetPositionR(ARCNO handle, char *FileName);
		long			AMGetLongFileNameNum(ARCNO handle);
		ARC_ID			CollectDispTbl (LPARC lpArc);
		void			lqsort(LPFILE FAR *lpSortFile,short int left,short int right,short int(__cdecl *compare)(LPFILE FAR *lpSorti, LPFILE FAR *lpSortlast));
		short int		_CollectDispTbl(LPARC lpArc, LPFILE FAR *hnd, short int count, short int *NowNestLvl, short int *MaxNestLvl, long *tmpnum);
		void			SwapLPFILE(LPFILE FAR *lpSortFile, short int i, short int j);
		LPFILE			lpCopyToDestFromSrc(LPARC lpArc, LPFILE lpDestFile, LPFILE lpTmpFile, short int flag);
		void			OverwriteArcFileInfo(LPFILE lpDestFile, LPFILE lpSrcFile);
		short int		errno2myerr(int er);
		short int		dcerr2myerr(short int dcerr, int ioerr);
		short int		DCStatus(short int dchnd);
		short int		InitDic(short int fp);
		void			DCInit(void);
		short int		DCInitfileDic(short int dchnd);
		void			DCInitFile(short int dchnd);
		DWORD			DCGetInCnt(short int dchnd);
		DWORD			DCGetOutCnt(short int dchnd);
		short int		DCSetTotalSize(short int dchnd, DWORD size);
		short int		DCSetHookFunction(short int dchnd, void (*func)(short int));
		short int		DCSetHookFlag(short int dchnd, short int flag);
		long			DCSeek(short int dchnd, SEEKPOINT *fpt);
		short int		DCOpenFile(char FAR *path, const char *mode, short int flag);
		short int		DCCloseFile(short int dchnd);
		unsigned short int DCRawRead(short int dchnd, void FAR *ibuf, unsigned short int insiz);
		short int		DCGetChar(short int dchnd);
		long			DCReadFile(short int dchnd, void FAR *buf,long size);
		short int		DCPutChar(short int dchnd, short int c);
		short int		DCWriteFile(short int dchnd, char *buf, short int size);
		short int		DCResetDic(short int dchnd);
		short int		DCDecodeFile(short int dest, short int src, DWORD dsize, void (*hook_func)(short int));
		short int		DCEncodeFile(short int dest, short int src);
		LPFILE			CopyTmpFromAMG(LPARC lpSrcArc, LPARC lpTmpArc, short int FAR *err, short int *FileNum, int cpyflag);
		void			_CopyTmpFromAMG(LPARC lpSrcArc, LPARC lpTmpArc, LPFILE lpSrcMark, LPFILE lpTmpFile, short int flag, short int FAR *err, short int *FileNum, int cpyflag, LPFILE *lpDiskFile);
		long			DeleteMarkedObjects (LPARC lpDelArc);
		short int		DeleteObjectsTree (LPARC lpDelArc, LPFILE lpDelFile);
		short int		DeleteObjectsTreeAll (LPARC lpDelArc, LPFILE lpDir);
		short int		DeleteObjectsOnly (LPARC lpDelArc, LPFILE lpDelFile);
		short int		_DeleteObjectsOnly (LPARC lpDelArc, LPFILE lpDelFile);
		short int		ExpandDirAll(LPARC lpArc);
		short int		_ExpandDirAll(LPFILE lpFile);
		short int		ErrorNeedsDetail (short int err);
		short int		ErrorEpilogue(short int err);
		short int		fileerr2arcerr (short int err);
		short int		returncall(LPARC lpArc, short int err, const char *lpMess);
		void			ClearError(LPARC lpArc);
		short int		_AMExtractSingleObject(LPARC lpArc, const char *name);
		short int		ExtractMarkedObject(LPARC lpExtArc, const char *name, BOOL Longname);
		short int		ExtractSingleObject(LPARC lpExtArc, LPFILE lpExtFile, const char *name);
		short int		_ExtractSingleObject (LPARC lpExtArc, LPFILE lpExtFile, FILE *hIn, const char *name);
		short int		_ExtractSingleObjectCopy(LPARC lpExtArc, LPFILE lpExtFile, FILE *hIn, const char *name);
		short int		make_path(LPARC lpArc, LPUPDATE lpUpdate, DIRDIC FAR *dp, word no, char *strtop, word wTableLength, char *bp, char *bptop);
		LPFILE			lpGetRootPtr (LPARC lpArc);
		LPFILE			lpGetARCPTRFromName(LPARC lpArc, char *localbuffer);
		LPFILE			_SearchFile(LPARC lpArc, char *file, LPFILE lpFile, short int flg);
		FILE_ID			lpSearchFile(LPARC lpArc, char *localbuffer, short int flg);
		FILE_ID			SearchFile(LPARC lpArc, char *fullpath);
		LPFILE			lpGetSearchFile(LPARC lpArc, LPFILE lpFile, char *file);
		LPFILE			GetLPFILE (LPARC lpArc, FILE_ID fid);
		short int		CheckDroppedPosition (LPARC lpArc, FILE_ID position);
		FILE_ID			GetFILEIDFromLPFILE (LPARC lpArc, LPFILE lpArcFile);
		short int		_Mark (LPFILE lpArcFile);
		short int		_UnMark (LPFILE lpArcFile);
		long			GetMarkFileCount(LPARC lpArc, BOOL DirCompressFlg);
		short int		_MarkFileCount(LPFILE lpArcFile, BOOL DirCompressFlg, long FAR *MarkFileNum);
		short int		_GetInFileSize(LPFILE lpArcFile, unsigned long *PackSize, unsigned long *ExtSize);
		short int		_GetFileSize(LPFILE lpArcFile, unsigned long *PackSize, unsigned long *ExtSize);
		short int		CreateDirTbl (LPARC lpArc, LPUPDATE lpUpdate, long DirNum);
		short int		DestroyDirTbl (LPUPDATE lpUpdate);
		short int		CreateFileTbl (LPARC lpArc, LPUPDATE lpUpdate);
		short int		DestroyFileTbl (LPUPDATE lpUpdate);
		LPFILE			lpDirDicNo2ArcNo (LPARC lpArc, DIRDIC FAR *DirDicTop, LPBYTE strtop, word wTableLength, word DirNo);
		short int		_OpenArcFile(short int fp, LPARC lpArc, SEEKPOINT *seekpoint);

		PtrToSortFunction	
						Selectsortfunc (short int smenu, BOOL sorder);
		static short int		Comp_name_up (LPFILE FAR *lpArcFile1, LPFILE FAR *lpArcFile2);
		static short int		Comp_name_down (LPFILE FAR *lpArcFile1, LPFILE FAR *lpArcFile2);
		static short int		Comp_ext_up (LPFILE FAR *lpArcFile1, LPFILE FAR *lpArcFile2);
		static short int		Comp_ext_down (LPFILE FAR *lpArcFile1, LPFILE FAR *lpArcFile2);
		static short int		Comp_size_up (LPFILE FAR *lpArcFile1, LPFILE FAR *lpArcFile2);
		static short int		Comp_size_down (LPFILE FAR *lpArcFile1, LPFILE FAR *lpArcFile2);
		static short int		Comp_date_up (LPFILE FAR *lpArcFile1, LPFILE FAR *lpArcFile2);
		static short int		Comp_date_down (LPFILE FAR *lpArcFile1, LPFILE FAR *lpArcFile2);
		static short int		CompDate(FILESTAT FAR *pfs1, FILESTAT FAR *pfs2);
		static int				__StrLen(const char *ss);
		static char *			__StrCat(char *s1, const char *s2);
		static char *			__StrCpy(char *s1, const char *s2);
		static int				__StrCmp(char *s1, const char *s2);
		static int				__StrCmpi(char *s1, const char *s2);
		static LPBYTE			GetExtPoint(char *lp);

#if _WIN95
		short int		GetStrLong(char *lpFilename);
		short int		DestoryFileNameTbl (NAME_TBL FAR *lpTbl);
		short int		CreateFileNameTbl (LPARC lpArc);
		short int		CheckFilenameExpression(char *NewFileName);
		long			GetLongNameNum(ARCNO handle);
		short int		GetFileTotalSize(LPARC lpArc, unsigned long *PackSize, unsigned long *ExtSize);
		int				CountView(LPVIEW lpViewbuffer);
		STR_ID			AppendStrTbl(NAME_TBL FAR *lpTbl, char *name);
		short int		AMSetMessageBox(ARCNO handle, BOOL dlgflg);
		short int		AMCheckDirStr(ARCNO handle);

#endif
		bool			AMIsEmptyArchive (ARCNO handle, char *amgpath);
		short int		AMGetExtractSize(ARCNO handle, unsigned long *PackSize, unsigned long *ExtSize);

		void			CutBaseName (char *path);
		void			CutBaseNameASCII (char *path);
		void			getpath(char *p, char *k, short int n);
		short int		GetNestCount(char *localbuffer);
		short int		_GetFullPathName (LPARC lpArc, LPFILE lpArcFile, char *lpBuf);
		char *			getstrptr(short int idx, char *p, word wTableLength);
		LPFILE			lpCheckName (LPFILE lpArcFile, char *lpNewFileName);
		short int		GetBaseOffset(char *lpFilename);
		short int		GetNestCountByFile(LPFILE lpFile);
		LPFILE			lpCheckFileName(LPARC lpArc, LPFILE lpDestFile, LPFILE lpTmpFile, short int FAR *DupFlg);
		long			_GetLongNameNum(LPFILE lpFile, long *num);
		int				__DirStrLen(LPFILE lpFile);
		bool			MakeTmpFile (char *base, char *buf);
		short int		CompressLargeTbl(LPARC lpArc, FILE *fp);
		short int		_CompressLargeTbl(LPARC lpArc, char *NameTbl, DWORD InSize, LPBYTE obuf, FILE *hCmp, FLDCSTRUCT *p);
		short int		CompressLargeDirTbl(LPARC lpArc, FILE *fp);
		short int		_CompressLargeDirTbl(LPARC lpArc, DWORD InSize, LPBYTE obuf, FILE *hCmp, FLDCSTRUCT *p);
		short int		UpdateArchivesBody (ARCNO handle, char *amgpath, char *pszTempFile, char *lpUpdatePath, short int modify, BOOL Form95);
		short int		UpdateArchives(LPARC lpUpdateArc, char *outfile, char *pszTempFile, char *UpdatePath, BOOL Form95);
		short int		RemoveAMG(LPARC lpUpdateArc, BOOL Form95);
		short int		AppendAMG(LPARC lpUpdateArc, BOOL Form95);
		short int		_AppendAMG (LPARC lpUpdateArc, LPFILE lpUpdateFile, DIRNO did, char *Upbuf, long *hdrcnt, short int *dirno, BOOL Form95);
		FILE_ID			def_file(LPARC lpUpdateArc, char *name, LPFILE pf, DIRNO did, long hdrcnt, BOOL Form95);
		DIRNO			def_dir(char *s, LPUPDATE lpUpdate, short int dirno);
		ARC_ID			SearchDirname(char *my, LPUPDATE lpUpdate, short int dirno);
		LPFILE			lpSearchArcDirectory (char *lpFileName, LPFILE lpDest);
		short int		recover_dir_condition (ARCNO an);
		short int		_recover_dir_condition (LPARC lpArc, LPFILE lpArcSrc, LPFILE lpDstParent);
		short int		WriteAccessCheck (char *filename);
		short int		CheckFileNameLength (LPARC lpUpdateArc);
		short int		_CheckFileNameLength (LPARC lpArc, LPFILE lpArcFile, short int path_len);
		short int		CountAutoUpdate (LPARC lpUpdateArc, char *lpUpPath);
		short int		_CountAutoUpdate (LPARC lpArc, LPFILE lpArcFile, char *lpUpPath);
		short int		ShouldCompress (LPARC lpArc, LPFILE lpArcFile, char *lpUpPass);
		unsigned long	CheckDirStr(LPARC lpArc);
		short int		_CheckDirStr(LPARC lpUpdateArc, LPFILE lpUpdateFile, DIRNO did, char *Upbuf, short int *dirno);
		short int		CopyArcInfo(ARCNO Dest, ARCNO Src);
		BOOL			IsKanji(unsigned char c);

private:
		// Previously these data fields were statics.

		DCFILE		dc[MAXDCFILE];
		char		dcbuf[BSIZ];
		LPMEM_STR	lpStrTbl;// = NULL;
		word		wStrNum;
		word		wArcNum;
		long		marknum;
		short int	abort_flag;			// 0: init 1: OK 2:CANCEL

		ERRTBL		lpError[HANDLE_MAX + 1];
		LPARC		lpArcTbl[HANDLE_MAX * 2];
		short int	ext_operation;
		short int	ErrorTbl;
		char		ErrorDetail[512];			// [taro:1995.12.5]
};

#endif
/*
 * End of `AMGImpl.h'
 */
