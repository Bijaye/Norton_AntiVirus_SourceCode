// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/AVCB.H_v   1.3   01 May 1997 11:26:22   MKEATIN  $
//
// Description:
//  AVAPI 2.0 cookie tray and callback structure definitions.
//
// Contains:
//
// See Also:
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/AVCB.H_v  $
// 
//    Rev 1.3   01 May 1997 11:26:22   MKEATIN
// Changed a char to a TCHAR for unicode compatability.
// 
//    Rev 1.2   18 Apr 1997 13:44:12   MKEATIN
// Latest header from NEWVIR/INCLUDE
// 
//    Rev 1.4   20 Nov 1996 15:20:58   AOONWAL
// No change.
// 
//    Rev 1.3   23 Oct 1996 11:18:06   DCHI
// Added IPC callback structure definition.
// 
//    Rev 1.2   22 Oct 1996 12:09:26   AOONWAL
// No change.
// 
//    Rev 1.1   04 Jun 1996 17:33:12   DCHI
// Changed LinearIsHardDisk to LinearIsPartitionedMedia.
// 
//    Rev 1.0   04 Jun 1996 12:57:14   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _AVCB_H

#define _AVCB_H

#include "avtypes.h"

//////////////////////////////////////////////////////////////////////
// CBSTATUS type definition
//////////////////////////////////////////////////////////////////////

typedef unsigned int CBSTATUS;

// CBSTATUS return values

#define     CBSTATUS_OK         0
#define     CBSTATUS_ERROR      1
#define     CBSTATUS_ABORT      2       // application wants to abort scan

typedef struct
{
    HFILE   hFile;

    TCHAR   lpstrPathName[SYM_MAX_PATH+1];
} FILEINFO_T, FAR *LPFILEINFORMATION;

//////////////////////////////////////////////////////////////////////
// Cookie tray structure definition
//////////////////////////////////////////////////////////////////////

typedef struct
{
    DWORD       dwGlobalCookie;
    DWORD       dwLocalCookie;
    DWORD       dwTransientCookie;
} COOKIETRAY_T, FAR *LPCOOKIETRAY;


//////////////////////////////////////////////////////////////////////
// Memory callbacks
//////////////////////////////////////////////////////////////////////

typedef struct tagMEMORYCALLBACKS
{
    CBSTATUS (*MemGetNumRegionsToScan)
    (
        LPVOID          lpvMemoryInfo,
        LPDWORD         lpdwNumRegionsToScan,
        LPCOOKIETRAY    lpCookieTray
    );

    CBSTATUS (*MemGetRegionInfo)
    (
        LPVOID          lpvMemoryInfo,
        DWORD           dwRegionToScan,
        LPDWORD         lpdwStartAddress,
        LPDWORD         lpdwMemSize,
        LPCOOKIETRAY    lpCookieTray,
        LPBOOL          lpbValidRegion
    );

    CBSTATUS (*MemGetPtrToMem)
    (
        LPVOID          lpvMemoryInfo,
        DWORD           dwMemAddress,
        LPBYTE          lpbyMemBuf,
        DWORD           dwMemSize,
        LPLPBYTE        lplpbyMemPtr,
        LPCOOKIETRAY    lpCookieTray
    );

    CBSTATUS (*MemReleasePtrToMem)
    (
        LPVOID          lpvMemoryInfo,
        LPBYTE          lpbyMemPtr,
        LPCOOKIETRAY    lpCookieTray
    );
} MEMORYCALLBACKS_T, FAR *LPMEMORYCALLBACKS;


//////////////////////////////////////////////////////////////////////
// Linear callbacks
//////////////////////////////////////////////////////////////////////

typedef struct tagLINEARCALLBACKS
{
    CBSTATUS (*LinearOpen)
    (
        LPVOID          lpvLinearInfo,
        WORD            wOpenMode
    );

    CBSTATUS (*LinearClose)
    (
        LPVOID          lpvLinearInfo
    );

    CBSTATUS (*LinearRead)
    (
        LPVOID          lpvLinearInfo,
        DWORD           dwSectorLow,
        DWORD           dwSectorHigh,
        LPBYTE          lpbyBuffer,
        DWORD           dwLength,
        LPDWORD         lpdwActualLength
    );

    CBSTATUS (*LinearWrite)
    (
        LPVOID          lpvLinearInfo,
        DWORD           dwSectorLow,
        DWORD           dwSectorHigh,
        LPBYTE          lpbyBuffer,
        DWORD           dwLength,
        LPDWORD         lpdwActualLength
    );

    CBSTATUS (*LinearGetDimensions)
    (
        LPVOID          lpvLinearInfo,
        LPBYTE          lpbySides,
        LPWORD          lpwCylinders,
        LPBYTE          lpbySectors
    );

    CBSTATUS (*LinearConvertFromPhysical)
    (
        LPVOID          lpvLinearInfo,
        BYTE            bySide,
        WORD            wCylinder,
        BYTE            bySector,
        LPDWORD         lpdwSectorLow
    );

    CBSTATUS (*LinearConvertToPhysical)
    (
        LPVOID          lpvLinearInfo,
        DWORD           dwSectorLow,
        LPBYTE          bySide,
        LPWORD          wCylinder,
        LPBYTE          bySector
    );

    CBSTATUS (*LinearIsPartitionedMedia)
    (
        LPVOID          lpvLinearInfo,
        LPBOOL          lpbIsPartitionedMedia
    );
} LINEARCALLBACKS, FAR * LPLINEARCALLBACKS;


//////////////////////////////////////////////////////////////////////
// File callbacks
//////////////////////////////////////////////////////////////////////

typedef struct tagFILECALLBACKS
{
    CBSTATUS (*FileOpen)
    (
        LPVOID          lpvFileInfo,
        WORD            wOpenMode
    );

    CBSTATUS (*FileClose)
    (
        LPVOID          lpvFileInfo
    );

    CBSTATUS (*FileSeek)
    (
        LPVOID          lpvFileInfo,
        long            lOffset,
        WORD            wWhence,
        LPDWORD         lpdwNewOffset
    );

    CBSTATUS (*FileRead)
    (
        LPVOID          lpvFileInfo,
        LPVOID          lpvBuffer,
        DWORD           dwNumBytes,
        LPDWORD         lpdwNumActualBytes
    );

    CBSTATUS (*FileWrite)
    (
        LPVOID          lpvFileInfo,
        LPVOID          lpvBuffer,
        DWORD           dwNumBytes,
        LPDWORD         lpdwNumActualBytes
    );

    CBSTATUS (*FileTell)
    (
        LPVOID          lpvFileInfo,
        LPDWORD         lpdwOffset
    );

    CBSTATUS (*FileSetEOF)
    (
        LPVOID          lpvFileInfo
    );

    CBSTATUS (*FileEOF)
    (
        LPVOID          lpvFileInfo,
        LPBOOL          lpbEOF
    );

    CBSTATUS (*FileGetAttr)
    (
        LPVOID          lpvFileInfo,
        WORD            wType,
        LPDWORD         lpdwInfo
    );

    CBSTATUS (*FileSetAttr)
    (
        LPVOID          lpvFileInfo,
        WORD            wType,
        DWORD           dwInfo
    );

    CBSTATUS (*FileGetDateTime)
    (
        LPVOID          lpvFileInfo,
        WORD            wType,
        LPWORD          lpwDate,
        LPWORD          lpwTime
    );

    CBSTATUS (*FileSetDateTime)
    (
        LPVOID          lpvFileInfo,
        WORD            wType,
        WORD            wDate,
        WORD            wTime
    );

    CBSTATUS (*FileFullNameAndPath)
    (
        LPVOID          lpvFileInfo,
        LPWSTR          lpwstrFileName,
        WORD            wBufferLen
    );

    CBSTATUS (*FileName)
    (
        LPVOID          lpvFileInfo,
        LPWSTR          lpwstrFileName,
        WORD            wBufferLen
    );

    CBSTATUS (*FilePath)
    (
        LPVOID          lpvFileInfo,
        LPWSTR          lpwstrFilePath,
        WORD            wBufferLen
    );

    CBSTATUS (*FileLogicalDrive)
    (
        LPVOID          lpvFileInfo,
        LPWSTR          lpwstrDriveLetter,
        WORD            wBufferLen
    );

    CBSTATUS (*FileLength)
    (
        LPVOID          lpvFileInfo,
        LPDWORD         lpdwFileLength
    );

    CBSTATUS (*FileDelete)
    (
        LPVOID          lpvFileInfo
    );

    CBSTATUS (*FileRename)
    (
        LPVOID          lpvFileInfo,
        LPWSTR          lpwstrNewName
    );

    CBSTATUS (*FileGetErrorCondition)
    (
        LPVOID          lpvFileInfo,
        LPDWORD         lpdwErrorCondition
    );
} FILECALLBACKS_T, FAR *LPFILECALLBACKS;


//////////////////////////////////////////////////////////////////////
// General callbacks
//////////////////////////////////////////////////////////////////////

typedef struct tagGENERALCALLBACKS
{
    LPLINEARCALLBACKS   lpLinearCallBacks;
    LPFILECALLBACKS     lpFileCallBacks;

    CBSTATUS (*MemoryAlloc)
    (
        DWORD           dwSize,
        WORD            wMemType,
        LPLPVOID        lplpvBuffer
    );

    CBSTATUS (*MemoryFree)
    (
        LPVOID          lpvBuffer
    );

    CBSTATUS (*CreateFileInfoDistinct)
    (
        LPLPVOID        lplpvFileInfo,
        LPWSTR          lpwstrDriveLetter,
        LPWSTR          lpwstrFilePath,
        LPWSTR          lpwstrFileName
    );

    CBSTATUS (*CreateFileInfo)
    (
        LPLPVOID        lplpvFileInfo,
        LPWSTR          lpwstrFullPathName
    );

    CBSTATUS (*DestroyFileInfo)
    (
        LPVOID          lpvFileInfo
    );

    CBSTATUS (*CreateLinearInfo)
    (
        LPLPVOID        lplpvLinearInfo,
        DWORD           dwDevice
    );

    CBSTATUS (*DestroyLinearInfo)
    (
        LPVOID          lpvLinearInfo
    );
} GENERALCALLBACKS_T, FAR *LPGENERALCALLBACKS;


//////////////////////////////////////////////////////////////////////
// Temporary file callbacks
//////////////////////////////////////////////////////////////////////

typedef struct tagTEMPFILECALLBACKS
{
    LPFILECALLBACKS     lpFileCallBacks;

    CBSTATUS (*TempFileCreate)
    (
        LPLPVOID        lplpvFileInfo,
        WORD            wCreateAttributes
    );

    CBSTATUS (*TempFileRemove)
    (
        LPVOID          lpvFileInfo
    );
} TEMPFILECALLBACKS_T, FAR *LPTEMPFILECALLBACKS;


//////////////////////////////////////////////////////////////////////
// Data file callbacks
//////////////////////////////////////////////////////////////////////

typedef struct tagDATAFILECALLBACKS
{
    LPFILECALLBACKS     lpFileCallBacks;

    CBSTATUS (*GetDataFileInfo)
    (
        LPLPVOID        lplpvFileInfo,
        WORD            wDataFileNum
    );

    CBSTATUS (*CloseDataFileInfo)
    (
        LPVOID          lpvFileInfo
    );
} DATAFILECALLBACKS_T, FAR *LPDATAFILECALLBACKS;


//////////////////////////////////////////////////////////////////////
// Progress callbacks
//////////////////////////////////////////////////////////////////////

typedef struct tagPROGRESSCALLBACKS
{
    CBSTATUS (*ProgressInit)
    (
        LPWSTR          lpwstrObjectName,
        DWORD           dwLevel,
        DWORD           dwProgressValueMax,
        LPCOOKIETRAY    lpCookieTray,
        LPLPVOID        lplpvData
    );

    CBSTATUS (*ProgressUpdate)
    (
        LPVOID          lpvData,
        DWORD           dwProgressValue
    );

    CBSTATUS (*ProgressClose)
    (
        LPVOID          lpvData
    );

    CBSTATUS (*RelinquishControl)
    (
        LPCOOKIETRAY    lpCookieTray
    );
} PROGRESSCALLBACKS_T, FAR *LPPROGRESSCALLBACKS;


//////////////////////////////////////////////////////////////////////
// User interface callbacks
//////////////////////////////////////////////////////////////////////

typedef struct tagUICALLBACKS
{
    CBSTATUS (*MessageBox)
    (
        LPWSTR          lpwstrTitle,
        LPWSTR          lpwstrPrompt,
        DWORD           dwStyle,
        LPDWORD         lpdwSelection,
        LPCOOKIETRAY    lpCookieTray
    );

    CBSTATUS (*SelectionPrompt)
    (
        LPWSTR          lpwstrTitle,
        LPWSTR          lpwstrPrompt,
        LPLPWSTR        lplpwstrSelections,
        WORD            wNumSelections,
        LPWORD          lpwSelection,
        LPCOOKIETRAY    lpCookieTray
    );

    CBSTATUS (*GetString)
    (
        LPWSTR          lpwstrTitle,
        LPWSTR          lpwstrPrompt,
        LPWSTR          lpwstrResponse,
        WORD            wResponseLen,
        LPCOOKIETRAY    lpCookieTray
    );

    CBSTATUS (*GetHexDWValue)
    (
        LPWSTR          lpwstrTitle,
        LPWSTR          lpwstrPrompt,
        LPDWORD         lpdwValue,
        LPCOOKIETRAY    lpCookieTray
    );

    CBSTATUS (*GetDecimalDWValue)
    (
        LPWSTR          lpwstrTitle,
        LPWSTR          lpwstrPrompt,
        LPDWORD         lpdwValue,
        LPCOOKIETRAY    lpCookieTray
    );

    CBSTATUS (*OpenInfoBox)
    (
        LPWSTR          lpwstrTitle,
        LPLPVOID        lplpvHandle,
        LPCOOKIETRAY    lpCookieTray
    );

    CBSTATUS (*UpdateInfoBox)
    (
        LPVOID          lpvHandle,
        LPWSTR          lpwstrMessage,
        LPCOOKIETRAY    lpCookieTray
    );

    CBSTATUS (*CloseInfoBox)
    (
        LPVOID          lpvHandle,
        LPCOOKIETRAY    lpCookieTray
    );
} UICALLBACKS_T, FAR *LPUICALLBACKS;


//////////////////////////////////////////////////////////////////////
// Inter-Process Communication (IPC) callback structure
//////////////////////////////////////////////////////////////////////

typedef struct tagIPCCALLBACKS
{
    // Mutex

    CBSTATUS (*MutexCreate)
    (
        LPLPVOID        lplpvMutexInfo
    );

    CBSTATUS (*MutexDestroy)
    (
        LPVOID          lpvMutexInfo
    );

    CBSTATUS (*MutexWait)
    (
        LPVOID          lpvMutexInfo,
        DWORD           dwTimeoutMS
    );

    CBSTATUS (*MutexRelease)
    (
        LPVOID          lpvMutexInfo
    );

    // Semaphore

    CBSTATUS (*SemaphoreCreate)
    (
        LPLPVOID        lplpvSemaphoreInfo,
        DWORD           dwInitialCount,
        DWORD           dwMaximumCount
    );

    CBSTATUS (*SemaphoreDestroy)
    (
        LPVOID          lpvSemaphoreInfo
    );

    CBSTATUS (*SemaphoreWait)
    (
        LPVOID          lpvSemaphoreInfo,
        DWORD           dwTimeoutMS
    );

    CBSTATUS (*SemaphoreRelease)
    (
        LPVOID          lpvSemaphoreInfo,
        LPDWORD         lpdwPrevCount
    );

} IPCCALLBACKS_T, FAR *LPIPCCALLBACKS;


//////////////////////////////////////////////////////////////////////
// Global callback structure
//////////////////////////////////////////////////////////////////////

typedef struct tagGLOBALCALLBACKS
{
    LPGENERALCALLBACKS      lpGeneralCallBacks;
    LPTEMPFILECALLBACKS     lpTempFileCallBacks;
    LPDATAFILECALLBACKS     lpDataFileCallBacks;
    LPPROGRESSCALLBACKS     lpProgressCallBacks;
    LPUICALLBACKS           lpUICallBacks;
    LPIPCCALLBACKS          lpIPCCallBacks;
} GLOBALCALLBACKS_T, FAR *LPGLOBALCALLBACKS;


// #defines to simplify the source code

#define     AVFileRead          lpFileObject->lpCallBacks->FileRead
#define     AVFileSeek          lpFileObject->lpCallBacks->FileSeek
#define     AVFileWrite         lpFileObject->lpCallBacks->FileWrite
#define     AVFileLength        lpFileObject->lpCallBacks->FileLength
#define     AVFileEOF           lpFileObject->lpCallBacks->FileEOF
#define     AVFileTell          lpFileObject->lpCallBacks->FileTell
#define     AVFileSetEOF        lpFileObject->lpCallBacks->FileSetEOF
#define     AVFileGetDateTime   lpFileObject->lpCallBacks->FileGetDateTime
#define     AVFileSetDateTime   lpFileObject->lpCallBacks->FileSetDateTime

#endif  // _AVCB_H


