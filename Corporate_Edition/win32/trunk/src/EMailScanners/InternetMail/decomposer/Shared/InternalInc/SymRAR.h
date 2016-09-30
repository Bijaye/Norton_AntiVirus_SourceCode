// SymRAR.h : Interface definitions for RAR engine
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2002, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(SYMRAR_H)
#define SYMRAR_H

#define	MAX_RAR_PW_LEN		128

#define ERAR_END_ARCHIVE	10
#define ERAR_NO_MEMORY		11
#define ERAR_BAD_DATA		12
#define ERAR_BAD_ARCHIVE	13
#define ERAR_UNKNOWN_FORMAT	14
#define ERAR_EOPEN			15
#define ERAR_ECREATE		16
#define ERAR_ECLOSE			17
#define ERAR_EREAD			18
#define ERAR_EWRITE			19
#define ERAR_SMALL_BUF		20
#define ERAR_UNKNOWN		21
#define ERAR_ABORT			22
#define ERAR_MAX_EXTRACT	23
// Added to flag errors for split archives
#define ERAR_SPLIT_BEFORE	24
#define ERAR_SPLIT_AFTER	25
// Added to detect passworded files
#define ERAR_CHILD_DECRYPT	26

#define RAR_OM_LIST           0
#define RAR_OM_EXTRACT        1

#define RAR_SKIP              0
#define RAR_TEST              1
#define RAR_EXTRACT           2

#define RAR_VOL_ASK           0
#define RAR_VOL_NOTIFY        1

#define RAR_DLL_VERSION       2

struct RARHeaderData
{
  char         ArcName[260];
  char         FileName[260];
  unsigned int Flags;
  unsigned int PackSize;
  unsigned int UnpSize;
  unsigned int HostOS;
  unsigned int FileCRC;
  unsigned int FileTime;
  unsigned int UnpVer;
  unsigned int Method;
  unsigned int FileAttr;
  char         *CmtBuf;
  unsigned int CmtBufSize;
  unsigned int CmtSize;
  unsigned int CmtState;
};


struct RARHeaderDataEx
{
  char         ArcName[1024];
  wchar_t      ArcNameW[1024];
  char         FileName[1024];
  wchar_t      FileNameW[1024];
  unsigned int Flags;
  unsigned int PackSize;
  unsigned int PackSizeHigh;
  unsigned int UnpSize;
  unsigned int UnpSizeHigh;
  unsigned int HostOS;
  unsigned int FileCRC;
  unsigned int FileTime;
  unsigned int UnpVer;
  unsigned int Method;
  unsigned int FileAttr;
  char         *CmtBuf;
  unsigned int CmtBufSize;
  unsigned int CmtSize;
  unsigned int CmtState;
  unsigned int Reserved[1024];
};


struct RAROpenArchiveData
{
  char         *ArcName;
  unsigned int OpenMode;
  unsigned int OpenResult;
  char         *CmtBuf;
  unsigned int CmtBufSize;
  unsigned int CmtSize;
  unsigned int CmtState;
};

enum UNRARCALLBACK_MESSAGES
{
	UCM_CHANGEVOLUME,
	UCM_PROCESSDATA,
	UCM_NEEDPASSWORD
};

typedef int (*UNRARCALLBACK)(UINT msg, void * UserData, DWORD P1, DWORD P2);

typedef int (*CHANGEVOLPROC)(char *ArcName,int Mode);
typedef int (*PROCESSDATAPROC)(unsigned char *Addr,int Size);

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////
// RAR engine interface Functions
void	RAR_SetMaxExtractSize(DWORD dwMaxSize);
void	RAR_AbortProcess(bool bAbort);
void	RARInit(void);
void *	RAROpenArchive(struct RAROpenArchiveData *ArchiveData);
int		RARCloseArchive(void * hArcData);
int		RARReadHeader(void * hArcData, struct RARHeaderData *HeaderData);
int		RARReadHeaderEx(void * hArcData, struct RARHeaderDataEx *HeaderData);
int		RARProcessFile(void * hArcData, int Operation, const char *DestName);
void	RARSetCallback(void * hArcData, UNRARCALLBACK Callback, void * UserData);
void	RARSetChangeVolProc(void * hArcData, CHANGEVOLPROC ChangeVolProc);
void	RARSetProcessDataProc(void * hArcData, PROCESSDATAPROC ProcessDataProc);
void	RARSetPassword(void * hArcData, const char *Password);
int		RARGetDllVersion();

#ifdef __cplusplus
}
#endif

#endif	// SYMRAR_H

