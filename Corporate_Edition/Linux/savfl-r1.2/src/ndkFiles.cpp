//ndkFiles.cpp

extern "C" {

#include <utime.h>
#include "ndkFiles.h"
#include "vpstrutils.h"

/*******************************************************************************************************/
// This NDK API is only called by FileName Conversion routines which 
//are not needed on Linux, however, are provided here for easing the port maintenance and build
LONG FESetCWVnum(LONG CWVnum)
{
  return CWVnum;
}
/*******************************************************************************************************/
//Linux doesnt support DOS or LONG name space file format and as such name conversion 
//is not required. So, jsut return the original name.
BOOL ConvertFileName(char *output , char *input , int len ,char inputNS , char outputNS)
{
  strcpy(output , input);
  return true;
}
/*******************************************************************************************************/
//this function emulates (in the way we use the call) netware's file copy command
//we ignore source and destination offsets becase they are always zero,
//meaning that we always copy the whole file when we call this API.
//return 0 if success, otherwise return errno.
//this call is different from Win32 CopyFile in checking for overwrite.
//the caller function has openned both files
//the caller has set the read for source and write for destination modes when open
//NOTE: we might have to implement the win32 version of CopyFile command later but not 
//when this API is used.
int FileServerFileCopy  (   
   int    sourceFileHandle,    
   int    destinationFileHandle,    
   LONG   sourceFileOffset,    
   LONG   destinationFileOffset,    
   LONG   numberOfBytesToCopy,    
   LONG  *numberOfBytesCopied)
{
  ssize_t byte_count;

  BYTE *buffer = NULL;

  //
  // Allocate memory...
  //
  buffer = new BYTE[numberOfBytesToCopy];
  if (!buffer)
    return ERROR_MEMORY;
  //read
  byte_count = read(sourceFileHandle , buffer , numberOfBytesToCopy);
  if (byte_count == -1)
    return errno;
  //write
  byte_count = write(destinationFileHandle , buffer , numberOfBytesToCopy);
  if (byte_count == -1)
    return errno;
  // return 0 on success
  return  ERROR_SUCCESS;
}
/*******************************************************************************************************/
//these apis will be replaced by AP for Linux. I'll just define them here to take care of any compile issues and return 0
LONG NWAddFSMonitorHook  (   
   LONG   callBackNumber,    
   void   *callBackFunc,    
   LONG   *callBackHandle)
{
    return ERROR_SUCCESS;
}
LONG NWRemoveFSMonitorHook (
   LONG   callBackNumber,
   LONG   callBackHandle) 
{
    return ERROR_SUCCESS;
}
/*******************************************************************************************************/
//following Linux functions (or something like them coded) must be called to match what we do in Win32 to replace the following NetWare API:
//SetFIleAttributes()
//GetFileAttributes()
//SetFileDate()
//GetFileDate()
//SetFIleState()
//GetFileState()

//This API is used to set NetWare attributes and flags, plus dos time, etc when setting purge flags and other nlm
//specific file manipulation.
//For Linux, there will be no purge flag set when deleting a file, and to make a file writable, we'll just set the mode if needed.
//when needed, for setting file time, mode, or permission, Linux will have to use SetFileAttributes(), and chmod() same as Win32

// The NetWare `st_attr' has no role play on a Linux system where RTVScan is running as
// `root' -- it does alread have read-write acces to all files, no need to change any
// file modes. `st_attr' and `st_mode' are disregarded. Ditto `st_gid' -- it is not used
// anywhere.

int SetFileInfo(const char* sFilePath,const struct stat* pStat) {
	struct utimbuf fileTimes;
	fileTimes.actime = pStat->st_atime;
	fileTimes.modtime =  pStat->st_mtime;

	int retVal = utime(sFilePath, &fileTimes) || chown(sFilePath, pStat->st_uid, static_cast<gid_t>(-1));

	return retVal;
}

/*******************************************************************************************************/
//These APIs are used for handling filename irregularities on netware and will not be
// needed on Linux so we just return success.
int NWGetNameSpaceEntryName  (   
   BYTE   *path,    
   LONG   nameSpace,    
   LONG   maxNameBufferlength,    
   BYTE   *nameSpaceEntryName)
{
    return ERROR_SUCCESS;
}
int NWSetNameSpaceEntryName  (      
   BYTE   *path,
   LONG   nameSpace,
   BYTE   *nameSpaceEntryName) 
{
    return ERROR_SUCCESS;
}
BYTE SetCurrentNameSpace  (BYTE   newNameSpace)
{
    return newNameSpace;
}
BYTE SetTargetNameSpace  (BYTE   newNameSpace) 
{
    return newNameSpace;
}
void UseAccurateCaseForPaths (int   yesno)
{
}
/*******************************************************************************************************/
//This API is used for handling filenale irregularities on netware and will not be
// needed on Linux so we just return success.
DWORD NWParsePath
(
	const char*    path,
	char*          server,
	NWCONN_HANDLE* conn,
	char*          volName,
	char*          dirPath
)
{
  return ERROR_SUCCESS;
}
/*******************************************************************************************************/
//This API is used for handling filenale irregularities on netware and will not be
// needed on Linux so we just return success.
int  ParsePath  (   
   char   *path,    
   char   *server,    
   char   *volume,    
   char   *directories)
{
  return ERROR_SUCCESS;
}
/*******************************************************************************************************/
//these call are for novell authentication and are not required on Linux, however,
//we'll just return ERROR_SUCCESS to take care of any possible common code support issues.
int AddTrustee(char *pathName , LONG trusteeObjectID, WORD trusteeRightsMask)
{
  return ERROR_SUCCESS;
}
int DeleteTrustee(char *pathName , LONG trusteeObjectID)
{
  return ERROR_SUCCESS;
}
/*******************************************************************************************************/
DWORD SetEA(char *name , char *key , BYTE *Data , DWORD Len)
{
  int fh;
  DWORD cc = ERROR_OPEN_FAIL;
// LINUX_CHANGE: FIXED FILEPATH
  char path[IMAX_PATH+100];
  ssize_t bytesWritten;

  vpsnprintf(path, sizeof(path), "%s:%s", name, key);
  fh = open(path , O_WRONLY | O_CREAT , S_IWUSR);
  if (fh != EFAILURE)
  {
    bytesWritten = write( fh, Data, Len );
    if (bytesWritten == static_cast<ssize_t>(Len))
      cc = ERROR_SUCCESS;
    if(close(fh) == 0 )
      cc = ERROR_SUCCESS;
  }
  return cc;
}
/*******************************************************************************************************/
DWORD GetEA(char *name , char *key , BYTE *Data  , DWORD Len)
{
  int fh;
// LINUX_CHANGE: FIXED FILEPATH
  char path[IMAX_PATH+100];
  DWORD cc = ERROR_OPEN_FAIL;
  ssize_t bytesRead;

  vpsnprintf(path, sizeof(path), "%s:%s", name, key);
  fh = open(path ,O_RDONLY | O_CREAT , S_IRUSR);
  if (fh != EFAILURE)
  {
    // TODO read might only return partial values
    bytesRead = read( fh, Data, Len );
    if (bytesRead == static_cast<ssize_t>(Len))
      cc = ERROR_SUCCESS;
    if(close(fh) == 0 )
      cc = ERROR_SUCCESS;
  }
  return cc;
}
/***********************************************************************************************/
//this function is defined as fd in decomp.cpp and is only called there
//so I will leave this commented out as reference and will code same as netware and
//win32 in decomp.cpp only.
/*
int FEsopen(char *name , int access , int share , int permission , int flagBits , BYTE dataStraem)
{
  int han;
  han = open(name ,O_RDONLY | O_CREAT , S_IRUSR);
  return han;
}
*/
/***********************************************************************************************/
int flushall  (void) {

  return fflush(NULL);
}

int PurgeFiles(const char *FileName){

  return remove(FileName);
}

int DeletePurgeFile(const char *FileName){

  return remove(FileName);
}

void RingTheBell ( void ){
}

char TransportSystemOK = TRUE;

}
