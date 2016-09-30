//ndkFiles.h

#ifndef _ndkFiles_h_
#define _ndkFiles_h_

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#include "ndkDefinitions.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NetWareErrno errno
#define O_BINARY 0
#define O_TEXT 0
#define wsprintf sprintf
#define ConsolePrintf Real_dprintf

//NDK Multiple and Intra-File Services
int AddTrustee(char *pathName , LONG trusteeObjectID, WORD trusteeRightsMask);
int DeleteTrustee(char *pathName , LONG trusteeObjectID);
BOOL ConvertFileName(char *output , char *input,int len , char inputNS , char outputNS);
LONG FESetCWVnum(LONG CWVnum);
int FileServerFileCopy  (   
   int    sourceFileHandle,    
   int    destinationFileHandle,    
   LONG   sourceFileOffset,    
   LONG   destinationFileOffset,    
   LONG   numberOfBytesToCopy,    
   LONG  *numberOfBytesCopied); 
int  ParsePath  (   
   char   *path,    
   char   *server,    
   char   *volume,    
   char   *directories);
DWORD NWParsePath
(
	const char*    path,
	char*          server,
	NWCONN_HANDLE* conn,
	char*          volName,
	char*          dirPath
);

int SetFileInfo(const char* sFilePath,const struct stat* pStat);

LONG NWAddFSMonitorHook  (   
   LONG   callBackNumber,    
   void   *callBackFunc,    
   LONG   *callBackHandle);

LONG NWRemoveFSMonitorHook (
   LONG   callBackNumber,
   LONG   callBackHandle); 

int NWGetNameSpaceEntryName  (   
   BYTE   *path,    
   LONG   nameSpace,    
   LONG   maxNameBufferlength,    
   BYTE   *nameSpaceEntryName); 

int NWSetNameSpaceEntryName  (      
   BYTE   *path,
   LONG   nameSpace,
   BYTE   *nameSpaceEntryName); 

BYTE SetCurrentNameSpace  (BYTE   newNameSpace);
BYTE SetTargetNameSpace  (BYTE   newNameSpace); 
void UseAccurateCaseForPaths (int   yesno);

//in decomp.cpp
//int FEsopen(char *name , int access , int share , int permission , int flagBits , BYTE dataStraem);

//NDK Single and Intra-File Services
DWORD GetEA(char *name , char *key , BYTE *Data , DWORD Len);
DWORD SetEA(char *name , char *key , BYTE *Data , DWORD Len);
int flushall(void);

//NDK Misc. Services
void RingTheBell (void);
char *NWNextChar( const char * );

//in-house, ported NLM functions

int PurgeFiles(const char *FileName);
int DeletePurgeFile( const char *FileName );

#ifdef __cplusplus
}
#endif

#endif // _ndkFiles_h_
