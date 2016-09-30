//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/NAVEXSHR.CPv   1.0   15 May 1997 18:22:50   jsulton  $
//
// Description:
//      Contains NAVEX code shared among all modules.
//
//************************************************************************
// $Log:   S:/ENGINE15/VCS/NAVEXSHR.CPv  $
// 
//    Rev 1.0   15 May 1997 18:22:50   jsulton
// Initial revision.
//************************************************************************

#include "platform.h"

#include "endutils.h"

#ifdef WIN16        // VIRSCAN QUAKE 2
#include "cts4.h"
#else
//#include "ctsn.h"		// NEW_UNIX
#include "n30type.h"		// NEW_UNIX
#include "callback.h"
#endif

#include "navexshr.h"

//
// This file should contain the definitions of all functions that
// can be used by more than one module.  The following commented
// out function is an example.
//


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
    }
    else
        dwRes = 0;

    return(dwRes);
}

