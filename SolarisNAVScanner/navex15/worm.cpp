//************************************************************************
//
// $Header:   S:/NAVEX/VCS/worm.cpv   1.3   26 Dec 1996 15:21:54   AOONWAL  $
//
// Description:
//      Contains AVEX DLL entry code for WORM.
//
// Notes:
//      Should only be part of Windows compilations.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/worm.cpv  $
// 
//    Rev 1.3   26 Dec 1996 15:21:54   AOONWAL
// No change.
// 
//    Rev 1.2   02 Dec 1996 14:01:26   AOONWAL
// No change.
// 
//    Rev 1.1   29 Oct 1996 12:58:22   AOONWAL
// No change.
// 
//    Rev 1.0   25 Oct 1995 13:19:34   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include "callback.h"
#include "ctsn.h"
#include "navex.h"

HINSTANCE hInstance;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int WINAPI LibMain (
    HINSTANCE hInst,
    WORD wDataSeg,
    WORD wHeapSize,
    LPSTR lpszCmdLine );

int WINAPI WEP (
    int nParam );

// ---------------------------------------------------------------------------

#if defined(SYM_WIN32)

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL WINAPI DllMain (
    HINSTANCE   hInstDLL,
    DWORD       dwReason,
    LPVOID      lpvReserved )
{
    BOOL        bResult = TRUE;

    switch (dwReason)
        {
        case DLL_PROCESS_ATTACH:
            bResult = LibMain(hInstDLL, 0, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            WEP(0);
            break;
        }   // switch

    return (bResult);
}   //  DllMain

#endif

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int WINAPI LibMain (HINSTANCE hInst, WORD wDataSeg, WORD wHeapSize,
                        LPSTR lpszCmdLine)
{
   hInstance = hInst;

#ifdef SYM_WIN16

   if (wHeapSize > 0)
      UnlockData (0);

#endif

   return (1);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int WINAPI WEP(int nParam)
{
   return(1);  // Return success to windows //
}



// EXTRepairBoot arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// wVersionNumber       : NAV version number.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the boot sec 0=A, 0x80=C
// byPartitionNum       : if != FFh, this is the partition # of the boot sector
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_DISK_READ_ERROR     if disk read error
//                        EXTSTATUS_DISK_WRITE_ERROR    if disk write error
//                        EXTSTATUS_MEM_ERROR           if memory error
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

EXTSTATUS FAR WINAPI NLOADDS EXTRepairBoot(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           LPN30          lpsN30,
                                           BYTE           byDrive,
                                           BYTE           byPartitionNum,
                                           WORD           wSector,
                                           WORD           wCylinder,
                                           WORD           wHead,
                                           LPBYTE         lpbyWorkBuffer)
{

    return ( EXTSTATUS_NO_REPAIR );
}





// EXTRepairPart arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// wVersionNumber       : NAV version number.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the partition sector
//                        (0x80=first physical HD
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_DISK_READ_ERROR     if disk read error
//                        EXTSTATUS_DISK_WRITE_ERROR    if disk write error
//                        EXTSTATUS_MEM_ERROR           if memory error
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

EXTSTATUS FAR WINAPI NLOADDS EXTRepairPart(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           LPN30          lpsN30,
                                           BYTE           byDrive,
                                           LPBYTE         lpbyWorkBuffer)
{
    return ( EXTSTATUS_NO_REPAIR );
}


// EXTScanBoot arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot sectors
//                        on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_READ_ERROR          if disk error occured
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//                        EXTSTATUS_MEM_ERROR           if memory alloc. error
//

EXTSTATUS FAR WINAPI NLOADDS EXTScanBoot(LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)
{
    return ( EXTSTATUS_OK );
}



// EXTScanFile arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpszFileName         : pointer to the name of the file
// hFile                : file handle into current file to scan
// lpbyInfectionBuffer  : 4K fstart buffers containing TOF, ENTRY, etc.
//                         +0   256 fstart bytes of target program.
//                         +256 64 bytes from TOF of target program.
//                         +512 256 unused
//                         +768 2 Number of external defs called.
//                         +770 2 ID of first external called.
//                         +772 ? ID of additional externals called.
// lpbyWorkBuffer       : 2-4K buffer for temporary operations (STACK OK also)
// wVersionNumber       : Engine version number so detections are not performed
//                        by an external DLL if the engine has been updated
//                        to provide the detection itself.
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_FILE_ERROR          if file error occured
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//                        EXTSTATUS_MEM_ERROR           if memory alloc. error

EXTSTATUS FAR WINAPI NLOADDS EXTScanFile(LPCALLBACKREV1 lpCallBack,
                                         LPSTR          lpszFileName,
                                         HFILE          hFile,
                                         LPBYTE         lpbyInfectionBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)
{

    return ( EXTSTATUS_OK );
}


// EXTRepairFile arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// wVersionNumber       : version of NAV calling external code
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_FILE_ERROR  if file error
//                        EXTSTATUS_MEM_ERROR   if memory error
//                        EXTSTATUS_NO_REPAIR   if unable to repair

EXTSTATUS FAR WINAPI NLOADDS EXTRepairFile(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           LPN30          lpsN30,
                                           LPSTR          lpszFileName,
                                           LPBYTE         lpbyWorkBuffer)
{
    return ( EXTSTATUS_NO_REPAIR );
}


// EXTScanMemory arguments:
//
// lpCallBack           : pointer to callback structure
// wVersionNumber       : NAV version which is using external code
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//                        EXTSTATUS_MEM_ERROR           if memory error
//

EXTSTATUS FAR WINAPI NLOADDS EXTScanMemory(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           BOOL           bScanHighMemory,
                                           LPWORD         lpwVID)
{
    return ( EXTSTATUS_OK );
}



// EXTScanPart arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : drive # containing the boot sector (00=A, 0x80=first
//                        physical HD
// lpbyPartBuffer       : contains image of appropriate part sector
//                        to scan (ignore byDrive)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV calling external code
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_READ_ERROR          if disk error occured
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//                        EXTSTATUS_MEM_ERROR           if memory alloc. error
//

EXTSTATUS FAR WINAPI NLOADDS EXTScanPart(LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         LPBYTE         lpbyPartBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)
{
    return ( EXTSTATUS_OK );
}




