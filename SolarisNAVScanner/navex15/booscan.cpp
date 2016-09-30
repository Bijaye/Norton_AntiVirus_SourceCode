// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/booscan.cpv   1.10   05 Aug 1998 18:48:16   hjaland  $
//
// Description:
//
// Contains: Bloodhound boot record scanning
//
// See Also:
//************************************************************************
// $Log:   S:/NAVEX/VCS/booscan.cpv  $
// 
//    Rev 1.10   05 Aug 1998 18:48:16   hjaland
// Carey added lpbExcluded in BloodhoundFindBootVirus to disable IBM STR scanner
// 
//    Rev 1.9   18 Mar 1998 17:06:12   CFORMUL
// Malloc'd a 2K buffer to replace the one that was declared on the stack
// 
//    Rev 1.8   28 Jan 1998 17:57:14   CNACHEN
// Added code to zero out BPB and partition table in boot records
// before scanning them to eliminate the chances of FPing on random
// data in these areas.
// 
//    Rev 1.7   05 Dec 1997 16:33:20   CNACHEN
// Removed code to check for STOPBH.DAT, #ifdefed code out for writing out
// navboofp.dat for non win16/win32 platforms.
// 
//    Rev 1.6   02 Dec 1997 16:38:54   CNACHEN
// 
//    Rev 1.0   02 Dec 1997 16:36:50   CNACHEN
// Initial revision.
// 
//    Rev 1.5   02 Dec 1997 11:49:26   CNACHEN
// Added creation of NAVBOOFP.DAT file in C:\ if the file C:\MAKEBOO.DAT is
// found, when a bloodhound detection occurs.
// 
//    Rev 1.4   10 Nov 1997 16:41:50   CNACHEN
// Removed bloodhound ifdefs
// 
//    Rev 1.3   15 Sep 1997 11:47:36   CNACHEN
// Added check for USE_BLOOD_BOOT ifdef
// 
// 
//    Rev 1.2   09 Sep 1997 15:56:30   CNACHEN
// Added support for creation of c:\\navboofp.dat file if a bloodhound.boot
// virus is found, and the build is done with the /DBOOT_BETA switch.
// 
//    Rev 1.1   09 Sep 1997 10:46:08   CNACHEN
// Changed boot parser to output BYTE sigs instead of work sigs.
// 
// 
//    Rev 1.0   05 Sep 1997 20:24:52   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef SYM_NLM

#ifdef VC
#include <windows.h>
#include <stdio.h>
#else
#include "platform.h"
#endif

#define BOO_DEFINE_DATA     // make sure to define virus defs data structure

#include "bootype.h"
#include "boodata.h"
#include "boodefs.h"
#include "callback.h"

// this function applies a single boot signature specified by nSigIndex
// to the 512 byte sector passed in.  It returns a single score to the
// calling function which is based on the type (nType) of the boot record:
// FBR, PBR or MBR.  If a given signature matches, it can specify 3 different
// heuristic values for each of the 3 types of boot records.  This allows us
// to detect viruses based on different criteria in different types of boot
// records (e.g. floppy should not read the HD, but MBR may do so, etc.)
// negative scores are allowed to "exclude" known good boot records.

void ApplyBootSig
(
    int                         nSigIndex,
    LPBYTE                      lpbyBuffer,
    int                         nType,
    LPINT                       lpnScore
)
{
    LPSIG_TYPE                  lpSig = gastSigs+nSigIndex;
    WORD                        wSigIndex = 0, wDataIndex = 0, wDist, wTopLevelIndex;
    WORD                        wFetchWord;
    BYTE                        byMask, byFindMe;
    int                         nFBR, nPBR, nMBR;

	// assumption is that all boot sigs begin with Scan(#512, 0x??)

    for (wTopLevelIndex = 0;wTopLevelIndex < BYTES_PER_SECTOR;wTopLevelIndex++)
	{
        if ((BYTE)lpSig->lpbyData[0] == lpbyBuffer[wTopLevelIndex])
        {
			wDataIndex = wTopLevelIndex;
			wSigIndex = 0;

		    wSigIndex++;
            wDataIndex++;

            while (wDataIndex < BYTES_PER_SECTOR)
            {
                switch (lpSig->lpbyData[wSigIndex])
                {
                    // scan N bytes for a masked byte
                    // encoding: [WORD=distance] [WORD=search byte] [WORD=search mask]

                    case BOOT_FUNC_SCAN:
                        wDist = lpSig->lpbyData[++wSigIndex];
                        if (wDist + wDataIndex > BYTES_PER_SECTOR)
                        {
                            wDist = BYTES_PER_SECTOR - wDataIndex;
                        }

                        byFindMe = (BYTE)lpSig->lpbyData[++wSigIndex];
                        byMask = (BYTE)lpSig->lpbyData[++wSigIndex];
                        wSigIndex++;

                        while (wDist > 0)
                        {
                            if ((lpbyBuffer[wDataIndex] & byMask) == byFindMe)
                            {
                                wDataIndex++;   // advance past match
                                break;
                            }

                            wDataIndex++;
                            wDist--;
                        }
                        if (wDist == 0)
                        {
                            wDataIndex = BYTES_PER_SECTOR;
                        }

                        break;

                    case BOOT_FUNC_MATCH:

                        // match a single byte at the current index
                        // encoding: [WORD=byte to match] [WORD=mask]

                        byFindMe = (BYTE)lpSig->lpbyData[++wSigIndex];
                        byMask = (BYTE)lpSig->lpbyData[++wSigIndex];
                        wSigIndex++;

                        if ((lpbyBuffer[wDataIndex] & byMask) == byFindMe)
                        {
                            wDataIndex++;
                        }
                        else
                        {
                            wDataIndex = BYTES_PER_SECTOR;
                        }
                        break;
                    case BOOT_FUNC_BINGO:
                        // report detection of signature
                        // encoding: [WORD=FBR score] [WORD = PBR score] [WORD = MBR score]

#ifdef VC
						//printf("Bingo: %s\n",gastSigs[nSigIndex].lpszDescriptor);
#endif

                        wFetchWord = lpSig->lpbyData[++wSigIndex];
                        wFetchWord |= (WORD)(lpSig->lpbyData[++wSigIndex] << 8);
                        nFBR = (int)(short int)wFetchWord;
                        wFetchWord = lpSig->lpbyData[++wSigIndex];
                        wFetchWord |= (WORD)(lpSig->lpbyData[++wSigIndex] << 8);
                        nPBR = (int)(short int)wFetchWord;
                        wFetchWord = lpSig->lpbyData[++wSigIndex];
                        wFetchWord |= (WORD)(lpSig->lpbyData[++wSigIndex] << 8);
                        nMBR = (int)(short int)wFetchWord;

                        switch (nType)
                        {
                            case BOO_TYPE_FLOPPY:
                                *lpnScore += nFBR;
                                break;
                            case BOO_TYPE_PBR:
                                *lpnScore += nPBR;
                                break;
                            case BOO_TYPE_MBR:
                                *lpnScore += nMBR;
                                break;
                        }

                        return;
                }
            }
        }
    }

    return;
}

void DumpBootRecord
(
    LPCALLBACKREV1          lpstCallBack,
    LPBYTE                  lpbyBuffer
)
{
#if defined (SYM_WIN16) || defined (SYM_WIN32)
    HFILE                   h;

    h = lpstCallBack->FileOpen(_T("C:\\MAKEBOO.DAT"),0);
    if (h == (HFILE)-1)
    {
        return;
    }

    lpstCallBack->FileClose(h);

    h = lpstCallBack->FileCreate(_T("C:\\NAVBOOFP.DAT"),0);
    if (h != (HFILE)-1)
    {
        lpstCallBack->FileWrite(h,lpbyBuffer,512);
        lpstCallBack->FileClose(h);
    }
#else
    (void)lpstCallBack;
    (void)lpbyBuffer;
#endif // #if defined (SYM_WIN16) || defined (SYM_WIN32)
}

// this function applies all bloodhound boot signatures to a given boot record
// buffer

int ScanBooBuffer
(
    LPBYTE                      lpbyBuffer,
    int                         nType 
)
{
    int                         i;
    int                         nScore;

    nScore = 0;

    for (i=0;i<NUM_BOO_SIGS;i++)
    {
        ApplyBootSig(i,lpbyBuffer,nType,&nScore);
    }

    return(nScore);
}

void StripVariantData
(
    LPBYTE                      lpbyBuffer,
    int                         nType
)
{
    int i;

    switch (nType)
    {
        case BOO_TYPE_FLOPPY:
        case BOO_TYPE_PBR:

            // PBR and FBR have variant data between 0x0b and 0x1d in the
            // boot record

            for (i=0x0b;i<0x1d;i++)
                lpbyBuffer[i] = 0;
            break;

        case BOO_TYPE_MBR:

            for (i=0x1be;i<0x1fe;i++)
                lpbyBuffer[i] = 0;
            break;
    }
}

#ifdef VC
DWORD gdwTotalScore;
DWORD gdwNumScanned;

int gnHist[3][512];
int gnFBR, gnMBR, gnPBR;
#endif

BOOL BloodhoundFindBootVirus
(
    LPCALLBACKREV1              lpCallBack,
    LPBYTE                      lpbyBuffer,
    LPBOOL                      lpbExcluded
)
{
    int                         nScore, nType, i;
//    BYTE                        byBuffer[512];
    LPBYTE                      lpbyBuf;

    *lpbExcluded = FALSE;

#ifndef SYM_WIN32
    lpbyBuf = (LPBYTE) lpCallBack->PermMemoryAlloc (512);
#else
    lpbyBuf = (LPBYTE) GlobalAlloc (GMEM_FIXED, 512);
#endif    

    // go for it!

    nType = DetermineBootType(lpbyBuffer);

    // make a backup copy of the boot buffer.

    for (i=0;i<512;i++)
        lpbyBuf[i] = lpbyBuffer[i];

    // strip the BPB or partition table from it

    StripVariantData(lpbyBuf,nType);

    // and scan it!

    nScore = ScanBooBuffer(lpbyBuf,nType);

    if (nScore < 0)
    {
        *lpbExcluded = TRUE;        // clean boot record! (partial overwrite)
    }

#ifndef SYM_WIN32
    lpCallBack->PermMemoryFree ((LPVOID) lpbyBuf);
#else
    GlobalFree ((LPVOID) lpbyBuf);
#endif

#ifdef VC
	gdwTotalScore +=nScore;
	gdwNumScanned ++;

	printf("Score: %d\n",nScore);

	if (nScore < 0)
    {
		nScore = 0;
    }

	gnHist[nType][nScore]++;
#endif

    switch (nType)
    {
        case BOO_TYPE_FLOPPY:
#ifdef VC
			gnFBR++;
			printf("floppy\n");
#endif
            if (nScore > FBR_THRESHOLD)
            {
                DumpBootRecord(lpCallBack, lpbyBuffer);
                return(TRUE);
            }

            return(FALSE);
        case BOO_TYPE_PBR:
#ifdef VC
			gnPBR++;
			printf("PBR\n");
#endif
            if (nScore > PBR_THRESHOLD)
            {
                DumpBootRecord(lpCallBack, lpbyBuffer);
                return(TRUE);
            }

            return(FALSE);
        case BOO_TYPE_MBR:
#ifdef VC
			gnMBR++;
			printf("MBR\n");
#endif
            if (nScore > MBR_THRESHOLD)
            {
                DumpBootRecord(lpCallBack, lpbyBuffer);
                return(TRUE);
            }

            return(FALSE);
    }

    return(FALSE);
}

#endif // #ifdef SYM_NLM
