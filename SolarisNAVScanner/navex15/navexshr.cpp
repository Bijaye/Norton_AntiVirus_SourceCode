//************************************************************************
//
// $Header:   S:/NAVEX/VCS/navexshr.cpv   1.17   15 Dec 1998 12:10:16   DCHI  $
//
// Description:
//      Contains NAVEX code shared among all modules.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/navexshr.cpv  $
// 
//    Rev 1.17   15 Dec 1998 12:10:16   DCHI
// Added GetTempFile() implementation.
// 
//    Rev 1.16   25 Aug 1998 10:54:24   DKESSNE
// added one line to GetEntryPtOffset() to account for the possibility of 
// a negative CS.
// 
//    Rev 1.15   09 Jan 1997 12:07:24   JWILBER
// Removed #ifdefs so GetEntryPtOffset can be used in NLM by Tentatrickle
// detection.
//
//    Rev 1.14   26 Dec 1996 15:21:56   AOONWAL
// No change.
//
//    Rev 1.13   02 Dec 1996 14:01:46   AOONWAL
// No change.
//
//    Rev 1.12   29 Oct 1996 12:58:40   AOONWAL
// No change.
//
//    Rev 1.11   28 Aug 1996 15:56:10   DCHI
// Endianized code.  Moved boot functions to NVXBTSHR.CPP.
//
//    Rev 1.10   26 Jul 1996 17:09:08   RAY
// Added #ifdef SYM_VXD for memset and memcpy
//
//    Rev 1.10   26 Jul 1996 15:05:36   RAY
// Added #ifdef SYM_VXD || SYM_NTK for memset and memcpy.
//
//    Rev 1.10   26 Jul 1996 14:54:54   RAY
//
//    Rev 1.9   26 Jul 1996 12:37:16   DCHI
// Addition of memset and memcpy functions.
//
//    Rev 1.8   25 Jul 1996 18:08:22   RAY
// Added Generic Repair of DMF floppies.
//
//    Rev 1.7   28 May 1996 16:59:48   JWILBER
// Added CR after #endif at EOF to make compiler happy.
//
//    Rev 1.6   28 May 1996 16:55:26   JWILBER
// #ifdef'd out GetEntryPtOffset() function for NLM, since it's not needed.
//
//    Rev 1.5   13 May 1996 18:56:00   JWILBER
// OOPS.  Twiddled #includes to make NAVEX build.
//
//    Rev 1.4   13 May 1996 16:52:00   JWILBER
// Moved GetEntryPtOffset() here from NAVEXRF.CPP.
//
//    Rev 1.3   18 Apr 1996 18:25:24   JWILBER
// Removed #ifdef pair that kept ScanString() out of the NLM.
//
//    Rev 1.2   30 Jan 1996 15:43:36   DCHI
// Added #ifndef SYM_NLM and #endif pairs to ifdef out functions unused
// on NLM platform.
//
//    Rev 1.1   14 Nov 1995 15:00:18   CNACHEN
// Fixed -1 compare bug..
//
//    Rev 1.0   01 Nov 1995 10:54:44   DCHI
// Initial revision.
//
//    Rev 1.2   16 Oct 1995 13:36:48   CNACHEN
// fixed W32 problem with setting wVariable = -1.
//
//    Rev 1.1   16 Oct 1995 10:00:14   JALLEE
// ScanString function added.
//
//    Rev 1.0   13 Oct 1995 13:06:00   DCHI
// Initial revision.
//
//************************************************************************

#include "gdefines.h"

#include "platform.h"

#include "endutils.h"

#ifdef WIN16        // VIRSCAN QUAKE 2
#include "cts4.h"
#else
#include "ctsn.h"
#include "callback.h"
#endif

#include "navexshr.h"

//
// This file should contain the definitions of all functions that
// can be used by more than one module.  The following commented
// out function is an example.
//

/*

DWORD EXTSegOffToLinearAddr(LPVOID segOff)
{
    DWORD linearAddr;

    linearAddr = (segOff & 0xFFFF0000U) >> 12;
    linearAddr += segOffset & 0x0000FFFFU;

    return linearAddr;
}

*/

// ScanString arguments:
//
// lpbyBuffer1          : Buffer in which to search.
// cwBuffer1            : Count of bytes in Buffer1.
//                      : cwBuffer1 must be > cwBuffer2.
// lpbyBuffer2          : String to search for in Buffer1.
// cwBuffer2            : Count of bytes in Buffer2.
//                      : cwBuffer2 ust be >= 2 bytes.
//
// Returns:
//
//     WORD             : -1            no match
//                        n >= 0        offset of buffer2 in buffer1.

WORD ScanString (LPBYTE lpbyBuffer1,
                 WORD cwBuffer1,
                 LPBYTE lpbyBuffer2,
                 WORD cwBuffer2)
{
    WORD cwI, cwK, wFirstWord, wSearchLen, wReturn = (WORD)-1;

    wFirstWord = AVDEREF_WORD(lpbyBuffer2);
    wSearchLen= cwBuffer1-cwBuffer2;

    cwI=0;
    while(cwI <= wSearchLen && wReturn == (WORD)-1)
    {
        if (wFirstWord == AVDEREF_WORD(lpbyBuffer1 +cwI))
        {
            // match first word.
            cwK=2;

            while ((cwK < cwBuffer2) &&
                   (*(lpbyBuffer1 + cwI + cwK) == *(lpbyBuffer2 + cwK)))
            {
                cwK++;
            }
            if (cwK >= cwBuffer2)
            {
                wReturn = cwI;
            }
        }
        cwI++;
    }
    return(wReturn);
}

// GetEntryPtOffset
//
// lpCallBack           : pointer to callback structure for FileSeek, etc.
// hFile                : File Handle of opened file to get entry point
// lpbyBuff             : Buffer to use for reading file header - must hold
//                        at least 0x18 bytes from the start of the file.
// Returns:
//  File offset of program entry point.  This is assumed to be 0 if the
//  program doesn't begin with "MZ" or 0xe9.  DWERROR is returned if
//  something went wrong.

DWORD GetEntryPtOffset (LPBYTE lpbyBuff)
{
    DWORD       dwRes;
    EXEHEADER   FAR *lpEHS;

    if (0xe9 == *lpbyBuff)
        dwRes = (DWORD) AVDEREF_WORD(lpbyBuff + 1) + 3;
    else if (ISEXEHDR(lpbyBuff))
    {
        lpEHS = (EXEHEADER FAR *) lpbyBuff;

        dwRes = (DWORD) WENDIAN(lpEHS->exHeaderSize);    // Header size in paras
        dwRes += (DWORD) WENDIAN(lpEHS->exInitCS);       // Add CS
        dwRes <<= 4;            // Convert paragraph count into byte count
        dwRes += (DWORD) WENDIAN(lpEHS->exInitIP);       // Add IP

        dwRes &= 0xfffff;    //if CS < 0, result will be too large by 0x100000
    }
    else
        dwRes = 0;

    return(dwRes);
}


//********************************************************************
//
// Function:
//  HFILE GetTempFile()
//
// Parameters:
//  lpstCallBack        Ptr to callback structure
//  lpszFileName        Ptr to buffer for temporary filename
//
// Description:
//  For NAVEX 1.0 WIN32 and WIN16, calls GetTempFileName() to get
//  a temporary filename and then opens the file.
//
//  For NAVEX 1.5, calls lpstCallBack->GetTempFile().
//
// Returns:
//  HFILE               On success
//  -1                  On error
//
//********************************************************************

HFILE GetTempFile
(
    LPCALLBACKREV1      lpstCallBack,
    LPTSTR              lpszFileName
)
{
#if defined(NAVEX15)

    return lpstCallBack->GetTempFile(lpszFileName,NULL);

#else

  #if defined(SYM_WIN32)

    TCHAR               szTempPath[MAX_PATH];

    // Get the temporary path

    if (GetTempPath(MAX_PATH,szTempPath) == 0)
        return((HFILE)-1);

    // Get a temporary filename

    if (GetTempFileName(szTempPath,"TMP",0,lpszFileName) == 0)
        return((HFILE)-1);

  #elif defined(SYM_WIN16)

    extern BOOL         gbInWindows;

    // If this is the DOS TSR, fail

    if (gbInWindows == FALSE)
        return((HFILE)-1);

    // Get a temporary filename

    if (GetTempFileName(0,"TMP",0,lpszFileName) == 0)
        return((HFILE)-1);

  #else

    return((HFILE)-1);

  #endif

    // Open the file

    return lpstCallBack->FileOpen(lpszFileName,2);
#endif // #if defined(NAVEX15)
}



