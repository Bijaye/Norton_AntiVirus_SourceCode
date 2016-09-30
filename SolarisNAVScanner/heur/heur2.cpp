// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/HEUR/VCS/HEUR2.CPv   1.7   14 Oct 1998 11:57:44   MKEATIN  $
//
// Description:
//
//  Contains code for the execution monitor.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/HEUR/VCS/HEUR2.CPv  $
// 
//    Rev 1.7   14 Oct 1998 11:57:44   MKEATIN
// Carey's RelinquishControl calls for NLM.
// 
//    Rev 1.6   27 Jan 1998 17:05:28   DDREW
// Changed the relinquishcontrol values
// 
//    Rev 1.5   03 Nov 1997 17:11:34   CNACHEN
// Updated to support Win16
// 
//    Rev 1.4   13 Oct 1997 13:22:38   CNACHEN
// Added more relinquish control for NLM.
// 
// 
//    Rev 1.3   01 Jul 1997 19:22:30   CNACHEN
// Fixed some exe region bugs (now uses region from CS:0 to EOImage, where
// image is min of exe header/executable actual size).
// 
//    Rev 1.2   27 May 1997 14:37:24   CNACHEN
// 
//    Rev 1.1   27 May 1997 14:20:04   CNACHEN
// Fixed some callback covers and conversions for DX.
// 
//    Rev 1.0   14 May 1997 17:23:12   CNACHEN
// Initial revision.
// 
//************************************************************************

#include "heurapi.h"

#ifdef SYM_WIN16
#define memset _fmemset
#endif


EXEC_STATUS EMInit
(
    LPPAMCONFIG                 lpConfig,
    LPEXEC_MANAGER              lpEM
)
{
    // set our virus start/end

    lpEM->m_wSeg = 0;
    lpEM->m_wLowerOff = 0;
    lpEM->m_wUpperOff = 0;
    lpEM->m_dwLinearLower = 0;
    lpEM->m_dwLinearUpper = 0;
    lpEM->m_dwLinearBase = 0;
    lpEM->m_bRangeFinalStatus = FALSE;    // have not set final range yet...

    // set our exploration mode

    lpEM->m_bExplore = FALSE;         // standard emulation

    // zero our revector bits

    lpEM->m_byRevecBits = 0;

    // reset our file type

    lpEM->m_nFileType = EXEC_FILE_TYPE_UNKNOWN;

    // null out our head and tail pointers

    lpEM->m_lpstHead = lpEM->m_lpstTail = NULL;

    // zero out the byte status map

    memset(lpEM->m_byExecSegmentMap,0,SEGMENT_MAP_SIZE);
    memset(lpEM->m_byWriteSegmentMap,0,SEGMENT_MAP_SIZE);
    memset(lpEM->m_byQueueMap,0,SEGMENT_MAP_SIZE);

    // remember our config info

    lpEM->m_stConfig = *lpConfig;

    return(EXEC_STATUS_OK);
}


EXEC_STATUS EMReset
(
    LPEXEC_MANAGER              lpEM
)
{
    LPCPU_CACHE_STATE         lpstState, lpstNext;

    // set our virus start/end

    lpEM->m_wSeg = 0;
    lpEM->m_wLowerOff = 0;
    lpEM->m_wUpperOff = 0;
    lpEM->m_dwLinearLower = 0;
    lpEM->m_dwLinearUpper = 0;
    lpEM->m_dwLinearBase = 0;
    lpEM->m_bRangeFinalStatus = FALSE;    // have not set final range yet...

    // set our exploration mode

    lpEM->m_bExplore = FALSE;         // standard emulation

    // zero our revector bits

    lpEM->m_byRevecBits = 0;

    // reset our file type

    lpEM->m_nFileType = EXEC_FILE_TYPE_UNKNOWN;

    // free any existing entries in the execution queue

    lpstState = lpEM->m_lpstTail;

    while (lpstState != NULL)
    {
        lpstNext = lpstState->lpstNext;
        PAMMemoryFree(lpstState);
        lpstState = lpstNext;
    }

    // null out our head and tail pointers

    lpEM->m_lpstHead = lpEM->m_lpstTail = NULL;

    // zero out the byte status map

    memset(lpEM->m_byExecSegmentMap,0,SEGMENT_MAP_SIZE);
    memset(lpEM->m_byWriteSegmentMap,0,SEGMENT_MAP_SIZE);
    memset(lpEM->m_byQueueMap,0,SEGMENT_MAP_SIZE);

    return(EXEC_STATUS_OK);
}


EXEC_STATUS EMSetBaseSeg
(
    LPEXEC_MANAGER              lpEM,
    WORD                        wSeg,
    WORD                        dwSegLen
)
{
    lpEM->m_wSeg = wSeg;
    lpEM->m_dwLinearBase = (DWORD)lpEM->m_wSeg * 0x10;
    lpEM->m_dwSegLen = dwSegLen;

    return(EXEC_STATUS_OK);
}


EXEC_STATUS EMUpdateRangeByCSIP
(
    LPEXEC_MANAGER              lpEM,
    WORD                        wSeg,
    WORD                        wCurIP,         // sandwich around this IP
    BOOL                        bFinal
)
{
    DWORD               dwSegOff, dwStart, dwEnd;

    dwSegOff = ((DWORD)wSeg * 0x10 + (DWORD)wCurIP) & EFFECTIVE_ADDRESS_MASK;
	
    if (dwSegOff < lpEM->m_dwLinearBase || dwSegOff >= lpEM->m_dwLinearBase + lpEM->m_dwSegLen)
		return(EXEC_STATUS_ERROR);


		
    dwSegOff -= lpEM->m_dwLinearBase;

    if (lpEM->m_nFileType == EXEC_FILE_TYPE_COM)
    {
        // COM case: sandwitch current CS:IP

		if (dwSegOff < (DWORD)(lpEM->m_stConfig.wAssumedVirusSize / 2))
            dwStart = 0;
        else
            dwStart = dwSegOff - (lpEM->m_stConfig.wAssumedVirusSize / 2);

        if (dwSegOff + (lpEM->m_stConfig.wAssumedVirusSize / 2) > lpEM->m_dwSegLen)
            dwEnd = lpEM->m_dwSegLen;
        else
            dwEnd = dwSegOff + (lpEM->m_stConfig.wAssumedVirusSize / 2);

        if (dwEnd - dwStart < lpEM->m_stConfig.wAssumedVirusSize )
        {
            if (dwStart == 0)
            {
                if (lpEM->m_dwSegLen < lpEM->m_stConfig.wAssumedVirusSize )
                    dwEnd = lpEM->m_dwSegLen;
                else
                    dwEnd = lpEM->m_stConfig.wAssumedVirusSize ;
            }

            if (dwEnd == lpEM->m_dwSegLen)
            {
                if (lpEM->m_dwSegLen < lpEM->m_stConfig.wAssumedVirusSize )
                    dwStart = 0;
                else
                    dwStart = dwEnd - lpEM->m_stConfig.wAssumedVirusSize ;
            }
        }

        if (dwStart < 0x100)
            dwStart = 0x100;

        if (dwEnd < 0x100)
            dwEnd = 0x100;
    }
    else
    {
        // EXE case: go from CS:IP to EOF

        dwStart = (DWORD)wSeg * 0x10 - lpEM->m_dwLinearBase;

        if (dwStart + lpEM->m_stConfig.wAssumedVirusSize > lpEM->m_dwSegLen)
            dwEnd = lpEM->m_dwSegLen;
        else
            dwEnd = dwStart + lpEM->m_stConfig.wAssumedVirusSize ;
    }

	if (dwStart >= dwEnd)
	{
		return(EXEC_STATUS_ERROR);		
	}

    if (dwStart < 0x10000)
    {
        lpEM->m_wLowerOff = (WORD)dwStart;
        lpEM->m_dwLinearLower = dwStart + lpEM->m_dwLinearBase;
    }
    else
        return(EXEC_STATUS_ERROR);

    if (dwEnd < 0x10000)
    {
        lpEM->m_wUpperOff = (WORD)dwEnd;
        lpEM->m_dwLinearUpper = dwEnd + lpEM->m_dwLinearBase;
    }
    else
        return(EXEC_STATUS_ERROR);

    lpEM->m_bRangeFinalStatus = bFinal;


    return(EXEC_STATUS_OK);
}

EXEC_STATUS EMUpdateRange
(
    LPEXEC_MANAGER              lpEM,
    WORD                        wSeg,
    WORD                        wStart,
    WORD                        wEnd,
    BOOL                        bFinal
)
{
    DWORD               dwSegOff, dwStart, dwEnd;

    dwSegOff = ((DWORD)wSeg * 0x10 + (DWORD)wStart) & EFFECTIVE_ADDRESS_MASK;
    dwSegOff -= lpEM->m_dwLinearBase;

    dwStart = dwSegOff;

    dwSegOff = ((DWORD)wSeg * 0x10 + (DWORD)wEnd) & EFFECTIVE_ADDRESS_MASK;
    dwSegOff -= lpEM->m_dwLinearBase;

    dwEnd = dwSegOff;

    if (dwStart < 0x10000)
    {
        lpEM->m_wLowerOff = (WORD)dwStart;
        lpEM->m_dwLinearLower = dwStart + lpEM->m_dwLinearBase;
    }
    else
        return(EXEC_STATUS_ERROR);

    if (dwEnd < 0x10000)
    {
        lpEM->m_wUpperOff = (WORD)dwEnd;
        lpEM->m_dwLinearUpper = dwEnd + lpEM->m_dwLinearBase;
    }
    else
        return(EXEC_STATUS_ERROR);

    lpEM->m_bRangeFinalStatus = bFinal;

    return(EXEC_STATUS_OK);
}



BOOL EMInRange
(
    LPEXEC_MANAGER              lpEM,
    WORD                        wSeg,
    WORD                        wOff
)
{
    DWORD dwOffset;

    if (lpEM->m_bExplore == FALSE)
        return(TRUE);           // allow all execution

    dwOffset = (((DWORD)wSeg * 0x10) + (DWORD) wOff) & EFFECTIVE_ADDRESS_MASK;

    if (dwOffset >= lpEM->m_dwLinearLower && dwOffset <= lpEM->m_dwLinearUpper)
        return(TRUE);

    return(FALSE);
}


void EMSetFileType
(
    LPEXEC_MANAGER              lpEM,
    int                         nFileType
)
{
    // set our host file type

    lpEM->m_nFileType = nFileType;
}

int EMGetFileType
(
    LPEXEC_MANAGER              lpEM
)
{
    // return our host file type

    return(lpEM->m_nFileType);
}

EXEC_STATUS EMRecordInt21Mod
(
    LPEXEC_MANAGER              lpEM,
    DWORD                       dwOff,
    LPBOOL                      lpbRevec
)
{
    if (dwOff == 0x84)
        lpEM->m_byRevecBits |= 1;
    else if (dwOff == 0x85)
        lpEM->m_byRevecBits |= 2;
    else if (dwOff == 0x86)
        lpEM->m_byRevecBits |= 4;
    else if (dwOff == 0x87)
        lpEM->m_byRevecBits |= 8;

    if (lpEM->m_byRevecBits == 0xF)
    {
        lpEM->m_byRevecBits = 0;

        *lpbRevec = TRUE;
    }
    else
        *lpbRevec = FALSE;

    return(EXEC_STATUS_OK);
}

void EMSetByteExecStatus
(
    LPEXEC_MANAGER              lpEM,
    WORD                        wSeg,
    WORD                        wOff,
    BOOL                        bStatus
)
{
    DWORD               dwLinearOffset, dwByte, dwBit;

    dwLinearOffset = ((DWORD) wSeg * 0x10 + (DWORD)wOff) &
                     EFFECTIVE_ADDRESS_MASK;

    if (dwLinearOffset >= lpEM->m_dwLinearLower &&
        dwLinearOffset <= lpEM->m_dwLinearUpper)
    {
        dwLinearOffset -= lpEM->m_dwLinearBase;

        dwByte = dwLinearOffset / 8;
        dwBit = dwLinearOffset % 8;

        if (bStatus == TRUE)
            lpEM->m_byExecSegmentMap[(WORD)dwByte] |= (BYTE)(1 << dwBit);
        else
            lpEM->m_byExecSegmentMap[(WORD)dwByte] &= ~(BYTE)(1 << dwBit);
    }
}


EXEC_STATUS EMTSByteExecStatus
(
    LPEXEC_MANAGER              lpEM,
    WORD                        wSeg,
    WORD                        wOff,
    LPBOOL                      lpbExec
)
{
    DWORD               dwLinearOffset, dwByte, dwBit;

    *lpbExec = FALSE;


	dwLinearOffset = ((DWORD) wSeg * 0x10 + (DWORD)wOff) &
					 EFFECTIVE_ADDRESS_MASK;

    if (dwLinearOffset >= lpEM->m_dwLinearLower &&
        dwLinearOffset <= lpEM->m_dwLinearUpper)
    {
        dwLinearOffset -= lpEM->m_dwLinearBase;

        dwByte = dwLinearOffset / 8;
        dwBit = dwLinearOffset % 8;

        if (lpEM->m_byExecSegmentMap[(WORD)dwByte] & (1 << dwBit))
            *lpbExec = TRUE;
        else
            *lpbExec = FALSE;

        lpEM->m_byExecSegmentMap[(WORD)dwByte] |= (BYTE)(1 << dwBit);
    }

    return(EXEC_STATUS_OK);
}

EXEC_STATUS EMGetByteExecStatus
(
    LPEXEC_MANAGER              lpEM,
    WORD                        wSeg,
    WORD                        wOff,
    LPBOOL                      lpbExec
)
{
    DWORD               dwLinearOffset, dwByte, dwBit;

    *lpbExec = FALSE;

    if (lpEM->m_bExplore == TRUE)
        return(EXEC_STATUS_OK);

	dwLinearOffset = ((DWORD) wSeg * 0x10 + (DWORD)wOff) &
                     EFFECTIVE_ADDRESS_MASK;

    if (dwLinearOffset >= lpEM->m_dwLinearLower &&
        dwLinearOffset <= lpEM->m_dwLinearUpper)
    {
        dwLinearOffset -= lpEM->m_dwLinearBase;

        dwByte = dwLinearOffset / 8;
        dwBit = dwLinearOffset % 8;

        if (lpEM->m_byExecSegmentMap[(WORD)dwByte] & (1 << dwBit))
            *lpbExec = TRUE;
        else
            *lpbExec = FALSE;
    }

    return(EXEC_STATUS_OK);
}

void EMSetByteWriteStatus
(
    LPEXEC_MANAGER              lpEM,
    WORD                        wSeg,
    WORD                        wOff,
    BOOL                        bStatus
)
{
    DWORD               dwLinearOffset, dwByte, dwBit;

    dwLinearOffset = ((DWORD) wSeg * 0x10 + (DWORD)wOff) &
                     EFFECTIVE_ADDRESS_MASK;

    if (dwLinearOffset >= lpEM->m_dwLinearLower &&
        dwLinearOffset <= lpEM->m_dwLinearUpper)
    {
        dwLinearOffset -= lpEM->m_dwLinearBase;

        dwByte = dwLinearOffset / 8;
        dwBit = dwLinearOffset % 8;

        if (bStatus == TRUE)
            lpEM->m_byWriteSegmentMap[(WORD)dwByte] |= (BYTE)(1 << dwBit);
        else
            lpEM->m_byWriteSegmentMap[(WORD)dwByte] &= ~(BYTE)(1 << dwBit);
    }
}


EXEC_STATUS EMTSByteWriteStatus
(
    LPEXEC_MANAGER              lpEM,
    WORD                        wSeg,
    WORD                        wOff,
    LPBOOL                      lpbWrite
)
{
    DWORD               dwLinearOffset, dwByte, dwBit;

    *lpbWrite = FALSE;


	dwLinearOffset = ((DWORD) wSeg * 0x10 + (DWORD)wOff) &
					 EFFECTIVE_ADDRESS_MASK;

    if (dwLinearOffset >= lpEM->m_dwLinearLower &&
        dwLinearOffset <= lpEM->m_dwLinearUpper)
    {
        dwLinearOffset -= lpEM->m_dwLinearBase;

        dwByte = dwLinearOffset / 8;
        dwBit = dwLinearOffset % 8;

        if (lpEM->m_byWriteSegmentMap[(WORD)dwByte] & (1 << dwBit))
            *lpbWrite = TRUE;
        else
            *lpbWrite = FALSE;

        lpEM->m_byWriteSegmentMap[(WORD)dwByte] |= (BYTE)(1 << dwBit);
    }

    return(EXEC_STATUS_OK);
}

EXEC_STATUS EMGetByteWriteStatus
(
    LPEXEC_MANAGER              lpEM,
    WORD                        wSeg,
    WORD                        wOff,
    LPBOOL                      lpbWrite
)
{
    DWORD               dwLinearOffset, dwByte, dwBit;

    *lpbWrite = FALSE;

	/*
    if (lpEM->m_bExplore == TRUE)
        return(EXEC_STATUS_OK);
	*/

	dwLinearOffset = ((DWORD) wSeg * 0x10 + (DWORD)wOff) &
                     EFFECTIVE_ADDRESS_MASK;

    if (dwLinearOffset >= lpEM->m_dwLinearLower &&
        dwLinearOffset <= lpEM->m_dwLinearUpper)
    {
        dwLinearOffset -= lpEM->m_dwLinearBase;

        dwByte = dwLinearOffset / 8;
        dwBit = dwLinearOffset % 8;

        if (lpEM->m_byWriteSegmentMap[(WORD)dwByte] & (1 << dwBit))
            *lpbWrite = TRUE;
        else
            *lpbWrite = FALSE;
    }

    return(EXEC_STATUS_OK);
}


void EMSetByteQueueStatus
(
    LPEXEC_MANAGER              lpEM,
    WORD                        wSeg,
    WORD                        wOff,
    BOOL                        bStatus
)
{
    DWORD               dwLinearOffset, dwByte, dwBit;

    dwLinearOffset = ((DWORD) wSeg * 0x10 + (DWORD)wOff) &
                     EFFECTIVE_ADDRESS_MASK;

    if (dwLinearOffset >= lpEM->m_dwLinearLower &&
        dwLinearOffset <= lpEM->m_dwLinearUpper)
    {
        dwLinearOffset -= lpEM->m_dwLinearBase;

        dwByte = dwLinearOffset / 8;
        dwBit = dwLinearOffset % 8;

        if (bStatus == TRUE)
            lpEM->m_byQueueMap[(WORD)dwByte] |= (BYTE)(1 << dwBit);
        else
            lpEM->m_byQueueMap[(WORD)dwByte] &= ~(BYTE)(1 << dwBit);
    }
}

EXEC_STATUS EMGetByteQueueStatus
(
    LPEXEC_MANAGER              lpEM,
    WORD                        wSeg,
    WORD                        wOff,
    LPBOOL                      lpbQueue
)
{
    DWORD               dwLinearOffset, dwByte, dwBit;

    *lpbQueue = FALSE;

	dwLinearOffset = ((DWORD) wSeg * 0x10 + (DWORD)wOff) &
                     EFFECTIVE_ADDRESS_MASK;

    if (dwLinearOffset >= lpEM->m_dwLinearLower &&
        dwLinearOffset <= lpEM->m_dwLinearUpper)
    {
        dwLinearOffset -= lpEM->m_dwLinearBase;

        dwByte = dwLinearOffset / 8;
        dwBit = dwLinearOffset % 8;

        if (lpEM->m_byQueueMap[(WORD)dwByte] & (1 << dwBit))
            *lpbQueue = TRUE;
        else
            *lpbQueue = FALSE;
    }

    return(EXEC_STATUS_OK);
}

void EMClearWriteMap
(
    LPEXEC_MANAGER              lpEM
)
{
    memset(lpEM->m_byWriteSegmentMap,0,SEGMENT_MAP_SIZE);
}

EXEC_STATUS EMGetMaxWriteRangeSize
(
    LPEXEC_MANAGER              lpEM,
    LPDWORD                     dwRange
)
{
    UINT                        uBaseIndex, uDist, i, uStart, uMax;
    BOOL                        bInRegion;

    uBaseIndex = (UINT)((lpEM->m_dwLinearLower - lpEM->m_dwLinearBase) / 8);
    uDist = (UINT)((lpEM->m_dwLinearUpper - lpEM->m_dwLinearLower) / 8);

    uStart = uMax = 0;
    bInRegion = FALSE;

    for (i=uBaseIndex, uStart = uBaseIndex;i<uDist+uBaseIndex;i++)
    {
#ifdef SYM_NLM
        if (i % 1024 == 0)
            DRelinquishControl();
#endif

        if (lpEM->m_byWriteSegmentMap[i] != 0xFF)
        {
			if (bInRegion == TRUE)
			{
		        if (i - uStart > uMax)
        	        uMax = i - uStart;

	            bInRegion = FALSE;
			}
        }
        else if (bInRegion == FALSE)
        {
            bInRegion = TRUE;
            uStart = i;
        }
    }

    if (bInRegion == TRUE)
    {
        if (i - uStart > uMax)
            uMax = i - uStart;
    }

    *dwRange = uMax * 8;

    return(EXEC_STATUS_OK);
}


EXEC_STATUS EMDequeue
(
    LPEXEC_MANAGER              lpEM,
    LPCPU_CACHE_STATE           lpstState,
    LPBOOL                      lpbGotState
)
{
    LPCPU_CACHE_STATE           lpstTemp;
    DWORD                       dwOffset;

    do
    {
#ifdef SYM_NLM
        DRelinquishControl();
#endif

        *lpbGotState = FALSE;

        if (lpEM->m_lpstTail == NULL)
        {
            *lpbGotState = FALSE;

            return(EXEC_STATUS_OK);
        }

        *lpstState = *lpEM->m_lpstTail;

        if (lpEM->m_lpstTail == lpEM->m_lpstHead)
            lpEM->m_lpstHead = NULL;

        lpstTemp = lpEM->m_lpstTail;

        lpEM->m_lpstTail = lpEM->m_lpstTail->lpstNext;
        PAMMemoryFree(lpstTemp);

        dwOffset = (((DWORD)lpstState->CS * 0x10) + (DWORD)lpstState->IP) &
            EFFECTIVE_ADDRESS_MASK;

        if (dwOffset >= lpEM->m_dwLinearLower && dwOffset <= lpEM->m_dwLinearUpper)
            *lpbGotState = TRUE;
//        else
//            printf("Out of range dequeue %04X:%04X\n",lpstState->CS,lpstState->IP);
    }
    while (*lpbGotState == FALSE);

//    printf("Dequeue %04X:%04X\n",lpstState->CS,lpstState->IP);

    return(EXEC_STATUS_OK);
}


EXEC_STATUS EMEnqueue
(
    LPEXEC_MANAGER              lpEM,
    LPCPU_CACHE_STATE           lpstState
)
{
    DWORD                       dwOffset;
    BOOL                        bHit;

    dwOffset = ((DWORD)lpstState->CS * 0x10) + (DWORD)lpstState->IP;
    if (dwOffset < lpEM->m_dwLinearLower ||
        dwOffset > lpEM->m_dwLinearUpper)
	{
//        printf("Ignored enqueue attempt for %04X:%04X\n",lpstState->CS,lpstState->IP);
		return(EXEC_STATUS_ERROR);
	}

    // make sure we don't enqueue already visited stuff...

    EMGetByteExecStatus(lpEM,lpstState->CS,lpstState->IP,&bHit);
    if (bHit == TRUE)
    {
//        printf("Ignored enqueue attempt for %04X:%04X\n",lpstState->CS,lpstState->IP);
		return(EXEC_STATUS_ERROR);
    }

//    printf("Enqueue %04X:%04X\n",lpstState->CS,lpstState->IP);

    // make sure we don't redundantly enqueue stuff...

    EMGetByteQueueStatus(lpEM,lpstState->CS,lpstState->IP,&bHit);

	if (bHit == TRUE)
    {
        return(EXEC_STATUS_OK);
    }

    EMSetByteQueueStatus(lpEM,lpstState->CS,lpstState->IP,TRUE);

    if (lpEM->m_lpstHead == NULL)
    {
        lpEM->m_lpstHead = (LPCPU_CACHE_STATE)PAMMemoryAlloc(sizeof(CPU_CACHE_STATE_T));
        if (lpEM->m_lpstHead == NULL)
        {
            return(EXEC_STATUS_ERROR);
        }

        *lpEM->m_lpstHead = *lpstState;
        lpEM->m_lpstHead->lpstNext = NULL;

        lpEM->m_lpstTail = lpEM->m_lpstHead;

        return(EXEC_STATUS_OK);
    }

    lpEM->m_lpstHead->lpstNext = (LPCPU_CACHE_STATE)PAMMemoryAlloc(sizeof(CPU_CACHE_STATE_T));
    if (lpEM->m_lpstHead->lpstNext == NULL)
    {
        return(EXEC_STATUS_ERROR);
    }

    lpEM->m_lpstHead = lpEM->m_lpstHead->lpstNext;

    *lpEM->m_lpstHead = *lpstState;
    lpEM->m_lpstHead->lpstNext = NULL;

    return(EXEC_STATUS_OK);
}



EXEC_STATUS EMGetIPOfNextClearBlock
(
    LPEXEC_MANAGER              lpEM,
    LPWORD                      lpwSeg,
    LPWORD                      lpwIP,
    LPBOOL                      bFoundBlock
)
{
    WORD                        wLowerIP, wUpperIP, wByte, wBit;

    wLowerIP = (WORD)(lpEM->m_dwLinearLower - lpEM->m_dwLinearBase);
    wUpperIP = (WORD)(lpEM->m_dwLinearUpper - lpEM->m_dwLinearBase);

    for (; wLowerIP <= wUpperIP; wLowerIP++)
    {
#ifdef SYM_NLM
        if (wLowerIP % 256 == 0)
            DRelinquishControl();
#endif

        wByte = wLowerIP / 8;
        wBit = wLowerIP % 8;

        if (!(lpEM->m_byExecSegmentMap[wByte] & (1 << wBit)))
        {
            *lpwSeg = lpEM->m_wSeg;
            *lpwIP = wLowerIP;
            *bFoundBlock = TRUE;
		  
            return(EXEC_STATUS_OK);
        }
    }

    *bFoundBlock = FALSE;

    return(EXEC_STATUS_OK);
}
					

EXEC_STATUS EMGetRange
(
    LPEXEC_MANAGER              lpEM,
    LPWORD                      pwSeg,
    LPWORD                      pwStartOff,
    LPWORD                      pwEndOff
)
{
    *pwSeg = lpEM->m_wSeg;
    *pwStartOff = lpEM->m_wLowerOff;
    *pwEndOff = lpEM->m_wUpperOff;

    return(EXEC_STATUS_OK);
}



void EMSetExploreMode
(
    LPEXEC_MANAGER              lpEM,
    BOOL                        bExplore
)
{
    lpEM->m_bExplore = bExplore;
}

BOOL EMGetExploreMode
(
    LPEXEC_MANAGER              lpEM
)
{
    return(lpEM->m_bExplore);
}

BOOL EMGetRangeFinalStatus
(
    LPEXEC_MANAGER              lpEM
)
{
    return(lpEM->m_bRangeFinalStatus);
}
