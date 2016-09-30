// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/HEUR/VCS/behav.cpv   1.26   15 Oct 1998 15:36:40   MKEATIN  $
//
// Description:
//
//  Contains code for the bloodhound behavior monitor.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/HEUR/VCS/behav.cpv  $
// 
//    Rev 1.26   15 Oct 1998 15:36:40   MKEATIN
// #ifdef-ed the RelinquishControl call for SYM_NLM.
// 
//    Rev 1.24   14 Oct 1998 11:57:42   MKEATIN
// Carey's RelinquishControl calls for NLM.
// 
//    Rev 1.23   07 Jul 1998 11:36:56   CNACHEN
// Fixed yet another memicmp bug.
// 
//    Rev 1.22   24 Jun 1998 17:40:46   CNACHEN
// fixed mymemicmp function for VxD/NTK platforms
// 
//    Rev 1.21   23 Mar 1998 18:06:56   CNACHEN
// Fixed for NTK
// 
//    Rev 1.20   23 Mar 1998 17:50:58   CNACHEN
// #define memicmp to mymemicmp under ntk platforms
// 
//    Rev 1.19   23 Mar 1998 17:38:54   CNACHEN
// added mymemicmp for ntk platforms
// 
//    Rev 1.18   20 Mar 1998 15:45:22   CNACHEN
// fixed memicmp function.
// 
//    Rev 1.17   13 Mar 1998 11:25:30   CNACHEN
// Fixed to work with VXD (defines our own mymemicmp function)
// 
//    Rev 1.16   03 Nov 1997 17:11:30   CNACHEN
// Updated to support Win16
// 
//    Rev 1.15   10 Jul 1997 17:28:10   CNACHEN
// prints out print_char behavior
// 
//    Rev 1.14   10 Jul 1997 17:05:38   CNACHEN
// Added check for INT 21, AH=40, BX=1 or BX=2 which is common in utils but not
// in viruses.
// 
//    Rev 1.13   09 Jul 1997 16:34:24   CNACHEN
// Added support for 3 config levels for all major config options.
// 
// 
//    Rev 1.12   13 Jun 1997 13:34:14   CNACHEN
// Added support for keyboard and vector 0 behaviors
// 
//    Rev 1.11   10 Jun 1997 13:35:30   CNACHEN
// Added two new behavior checks for prependers: file_Write_Cs100 and repmovs_cs100
// 
//    Rev 1.10   06 Jun 1997 15:31:14   CNACHEN
// Does not terminate after finding INT 21, AH=4C now; continues emulating until
// (1) we start executing the host or (2) we start searching for new regions to
// execute that were never branched to.
// 
//    Rev 1.9   06 Jun 1997 11:21:00   CNACHEN
// Write < 8 means write < 8 and >= 5 now.
// 
// 
//    Rev 1.8   04 Jun 1997 17:54:34   CNACHEN
// fixed strings checking for ?.exe
// 
//    Rev 1.7   04 Jun 1997 17:26:52   CNACHEN
// Added ?.EXE and ?.COM testing to strings.
// 
//    Rev 1.6   02 Jun 1997 17:21:48   CNACHEN
// Added header to file for VCS
// 
// 
//************************************************************************


#include "avtypes.h"

#include "heurapi.h"

#ifdef SYM_WIN16
#define memicmp _fmemicmp
#define strlen _fstrlen
#elif defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_UNIX)
int mymemicmp(LPVOID,LPVOID,int);
#define memicmp mymemicmp
#endif

void BMReset
(
    LPPAMCONFIG                 lpConfig,
    LPBEHAVE_MONITOR            lpBM,
    int                         nHeuristicLevel
)
{
    int                         i;

    // initialize interrupt flags

    MEMSET(lpBM->m_byBehaviorArray,0,sizeof(lpBM->m_byBehaviorArray));

    lpBM->m_byBehaviorArray[HEURISTIC_LEVEL] = (BYTE)nHeuristicLevel;

    // init state flags

    lpBM->m_dwStateFlags1 = 0;
    lpBM->m_dwPersistFlags = 0;
    lpBM->m_dwCallIterNum = 0;
    lpBM->m_dwLinearPUSHFOff = 0;

    // init misc information

    lpBM->m_nFileType = BEHAVE_FILE_TYPE_UNKNOWN;
    lpBM->m_dwLastInstrNum = (DWORD)-1;
    lpBM->m_dwSuspiciousOpcodeCount = 0;
    lpBM->m_dwSuspiciousInt21Count = 0;
    lpBM->m_dwSuspiciousIntCount = 0;
    lpBM->m_nOpcodeIndex = 0;
    lpBM->m_wEPCS = lpBM->m_wEPIP = 0;
    lpBM->m_nNumREPMOVSSubmissions = 0;
    lpBM->m_nNumFileWriteSubmissions = 0;
    lpBM->m_nNumIPSubmissions = 0;
    lpBM->m_wVirusSize = 0;
    lpBM->m_dwImageSize = 0;
    lpBM->m_dwEPToEOFDist = 0;

    for (i=0;i<OPCODE_QUEUE_SIZE;i++)
	lpBM->m_byOpcodeQueue[i] = 0;

    lpBM->m_stConfig = *lpConfig;
}

BOOL BMStopScanning
(
	LPBEHAVE_MONITOR                        lpBM
)
{
    lpBM->m_byBehaviorArray[BF7_STOP_SCANNING] = TRUE;

    return(TRUE);
}

BOOL BMScanningWindowsFile
(
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_byBehaviorArray[BF7_WINDOWS_EXE] = TRUE;

    return(TRUE);
}

BOOL BMStopEmulating
(
    LPBEHAVE_MONITOR            lpBM
)
{
    if (lpBM->m_nFileType == BEHAVE_FILE_TYPE_EXE)
    {
	if (lpBM->m_dwEPToEOFDist > lpBM->m_stConfig.wAssumedVirusSize)
	{
//            printf("StopEmulating: EP to EOF Dist > 4K\n");
	    return(TRUE);
	}

	if (!(lpBM->m_byBehaviorArray[BF7_ENTRY_AFTER_FIXUPS]))
	{
//            printf("StopEmulating: EP after fixups\n");
	    return(TRUE);
	}

	if (lpBM->m_byBehaviorArray[BF7_WINDOWS_EXE])
	{
//            printf("StopEmulating: Windows executable!\n");
	    return(TRUE);
	}
    }

    // check to see if we have a request to stop scanning?

    if (lpBM->m_byBehaviorArray[BF7_STOP_SCANNING] == TRUE)
    {
	return(TRUE);
    }

    // next make sure our image size is OK

    if (lpBM->m_dwImageSize < MIN_VIRUS_SIZE)
    {
//            printf("StopEmulating: Image < 128 bytes\n");
	    return(TRUE);
    }

    // check to see if we have any print statements reached during standard
    // emulation...

    if (lpBM->m_byBehaviorArray[BF9_JUMP_AT_TOF_TO_MIDDLE])
    {
//        printf("StopEmulating: Found JMP to MOF\n");
		return(TRUE);
    }

    if (lpBM->m_byBehaviorArray[BF9_CALL_WITH_LARGE_DISP])
    {
	if (lpBM->m_stConfig.dwFlags & HEUR_FLAG_ABORT_CALL_LARGE_DISP)
	{
	    return(TRUE);
	}
    }

	return(FALSE);
}


BEHAVE_STATUS BMSubmitFileType
(
    LPBEHAVE_MONITOR            lpBM,
    int                         nFileType
)
{
    lpBM->m_nFileType = nFileType;

    if (nFileType == BEHAVE_FILE_TYPE_EXE)
	lpBM->m_byBehaviorArray[BF7_EXE_FILE] = TRUE;
    else
	lpBM->m_byBehaviorArray[BF7_COM_FILE] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMGetFileType
(
    LPBEHAVE_MONITOR            lpBM,
    LPINT                       lpnFileType
)
{
    *lpnFileType = lpBM->m_nFileType;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitInt21Call
(
    LPBEHAVE_MONITOR            lpBM,
    int                         nIntType,
    DWORD                       dwInstrNum,
    WORD                        wAX,
    WORD                        wBX,
    WORD                        wCX,
    WORD                        wDX,
    LPWORD                      lpwAX,
    LPWORD                      lpwBX,
    LPWORD                      lpwCX,
    LPWORD                      lpwDX,
    BOOL                        bExplore

)
{
	if (nIntType == INT_TYPE_INT_21)
	{
	if ((wAX >> 8) > lpBM->m_stConfig.wNormalLegitInt21SubFuncMax)
	    {
	    lpBM->m_byBehaviorArray[IF14_SYSTEM_AM_I_THERE] = TRUE;
	    lpBM->m_wAXFromAmIThere = wAX;
	}

	if ((wAX >> 8) == INT_21_PRINT_STRING)
	{
	    // only look for genuine print int calls

	    if (lpBM->m_dwPersistFlags & PSF_REACHED_FROM_ORIG_EP)
		lpBM->m_byBehaviorArray[IF16_FOUND_PRINT_STRING] = TRUE;

	    // if we're not exploring and hit a print string, exit right away

	    if (bExplore == FALSE &&
		dwInstrNum < lpBM->m_stConfig.wDisreguardCleanBehaviorInstrCount)
	    {
		lpBM->m_byBehaviorArray[BF7_STOP_SCANNING] = TRUE;
	    }
	}


	if ((wAX >> 8) == INT_21_TERMINATE)
	{
	    // only look for genuine print int calls

	    if (lpBM->m_dwPersistFlags & PSF_REACHED_FROM_ORIG_EP)
		lpBM->m_byBehaviorArray[IF16_FOUND_TERMINATE] = TRUE;
	}

	if ((wAX >> 8) == INT_21_WRITE_FILE)
	{
	    if (wBX == 1 || wBX == 2)
	    {
		// allow interpreted expert system to exclude based on this

		if (lpBM->m_dwPersistFlags & PSF_REACHED_FROM_ORIG_EP)
		    lpBM->m_byBehaviorArray[IF16_FOUND_PRINT_CHAR] = TRUE;
	    }
	}
	}


    if (nIntType == INT_TYPE_INT_OTHER || nIntType == INT_TYPE_INT_21)
    {


	return(BMSubmitInt21CallAux(lpBM,
									nIntType,
				    dwInstrNum,
				    wAX,
				    wBX,
				    wCX,
				    wDX,
				    lpwAX,
				    lpwBX,
				    lpwCX,
				    lpwDX));
    }

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitInt21CallAux
(
    LPBEHAVE_MONITOR            lpBM,
    int                         nIntType,
    DWORD                       dwInstrNum,
    WORD                        wAX,
    WORD                        wBX,
    WORD                        wCX,
    WORD                        wDX,
    LPWORD                      lpwAX,
    LPWORD                      lpwBX,
    LPWORD                      lpwCX,
    LPWORD                      lpwDX
)
{
    BYTE        byAH, byAL;
    BOOL        bInt21Func = FALSE;

    // record the instruction number

    lpBM->m_dwLastInstrNum = dwInstrNum;

    byAH = (BYTE)(wAX >> 8);
    byAL = (BYTE)(wAX & 0xFF);

    switch (byAH)
    {
	case INT_21_OPEN_FCB:

	    bInt21Func = TRUE;

	    lpBM->m_byBehaviorArray[IF1_FILE_OPEN_UNKNOWN_MODE] = TRUE;
	    break;

	case INT_21_OPEN_FILE:
	case INT_21_OPEN_FILE_EXT:

	    bInt21Func = TRUE;

	    if ((byAL & INT_21_FILE_OPEN_MODE_MASK) ==
		INT_21_FILE_OPEN_READ_WRITE)
		lpBM->m_byBehaviorArray[IF1_FILE_OPEN_READ_WRITE] = TRUE;
	    else if ((byAL & INT_21_FILE_OPEN_MODE_MASK) ==
		INT_21_FILE_OPEN_READ_ONLY)
		lpBM->m_byBehaviorArray[IF1_FILE_OPEN_READ_ONLY] = TRUE;
	    else if ((byAL & INT_21_FILE_OPEN_MODE_MASK) ==
		INT_21_FILE_OPEN_WRITE_ONLY)
		lpBM->m_byBehaviorArray[IF1_FILE_OPEN_WRITE_ONLY] = TRUE;
	    else
		lpBM->m_byBehaviorArray[IF1_FILE_OPEN_UNKNOWN_MODE] = TRUE;
	    break;

	case INT_21_CLOSE_FCB:
	case INT_21_CLOSE_FILE:

	    bInt21Func = TRUE;

	    lpBM->m_byBehaviorArray[IF5_FILE_CLOSE] = TRUE;
	    break;

	case INT_21_FIND_FIRST_FCB:
	case INT_21_FIND_FIRST:
	    bInt21Func = TRUE;

	    
	    lpBM->m_byBehaviorArray[IF9_FILE_FIND_FIRST] = TRUE;
	    break;

	case INT_21_FIND_NEXT_FCB:
	case INT_21_FIND_NEXT:
	    bInt21Func = TRUE;

	    lpBM->m_byBehaviorArray[IF9_FILE_FIND_NEXT] = TRUE;
	    break;

	case INT_21_DELETE_FCB:
	case INT_21_DELETE_FILE:
	    bInt21Func = TRUE;

	    lpBM->m_byBehaviorArray[IF10_FILE_DELETE] = TRUE;
	    break;

	case INT_21_CREATE_FCB:
	case INT_21_CREATE_FILE:
	    bInt21Func = TRUE;

	    lpBM->m_byBehaviorArray[IF10_FILE_CREATE] = TRUE;
	    break;

	case INT_21_RENAME_FCB:
	case INT_21_RENAME_FILE:
	    bInt21Func = TRUE;

	    lpBM->m_byBehaviorArray[IF10_FILE_RENAME] = TRUE;
	    break;

	case INT_21_SET_DTA:
	    bInt21Func = TRUE;

	    lpBM->m_byBehaviorArray[IF11_FILE_SET_DTA] = TRUE;
	    break;

	case INT_21_SET_VECTOR:
	    bInt21Func = TRUE;

	    if (byAL == 0x21)
		lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_21] = TRUE;
	    else if (byAL == 0x13)
		lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_13] = TRUE;
	    else if (byAL == 0x24)
		lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_24] = TRUE;
	    else if (byAL < lpBM->m_stConfig.wNormalInterruptVectorMax)
		lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_LOW] = TRUE;
	    else
		lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_HIGH] = TRUE;
	    break;

	case INT_21_GET_DATE:
	    bInt21Func = TRUE;

	    lpBM->m_byBehaviorArray[IF8_SYSTEM_GET_DATE] = TRUE;
	    break;

	case INT_21_GET_TIME:
	    bInt21Func = TRUE;

	    lpBM->m_byBehaviorArray[IF8_SYSTEM_GET_TIME] = TRUE;
	    break;

	case INT_21_GET_DTA:
	    bInt21Func = TRUE;

	    lpBM->m_byBehaviorArray[IF11_FILE_GET_DTA] = TRUE;
	    break;

	case INT_21_GET_VECTOR:
	    bInt21Func = TRUE;

	    if (byAL == 0x21)
		lpBM->m_byBehaviorArray[IF12_SYSTEM_GET_VECTOR_21] = TRUE;
	    else if (byAL == 0x13)
		lpBM->m_byBehaviorArray[IF12_SYSTEM_GET_VECTOR_13] = TRUE;
	    else if (byAL == 0x24)
		lpBM->m_byBehaviorArray[IF12_SYSTEM_GET_VECTOR_24] = TRUE;
	    else if (byAL < lpBM->m_stConfig.wNormalInterruptVectorMax)
		lpBM->m_byBehaviorArray[IF12_SYSTEM_GET_VECTOR_LOW] = TRUE;
	    else
		lpBM->m_byBehaviorArray[IF12_SYSTEM_GET_VECTOR_HIGH] = TRUE;
	    break;

	case INT_21_CHDIR:
	    bInt21Func = TRUE;

	    lpBM->m_byBehaviorArray[IF10_FILE_CHDIR] = TRUE;
	    break;

	case INT_21_READ_FILE:
			
	    bInt21Func = TRUE;

	    if (wCX == 3 || wCX == 4)
		lpBM->m_byBehaviorArray[IF2_FILE_READ_3_OR_4] = TRUE;
	    else if (wCX < 8 && wCX > 4)
		lpBM->m_byBehaviorArray[IF2_FILE_READ_LESS_8] = TRUE;
	    else if (wCX >= 0x1c && wCX <= 0x1e)
		lpBM->m_byBehaviorArray[IF2_FILE_READ_1C_TO_1E] = TRUE;
	    else if (wCX == 0xFFFF)
		lpBM->m_byBehaviorArray[IF2_FILE_READ_FFFF] = TRUE;
	    else if (wCX >= (FILE_SIZE_BAIT_VALUE -
			     lpBM->m_stConfig.wReadWriteBaitThreshold) &&
		     wCX < (FILE_SIZE_BAIT_VALUE +
			     lpBM->m_stConfig.wReadWriteBaitThreshold))
		lpBM->m_byBehaviorArray[IF2_FILE_READ_BAIT] = TRUE;
	    else if (wCX >= 128)
		lpBM->m_byBehaviorArray[IF2_FILE_READ_128_PLUS] = TRUE;
	    else
		lpBM->m_byBehaviorArray[IF2_FILE_READ_GENERAL] = TRUE;
	    break;

	case INT_21_WRITE_FILE:
			
	    bInt21Func = TRUE;

	    // for file writes, make sure we're not writing
	    // to stdout or stderr; BX (handle) == 1 or 2 ...

	    if (wBX == 1 || wBX == 2)
		break;

	    if (wCX == 3 || wCX == 4)
		lpBM->m_byBehaviorArray[IF3_FILE_WRITE_3_OR_4] = TRUE;
	    else if (wCX < 8 && wCX > 4)
	    {
		lpBM->m_byBehaviorArray[IF3_FILE_WRITE_LESS_8] = TRUE;
	    }
	    else if (wCX >= 0x1c && wCX <= 0x1e)
		lpBM->m_byBehaviorArray[IF3_FILE_WRITE_1C_TO_1E] = TRUE;
	    else if (wCX >= (FILE_SIZE_BAIT_VALUE -
			     lpBM->m_stConfig.wReadWriteBaitThreshold) &&
		     wCX < (FILE_SIZE_BAIT_VALUE +
			     lpBM->m_stConfig.wReadWriteBaitThreshold))
		lpBM->m_byBehaviorArray[IF3_FILE_WRITE_BAIT] = TRUE;
	    else if (wCX >= 128)
	    {
		lpBM->m_byBehaviorArray[IF3_FILE_WRITE_128_PLUS] = TRUE;

		BMCheckWritesAgainstIP(lpBM,wCX);
		BMCheckWritesAgainstMOVS(lpBM,wCX);

	    }
	    else
		lpBM->m_byBehaviorArray[IF3_FILE_WRITE_GENERAL] = TRUE;
	    break;

	case INT_21_SEEK_FILE:
			
	    bInt21Func = TRUE;

	    if (byAL == INT_21_FILE_SEEK_SET &&
		wCX == 0 && wDX == 0)
		lpBM->m_byBehaviorArray[IF4_FILE_SEEK_TOF] = TRUE;
	    else if (byAL == INT_21_FILE_SEEK_END &&
		     wCX == 0 && wDX == 0)
		lpBM->m_byBehaviorArray[IF4_FILE_SEEK_EOF] = TRUE;
	    else
		lpBM->m_byBehaviorArray[IF4_FILE_SEEK_GENERAL] = TRUE;
	    break;

	case INT_21_CHMOD_FILE:
	    bInt21Func = TRUE;

	    if (byAL == INT_21_FILE_CHMOD_GET)
		lpBM->m_byBehaviorArray[IF6_FILE_GET_ATTR] = TRUE;
	    else
		lpBM->m_byBehaviorArray[IF6_FILE_SET_ATTR] = TRUE;
	    break;

	case INT_21_GET_CUR_DIR:
	    bInt21Func = TRUE;

	    lpBM->m_byBehaviorArray[IF10_FILE_GET_CUR_DIR] = TRUE;
	    break;

	case INT_21_MEM_ALLOC:
	    bInt21Func = TRUE;

	    lpBM->m_byBehaviorArray[IF13_SYSTEM_MEM_ALLOC] = TRUE;
	    break;

	case INT_21_EXEC:
	    lpBM->m_byBehaviorArray[IF13_SYSTEM_PROG_EXEC] = TRUE;
	    break;

	case INT_21_TIME_STAMP:
	    bInt21Func = TRUE;

	    if (byAL == INT_21_FILE_TIME_GET)
		lpBM->m_byBehaviorArray[IF7_FILE_GET_FILE_DATE] = TRUE;
	    else
		lpBM->m_byBehaviorArray[IF7_FILE_SET_FILE_DATE] = TRUE;
	    break;
    }

    // update registers based on behavior...

    switch (byAH)
    {
	case INT_21_GET_DOS_VERSION:
	    *lpwAX = 1;             // just in case
	    *lpwBX = *lpwCX = 0;    // some viruses rely on BX=CX=0
	    break;

	case INT_21_OPEN_FILE:
	case INT_21_OPEN_FILE_EXT:
	    *lpwAX = 7;             // file handle to give back to program
	    break;

	case INT_21_READ_FILE:
	    if (wCX == 0xFFFF)
		*lpwAX = FILE_SIZE_BAIT_VALUE;
	    break;

	case INT_21_SEEK_FILE:
	    if (byAL == INT_21_FILE_SEEK_END &&
		     wCX == 0 && wDX == 0)
		*lpwDX = 0;
		*lpwAX = FILE_SIZE_BAIT_VALUE;
	    break;
    }

    if (byAH >= 0x6D)
    {
	// all DOS functions >= 6D clear value in AL...

	*lpwAX = wAX & 0xFF00U;
    }

    // pushf call far int 21 call (ah was right...)

    if (bInt21Func == TRUE && nIntType == INT_TYPE_FAR_CALL)
	lpBM->m_byBehaviorArray[BF5_FOUND_PUSHF_CALL_FAR_21] = TRUE;

    if (bInt21Func == TRUE && nIntType == INT_TYPE_INT_OTHER)
    {
	lpBM->m_dwSuspiciousInt21Count++;

	if (lpBM->m_dwSuspiciousInt21Count >= lpBM->m_stConfig.wSuspiciousIntThreshold)
	    lpBM->m_byBehaviorArray[BF5_FOUND_SUSPICIOUS_INT_21] = TRUE;
    }
    
    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitDirectGetVector
(
    LPBEHAVE_MONITOR            lpBM,
    BYTE                        byIntNum
)
{
    if (byIntNum == 0x21)
	lpBM->m_byBehaviorArray[IF12_SYSTEM_GET_VECTOR_21] = TRUE;
    else if (byIntNum == 0x13)
	lpBM->m_byBehaviorArray[IF12_SYSTEM_GET_VECTOR_13] = TRUE;
    else if (byIntNum == 0x24)
	lpBM->m_byBehaviorArray[IF12_SYSTEM_GET_VECTOR_24] = TRUE;
    else if (byIntNum <= lpBM->m_stConfig.wNormalInterruptVectorMax)
	lpBM->m_byBehaviorArray[IF12_SYSTEM_GET_VECTOR_LOW] = TRUE;
    else
	lpBM->m_byBehaviorArray[IF12_SYSTEM_GET_VECTOR_HIGH] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitDirectSetVector
(
    LPBEHAVE_MONITOR            lpBM,
    BYTE                        byIntNum
)
{
    if (byIntNum == 0x21)
	lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_21] = TRUE;
    else if (byIntNum == 0x13)
	lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_13] = TRUE;
    else if (byIntNum == 0x24)
	lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_24] = TRUE;
    else if (byIntNum <= lpBM->m_stConfig.wNormalInterruptVectorMax)
	lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_LOW] = TRUE;
    else
	lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_HIGH] = TRUE;

    // record that it was a direct set vector since this is fishy

    lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_DIRECT] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitMCBRead
(
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_byBehaviorArray[BF1_READ_MCB] = TRUE;

    return(BEHAVE_STATUS_OK);
}


BEHAVE_STATUS BMSubmitMCBWrite
(
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_byBehaviorArray[BF1_WRITE_MCB] = TRUE;

    return(BEHAVE_STATUS_OK);
}


BEHAVE_STATUS BMSubmitIntCall
(
    LPBEHAVE_MONITOR            lpBM,
    BYTE                        byIntNum,
    DWORD                       dwInstrNum,
    WORD                        wAX,
    WORD                        wBX,
    WORD                        wCX,
    WORD                        wDX,
    LPWORD                      lpwAX,
    LPWORD                      lpwBX,
    LPWORD                      lpwCX,
    LPWORD                      lpwDX
)
{
    BYTE        byAH, byAL;

    byAH = (BYTE)(wAX >> 8);
    byAL = (BYTE)(wAX & 0xFF);

    if (byIntNum == 0x13)
    {
	switch(byAH)
	{
	    case INT_13_READ_FUNC:
		if (wCX == 0x0001 && (wDX >> 8) == 0)      // track0,sec1,head0
		    lpBM->m_byBehaviorArray[IF15_INT_13_READ_MBR] = TRUE;
		else
		    lpBM->m_byBehaviorArray[IF15_INT_13_READ_GENERAL] = TRUE;
		break;

	    case INT_13_WRITE_FUNC:
		if (wCX == 0x0001 && (wDX >> 8) == 0)      // track0,sec1,head0
		    lpBM->m_byBehaviorArray[IF15_INT_13_WRITE_MBR] = TRUE;
		else
		    lpBM->m_byBehaviorArray[IF15_INT_13_WRITE_GENERAL] = TRUE;
		break;

	    case INT_13_FORMAT_FUNC:
		lpBM->m_byBehaviorArray[IF15_INT_13_FORMAT] = TRUE;
		break;

	    default:
		lpBM->m_byBehaviorArray[IF15_INT_13_GENERAL] = TRUE;
		break;
	}
    }

    if (byIntNum == 0x16)
    {
	if (byAH == 0 || byAH == 1)
	{
	    if (lpBM->m_dwPersistFlags & PSF_REACHED_FROM_ORIG_EP)
		lpBM->m_byBehaviorArray[IF16_FOUND_KEYBOARD_INPUT] = TRUE;
	}
    }

    if (byIntNum == 0x1a)
    {
	lpBM->m_byBehaviorArray[IF8_SYSTEM_GET_TIME] = TRUE;
    }

    if (byIntNum > lpBM->m_stConfig.wNormalInterruptVectorMax ||
	lpBM->m_stConfig.
	    bySuspiciousInts[byIntNum] != 0)    // these ints are unused basically
    {
	lpBM->m_dwSuspiciousIntCount++;

	// check to see if we have an int 21-like function... if so, assume
	// this is a call to int 21 and deal with it accordingly...

	switch (byAH)
	{

	    case INT_21_OPEN_FCB:
	    case INT_21_CLOSE_FCB:
	    case INT_21_FIND_FIRST_FCB:
	    case INT_21_FIND_NEXT_FCB:
	    case INT_21_DELETE_FCB:
	    case INT_21_CREATE_FCB:
	    case INT_21_RENAME_FCB:
	    case INT_21_SET_DTA:
	    case INT_21_SET_VECTOR:
	    case INT_21_GET_DATE:
	    case INT_21_GET_TIME:
	    case INT_21_GET_DTA:
	    case INT_21_GET_VECTOR:
	    case INT_21_CHDIR:
	    case INT_21_CREATE_FILE:
	    case INT_21_OPEN_FILE:
	    case INT_21_CLOSE_FILE:
	    case INT_21_READ_FILE:
	    case INT_21_WRITE_FILE:
	    case INT_21_DELETE_FILE:
	    case INT_21_SEEK_FILE:
	    case INT_21_CHMOD_FILE:
	    case INT_21_GET_CUR_DIR:
	    case INT_21_MEM_ALLOC:
	    case INT_21_FIND_FIRST:
	    case INT_21_FIND_NEXT:
	    case INT_21_RENAME_FILE:
	    case INT_21_TIME_STAMP:
	    case INT_21_OPEN_FILE_EXT:

		lpBM->m_byBehaviorArray[BF5_FOUND_SUSPICIOUS_INT] = TRUE;

		return (BMSubmitInt21CallAux(lpBM,
											 INT_TYPE_INT_OTHER,
					     dwInstrNum,
					     wAX,
					     wBX,
					     wCX,
					     wDX,
					     lpwAX,
					     lpwBX,
					     lpwCX,
					     lpwDX));
	    default:
		break;
	}
    }

    return(BEHAVE_STATUS_OK);
}


BEHAVE_STATUS BMSubmitE9AtTOF
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wDisp
)
{
    DWORD           dwMinVirusOffset;

    lpBM->m_byBehaviorArray[BF5_FOUND_E9_AT_TOF] = TRUE;

    wDisp += 0x103;         // determine the dest offset...
    wDisp -= 0x100;         // normalize w/respect to TOF

    if (lpBM->m_dwImageSize >= lpBM->m_stConfig.wAssumedVirusSize)
	dwMinVirusOffset = lpBM->m_dwImageSize -
				lpBM->m_stConfig.wAssumedVirusSize;
    else
	dwMinVirusOffset = 0;

    if (wDisp > lpBM->m_stConfig.wAssumedVirusSize &&
	wDisp < dwMinVirusOffset)
    {
	lpBM->m_byBehaviorArray[BF9_JUMP_AT_TOF_TO_MIDDLE] = TRUE;
    }

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitIP100
(
    LPBEHAVE_MONITOR            lpBM,
    DWORD                       dwInstrNum
)
{
    // this is only noteworthy if we have done so after instr #0

    if (dwInstrNum != 0)
	lpBM->m_byBehaviorArray[BF2_JUMP_TO_CS100] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitWriteTo100
(
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_byBehaviorArray[BF1_WRITE_TO_CS100] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitEP
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wCS,
    WORD                        wIP,
    DWORD                       dwEPToEOFDist
)
{
    lpBM->m_wEPCS = wCS;
    lpBM->m_wEPIP = wIP;
    lpBM->m_dwEPToEOFDist = dwEPToEOFDist;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitFarJump
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wDestCS,
    WORD                        wDestIP
)
{
    DWORD           dwLinearEP, dwLinearCur;

    dwLinearEP = (DWORD)lpBM->m_wEPCS * 0x10 + lpBM->m_wEPIP;
    dwLinearCur = (DWORD)wDestCS * 0x10 + wDestIP;

    if (lpBM->m_nFileType == BEHAVE_FILE_TYPE_EXE && dwLinearEP > dwLinearCur)
	lpBM->m_byBehaviorArray[BF2_FAR_JUMP_BEFORE_ENTRY_EXE] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitImageSize
(
    LPBEHAVE_MONITOR            lpBM,
    DWORD                       dwImageSize
)
{
    lpBM->m_dwImageSize = dwImageSize;

	return(BEHAVE_STATUS_OK);
}


BEHAVE_STATUS BMSubmitBYTECMP     // E9
(
    LPBEHAVE_MONITOR            lpBM,
    int                         nRegister,
    BYTE                        byValue
)
{
    if (byValue == 0xE9)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_ANY_E9] = TRUE;

    if (nRegister == BEHAVE_REG_AH && byValue == 0x4B)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_4B] = TRUE;

    if (nRegister == BEHAVE_REG_AH && byValue == 0x3D)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_3D] = TRUE;

    if (nRegister == BEHAVE_REG_AH && byValue == 0x11)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_11] = TRUE;

    if (nRegister == BEHAVE_REG_AH && byValue == 0x12)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_12] = TRUE;

    if (nRegister == BEHAVE_REG_AH && byValue == 0x4E)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_4E] = TRUE;

    if (nRegister == BEHAVE_REG_AH && byValue == 0x4F)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_4F] = TRUE;

    if (lpBM->m_byBehaviorArray[IF14_SYSTEM_AM_I_THERE] &&
	byValue == lpBM->m_wAXFromAmIThere>>8)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AM_I_THERE] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitWORDCMP     // ZM or ZM
(
    LPBEHAVE_MONITOR            lpBM,
    int                         nRegister,
    WORD                        wValue
)
{
    BYTE            byHigh, byLow;

    byHigh = (BYTE)(wValue >> 8);
    byLow = (BYTE)(wValue & 0xFF);

    if (byHigh == 0xE9 || byLow == 0xE9)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_ANY_E9] = TRUE;

    if (nRegister == BEHAVE_REG_AX && byHigh == 0x4B)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_4B] = TRUE;

    if (nRegister == BEHAVE_REG_AX && byHigh == 0x3D)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_3D] = TRUE;

    if (nRegister == BEHAVE_REG_AX && byHigh == 0x11)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_11] = TRUE;

    if (nRegister == BEHAVE_REG_AX && byHigh == 0x12)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_12] = TRUE;

    if (nRegister == BEHAVE_REG_AX && byHigh == 0x4E)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_4E] = TRUE;

    if (nRegister == BEHAVE_REG_AX && byHigh == 0x4F)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_4F] = TRUE;

    if (wValue == 0x5A4D)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_ANY_MZ] = TRUE;

    if (lpBM->m_byBehaviorArray[IF14_SYSTEM_AM_I_THERE] &&
	wValue == lpBM->m_wAXFromAmIThere)
	lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AM_I_THERE] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitREPMOVS
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wSourceSeg,
    WORD                        wSourceOff,
    WORD                        wLength,
    WORD                        wCurCS,
    WORD                        wCurIP
)
{
    DWORD           dwLinearMoveStart, dwLinearCSIP;

    BMCheckMOVSAgainstWrites(lpBM,wLength);

    lpBM->m_byBehaviorArray[BF5_FOUND_REPMOVS] = TRUE;

    dwLinearMoveStart = ((DWORD)wSourceSeg * 0x10) + wSourceOff;
    dwLinearCSIP = ((DWORD)wCurCS * 0x10) + wCurIP;

    if (dwLinearCSIP >= dwLinearMoveStart &&
	dwLinearCSIP < dwLinearMoveStart + (DWORD)wLength)
    {
	// code at the CSIP is being moved...

	lpBM->m_byBehaviorArray[BF5_FOUND_REPMOVS_CODE] = TRUE;
    }

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitEntryAfterFixups
(
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_byBehaviorArray[BF7_ENTRY_AFTER_FIXUPS] = TRUE;

    return(BEHAVE_STATUS_OK);
}


BEHAVE_STATUS BMSubmitFarCall
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wCS,
    WORD                        wIP,
    WORD                        wAX,
    WORD                        wBX,
    WORD                        wCX,
    WORD                        wDX,
    DWORD                       dwInstrNum,
    LPWORD                      lpwAX,
    LPWORD                      lpwBX,
    LPWORD                      lpwCX,
    LPWORD                      lpwDX,
    LPWORD                      lpwSP,
    BOOL                        bExplore,
    LPBOOL                      lpbPossibleIntCall
)
{
    DWORD           dwLinearOff;
    BEHAVE_STATUS   status;

    *lpbPossibleIntCall = FALSE;

    dwLinearOff = (DWORD)wCS * 0x10 + (DWORD)wIP;

    if (dwLinearOff == lpBM->m_dwLinearPUSHFOff + 1)
    {
	// we have a possible INT 21 call... lets assume!

	lpBM->m_byBehaviorArray[BF5_FOUND_PUSHF_CALL_FAR] = TRUE;

	status = BMSubmitInt21CallAux(lpBM,
									  INT_TYPE_FAR_CALL,
				      dwInstrNum,
				      wAX,
				      wBX,
				      wCX,
				      wDX,
				      lpwAX,
				      lpwBX,
				      lpwCX,
				      lpwDX);

	*lpbPossibleIntCall = TRUE;

	// note, CS:IP has not been pushed yet so don't worry about removing
	// it from the stack... only remove the flags...

	if (bExplore == TRUE)
	    *lpwSP += sizeof(WORD);         // pop flags

	return(status);
    }

    return(BEHAVE_STATUS_OK);
}


BEHAVE_STATUS BMSubmitCall
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wDestCS,
    WORD                        wDestIP,
    WORD                        wDisp,
    WORD                        wAX,
    WORD                        wBX,
    WORD                        wCX,
    WORD                        wDX,
    DWORD                       dwInstrNum,
    LPBYTE                      lpbyDestIPBuffer,
    int                         nBufferLen,
    BOOL                        bExplore
)
{
    int             i;
    BOOL            bPUSHF, bCallFar, bCallInt;
    WORD            wTemp;

    if (wDisp == 0)
	lpBM->m_byBehaviorArray[BF5_FOUND_E80000] = TRUE;

    if (nBufferLen == 0)
	return(BEHAVE_STATUS_OK);

	// now check to see if we get any calls that have a large displacement,
    // indicating that we don't have a virus...

    if (dwInstrNum >= lpBM->m_stConfig.wMaxIterForEPAdjust &&
	bExplore == FALSE)
    {
	if (wDisp & 0x8000)
	    wDisp = -(short)wDisp;

	if (wDisp > lpBM->m_stConfig.wAssumedVirusSize )
	    lpBM->m_byBehaviorArray[BF9_CALL_WITH_LARGE_DISP] = TRUE;
    }


    bCallInt = bPUSHF = bCallFar = FALSE;

    nBufferLen--;

    for (i=0;i<nBufferLen;i++)
    {
	if (lpbyDestIPBuffer[i] == 0x9c)    // pushf
	    bPUSHF = TRUE;

	if (lpbyDestIPBuffer[i] == 0xFF && // call far indirect
	     ((lpbyDestIPBuffer[i+1] >> 3) & 7) == 3)
	    bCallFar = TRUE;

	if (lpbyDestIPBuffer[i] == 0x9A)    // call far direct
	    bCallFar = TRUE;

	if (lpbyDestIPBuffer[i] == 0xCD)
	    bCallInt = TRUE;
    }

    if ((bPUSHF == TRUE && bCallFar == TRUE) || bCallInt == TRUE)
    {
	int                         nIntType;

	if (bCallInt == TRUE)
	    nIntType = INT_TYPE_INT_OTHER;
	else if (bCallFar == TRUE)
	    nIntType = INT_TYPE_FAR_CALL;

	return(BMSubmitInt21CallAux(lpBM,
									nIntType,
				    dwInstrNum,
				    wAX,
				    wBX,
				    wCX,
				    wDX,
				    &wTemp,
				    &wTemp,
				    &wTemp,
				    &wTemp));

	// use wTemp for output registers since call is being used
	// and is not really being emulated
    }

    
    return(BEHAVE_STATUS_OK);
}


BEHAVE_STATUS BMSubmitJmp
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wDestCS,
    WORD                        wDestIP,
    WORD                        wDisp,
    WORD                        wAX,
    WORD                        wBX,
    WORD                        wCX,
    WORD                        wDX,
    DWORD                       dwInstrNum,
    LPBYTE                      lpbyDestIPBuffer,
    int                         nBufferLen,
    BOOL                        bExplore
)
{
    int             i;
    BOOL            bPUSHF, bCallFar, bCallInt;
    WORD            wTemp;

    if (nBufferLen == 0)
	return(BEHAVE_STATUS_OK);

    // now check to see if we get any jmps that have a large displacement,
    // indicating that we don't have a virus...

    if (dwInstrNum >= lpBM->m_stConfig.wMaxIterForEPAdjust &&
	bExplore == FALSE)
    {
	if (wDisp & 0x8000)
	    wDisp = -(short)wDisp;

	if (wDisp > lpBM->m_stConfig.wAssumedVirusSize )
	    lpBM->m_byBehaviorArray[BF9_CALL_WITH_LARGE_DISP] = TRUE;
    }


    bCallInt = bPUSHF = bCallFar = FALSE;

    nBufferLen--;

    if (nBufferLen > 3)                     // limit searching for CD to only
	nBufferLen = 3;                     // 3 bytes

    for (i=0;i<nBufferLen;i++)
    {
	if (lpbyDestIPBuffer[i] == 0x9c)    // pushf
	    bPUSHF = TRUE;

	if (lpbyDestIPBuffer[i] == 0xFF && // call far indirect
	     ((lpbyDestIPBuffer[i+1] >> 3) & 7) == 3)
	    bCallFar = TRUE;

	if (lpbyDestIPBuffer[i] == 0x9A)    // call far direct
	    bCallFar = TRUE;

	if (lpbyDestIPBuffer[i] == 0xCD)
	    bCallInt = TRUE;
    }

    if ((bPUSHF == TRUE && bCallFar == TRUE) || bCallInt == TRUE)
    {
	int                         nIntType;

	if (bCallInt == TRUE)
	    nIntType = INT_TYPE_INT_OTHER;
	else if (bCallFar == TRUE)
	    nIntType = INT_TYPE_FAR_CALL;

	return(BMSubmitInt21CallAux(lpBM,
								    nIntType,
				    dwInstrNum,
				    wAX,
				    wBX,
				    wCX,
				    wDX,
				    &wTemp,
				    &wTemp,
				    &wTemp,
				    &wTemp));

	// use wTemp for output registers since call is being used
	// and is not really being emulated
    }


    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitSelfModifyingCode
(
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_byBehaviorArray[BF6_FOUND_SELF_MOD_CODE] = TRUE;
	 
    return(BEHAVE_STATUS_OK);
}


BEHAVE_STATUS BMSubmitOpcode   // IRET, PUSHF, CS: with EXE
(
    LPBEHAVE_MONITOR            lpBM,
    BYTE                        byOpcode,
    WORD                        wCS,
    WORD                        wIP,
    WORD                        wAfterCS,
    WORD                        wAfterIP,
    DWORD                       dwInstrNum
)
{
	// update the opcode queue

    lpBM->m_byOpcodeQueue[lpBM->m_nOpcodeIndex] = byOpcode;
    lpBM->m_nOpcodeIndex++;
    if (lpBM->m_nOpcodeIndex == OPCODE_QUEUE_SIZE)
	lpBM->m_nOpcodeIndex = 0;

    // count suspicious opcodes

    BMCheckForSuspiciousOpcodes(lpBM,byOpcode);

    // check for calls & pops

    BMCheckForCallPop(lpBM,byOpcode, wCS, wIP, dwInstrNum);

    // check for pushf and call far

    BMCheckForPUSHF(lpBM,byOpcode, wCS, wIP, dwInstrNum);

    // check for push/ret

    if ((byOpcode & 0xF8) == 0x50)              // check for push
    {
	lpBM->m_dwStateFlags1 |= SF1_FOUND_PUSH;
    }
    else
    {
	if (lpBM->m_dwStateFlags1 & SF1_FOUND_PUSH)
	{
	    if (byOpcode == 0xC3 || byOpcode == 0xCB)   // check for RET/F
	    {
		BMSubmitPushRet(lpBM, wAfterIP == 0x100);
	    }
	}

	lpBM->m_dwStateFlags1 &= ~SF1_FOUND_PUSH;
    }

	// update the instr #

    lpBM->m_dwLastInstrNum = dwInstrNum;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMCheckForSuspiciousOpcodes
(
    LPBEHAVE_MONITOR            lpBM,
    BYTE                        byOpcode
)
{
    switch (byOpcode)
    {
	case 0x90:          // NOP
	case 0xF5:          // CMC
	case 0xF9:          // STC
	case 0xFA:          // CLI
	case 0xFB:          // STI
	case 0xFC:          // CLD
	case 0xFD:          // STD
	    lpBM->m_dwSuspiciousOpcodeCount++;

	    if (lpBM->m_dwSuspiciousOpcodeCount >= lpBM->m_stConfig.wSuspiciousOpcodeThreshold)
		lpBM->m_byBehaviorArray[BF5_FOUND_SUSPICIOUS_OPCODES] = TRUE;

	    break;
	case 0xCF:          // IRET
	    lpBM->m_byBehaviorArray[BF5_FOUND_IRET] = TRUE;
	    break;
    }

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMCheckForCallPop
(
    LPBEHAVE_MONITOR            lpBM,
    BYTE                        byOpcode,
    WORD                        wCS,
    WORD                        wIP,
    DWORD                       dwInstrNum
)
{
    
    if (byOpcode == 0xE8)           // call opcode == 0xE8
    {
	lpBM->m_dwStateFlags1 |= SF1_FOUND_CALL;
	lpBM->m_dwCallIterNum = dwInstrNum;

	return(BEHAVE_STATUS_OK);
    }

    if (lpBM->m_dwStateFlags1 & SF1_FOUND_CALL)
    {
	switch (byOpcode)
	{
	    case 0x58:          // pop AX
	    case 0x5B:          // pop BX
	    case 0x59:          // pop CX
	    case 0x5A:          // pop DX
	    case 0x5E:          // pop SI
	    case 0x5F:          // pop DI
	    case 0x5D:          // pop BP

		if (dwInstrNum == lpBM->m_dwCallIterNum + 1)
		    lpBM->m_byBehaviorArray[BF5_FOUND_CALL_POP] = TRUE;

		// fall through to default

	    default:
//                only remove our call status if we find a PUSH of some
//                sort  (see submitpush member function)

//                lpBM->m_dwStateFlags1 &= ~(DWORD)SF1_FOUND_CALL;
		break;
	}
    }

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMCheckForPUSHF
(
    LPBEHAVE_MONITOR            lpBM,
    BYTE                        byOpcode,
    WORD                        wCS,
    WORD                        wIP,
    DWORD                       dwInstrNum
)
{
    if (byOpcode == 0x9C)                       // PUSHF
    {
	lpBM->m_dwStateFlags1 |= SF1_FOUND_PUSHF;
	lpBM->m_dwLinearPUSHFOff = (DWORD)wCS * 0x10 + (DWORD)wIP;

	return(BEHAVE_STATUS_OK);
    }



    if (lpBM->m_dwStateFlags1 & SF1_FOUND_PUSHF)
    {
	switch (byOpcode)
	{
	    case 0x2E:                          // CS: prefix
	    case 0x3E:                          // DS: prefix
	    case 0x26:                          // ES: prefix
	    case 0x36:                          // SS: prefix

		// update the location of the supposed PUSHF so
		// our call far is properly handled

		lpBM->m_dwLinearPUSHFOff = (DWORD)wCS * 0x10 + (DWORD)wIP;
		break;
	    default:
//                lpBM->m_dwStateFlags1 &= ~(DWORD)SF1_FOUND_PUSHF;
		break;
	}
    }

    return(BEHAVE_STATUS_OK);
}


BEHAVE_STATUS BMResetContext        // reset CALL/POP status and
(                                   //       PUSHF/CALLFAR status
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_dwStateFlags1 &= ~(DWORD)(SF1_FOUND_CALL | SF1_FOUND_PUSHF);

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitPush
(
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_dwStateFlags1 &= ~(DWORD)(SF1_FOUND_CALL | SF1_FOUND_PUSHF);

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitPop       // don't remove call pop status
(                               // as the pop hasn't yet been
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_dwStateFlags1 &= ~(DWORD)(SF1_FOUND_PUSHF);   // do remove pushf status

    return(BEHAVE_STATUS_OK);
}

BOOL BMMatchAlternatingString
(
    LPBEHAVE_MONITOR            lpBM,
    LPBYTE                      lpbyBuffer,
    DWORD                       dwMaxLen
)
{
/*
    DWORD                       i;
    int             nLowerState, nStringLen;

    if (dwMaxLen > MAX_ALTERNATING_STRING_LEN)
	dwMaxLen = MAX_ALTERNATING_STRING_LEN;

    nStringLen = 0;

    if (islower(lpbyBuffer[0]))
    {
	nLowerState = ALTERNATE_STATE_LOWER;
	nStringLen++;
    }
    else
    {
	if (isalpha(lpbyBuffer[0]))
	{
	    nLowerState = ALTERNATE_STATE_UPPER;
	    nStringLen++;
	}
	else
	    nLowerState = ALTERNATE_STATE_NEITHER;
    }

    for (i=0;i<dwMaxLen;i++)
    {
	if (isalpha(lpbyBuffer[i]))
	{
	    if (islower(lpbyBuffer[i]))
	    {
		if (nLowerState == ALTERNATE_STATE_UPPER)
		{
		    nStringLen++;
		}
		nLowerState = ALTERNATE_STATE_LOWER;
	    }
	    else
	    {
		if (nLowerState == ALTERNATE_STATE_LOWER)
		{
		    nStringLen++;
		}
		nLowerState = ALTERNATE_STATE_UPPER;
	    }
	}
	else
	{
	    nLowerState = ALTERNATE_STATE_NEITHER;
	}
    }

    if (nStringLen >= MIN_ALTERNATING_STRING_LEN)
	return(TRUE);

*/
    return(FALSE);
}


BEHAVE_STATUS BMSubmitBuffers
(
    LPBEHAVE_MONITOR            lpBM,
    LPBYTE                      lpbyBuffer,
    DWORD                       dwBufferLen
)
{
    // do string searching for cuss words, etc...

    int                     i, j, nBufferLen = (int)dwBufferLen;
    LPHEUR_STRING_INFO      lpstHSI =
				&lpBM->m_stConfig.stStringInfo;

    for (i=0;i<nBufferLen;i++)
    {

#ifdef SYM_NLM
	if (i % 256 == 0)
	    DRelinquishControl();
#endif

	if (lpstHSI->abyFirstByteHash[lpbyBuffer[i]])
	{
	    if (!memicmp(lpbyBuffer+i,"FUCK",strlen("FUCK")))
		lpBM->m_byBehaviorArray[BF8_STRING_CUSS_WORD] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"VIRUS",strlen("VIRUS")))
		lpBM->m_byBehaviorArray[BF8_STRING_VIRUS] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"COM",strlen("COM")))
		lpBM->m_byBehaviorArray[BF8_STRING_COM] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"EXE",strlen("EXE")))
		lpBM->m_byBehaviorArray[BF8_STRING_EXE] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"*.COM",strlen("*.COM")))
		lpBM->m_byBehaviorArray[BF8_STRING_STAR_COM] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"?.COM",strlen("?.COM")))
		lpBM->m_byBehaviorArray[BF8_STRING_STAR_COM] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"?.EXE",strlen("?.EXE")))
		lpBM->m_byBehaviorArray[BF8_STRING_STAR_EXE] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"*.?OM",strlen("*.?OM")))
		lpBM->m_byBehaviorArray[BF8_STRING_STAR_COM] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"*.?OM",strlen("*.CO?")))
		lpBM->m_byBehaviorArray[BF8_STRING_STAR_COM] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"*.?O?",strlen("*.?O?")))
		lpBM->m_byBehaviorArray[BF8_STRING_STAR_COM] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"*.C*",strlen("*.C*")))
		lpBM->m_byBehaviorArray[BF8_STRING_STAR_COM] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"*.EXE",strlen("*.EXE")))
		lpBM->m_byBehaviorArray[BF8_STRING_STAR_EXE] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"*.?XE",strlen("*.?XE")))
		lpBM->m_byBehaviorArray[BF8_STRING_STAR_EXE] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"*.EX?",strlen("*.EX?")))
		lpBM->m_byBehaviorArray[BF8_STRING_STAR_EXE] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"*.?X?",strlen("*.?X?")))
		lpBM->m_byBehaviorArray[BF8_STRING_STAR_EXE] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"*.E*",strlen("*.E*")))
		lpBM->m_byBehaviorArray[BF8_STRING_STAR_EXE] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"?COM",strlen("?COM")))
		lpBM->m_byBehaviorArray[BF8_STRING_STAR_COM] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"?EXE",strlen("?EXE")))
		lpBM->m_byBehaviorArray[BF8_STRING_STAR_EXE] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"(C)",strlen("(C)")))
		lpBM->m_byBehaviorArray[BF8_STRING_COPYRIGHT] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"BY ",strlen("BY ")))
		lpBM->m_byBehaviorArray[BF8_STRING_BY] = TRUE;

	    if (!memicmp(lpbyBuffer+i,"COMMAND.COM",strlen("COMMAND.COM")))
		lpBM->m_byBehaviorArray[BF8_STRING_COMMAND_COM] = TRUE;

	    for (j=0;j<lpstHSI->wNumStrings;j++)
	    {
		if (!memicmp(lpbyBuffer+i,
			     lpstHSI->astBehaveStrings[j].abyString,
			     lpstHSI->astBehaveStrings[j].wStringLen))
		{
		    lpBM->m_byBehaviorArray[BF8_STRING_SLOT_0 + j] = TRUE;
		    break;
		}
	    }

// CAREY: do we want to check for alternating strings?
/*
	    if (BMMatchAlternatingString(lpBM,
										 lpbyBuffer+i, 
										 dwBufferLen-i))
	    {
		lpBM->m_byBehaviorArray[BF8_STRING_ALTERNATING_UP_LOW] = TRUE;

	    }
*/
	}
    }

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMCheckMOVSAgainstWrites
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wLength
)
{
    int                 i;
    long                lAbs;

    // See if we even want to consider this repmovs...

    if (wLength < MIN_VIRUS_SIZE ||
	wLength > lpBM->m_stConfig.wAssumedVirusSize )
	return(BEHAVE_STATUS_OK);

    // first submit the removs

    if (lpBM->m_nNumREPMOVSSubmissions < MAX_REPMOVS_SUBMISSIONS)
	lpBM->m_wREPMOVSSubmissions[lpBM->m_nNumREPMOVSSubmissions++] = wLength;

    for (i=0;i<lpBM->m_nNumFileWriteSubmissions;i++)
    {
	lAbs = (long)wLength - lpBM->m_wFileWriteSubmissions[i];
	if (lAbs < 0)
	    lAbs = -lAbs;

	// exact +/- 4 bytes in length

	if (lAbs < lpBM->m_stConfig.wMOVSWriteThreshold)
	{
	    lpBM->m_byBehaviorArray[BF6_EXACT_MOVS_WRITE_MATCH] = TRUE;
	    lpBM->m_wVirusSize = wLength;
	    break;
	}

	// close is +/- 10% in length

	if (lAbs < (wLength / lpBM->m_stConfig.wMOVSWriteClosePercent))
	{
	    lpBM->m_byBehaviorArray[BF6_CLOSE_MOVS_WRITE_MATCH] = TRUE;
	    break;
	}
    }

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMCheckWritesAgainstMOVS
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wLength
)
{
    int                 i;
    long                lAbs;

    // See if we even want to consider this file write...

    if (wLength < MIN_VIRUS_SIZE ||
	wLength > lpBM->m_stConfig.wAssumedVirusSize )
	return(BEHAVE_STATUS_OK);

    // first submit the file write

    if (lpBM->m_nNumFileWriteSubmissions < MAX_FILE_WRITE_SUBMISSIONS)
	lpBM->m_wFileWriteSubmissions[lpBM->m_nNumFileWriteSubmissions++] = wLength;

    for (i=0;i<lpBM->m_nNumREPMOVSSubmissions;i++)
    {
	lAbs = (long)wLength - lpBM->m_wREPMOVSSubmissions[i];
	if (lAbs < 0)
	    lAbs = -lAbs;

	// exact +/- 4 bytes in length

	if (lAbs < lpBM->m_stConfig.wMOVSWriteThreshold)
	{
	    lpBM->m_byBehaviorArray[BF6_EXACT_MOVS_WRITE_MATCH] = TRUE;
	    lpBM->m_wVirusSize = wLength;
	    break;
	}

	// close is +/- 10% in length

	if (lAbs < (wLength / lpBM->m_stConfig.wMOVSWriteClosePercent))
	{
	    lpBM->m_byBehaviorArray[BF6_CLOSE_MOVS_WRITE_MATCH] = TRUE;
	    break;
	}
    }

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitCurCSIP
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wCS,
    WORD                        wIP
)
{
    DWORD       dwLinearEP, dwLinearCur, dwEOFDist;

    dwLinearEP = (DWORD)lpBM->m_wEPCS * 0x10 + lpBM->m_wEPIP;
    dwLinearCur = (DWORD)wCS * 0x10 + wIP;

    dwEOFDist = (long)dwLinearEP - (long)dwLinearCur + (long)lpBM->m_dwEPToEOFDist;

    if (lpBM->m_nNumIPSubmissions < MAX_IP_SUBMISSIONS)
	lpBM->m_wIPSubmissions[lpBM->m_nNumIPSubmissions++] = (WORD)dwEOFDist;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMCheckWritesAgainstIP
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wLength
)
{
    int                 i;
    long                lAbs;

    if (wLength < MIN_VIRUS_SIZE || wLength > lpBM->m_stConfig.wAssumedVirusSize )
	return(BEHAVE_STATUS_OK);

    for (i=0;i<lpBM->m_nNumIPSubmissions;i++)
    {
	lAbs = (long)wLength - lpBM->m_wIPSubmissions[i];
	if (lAbs < 0)
	    lAbs = -lAbs;

	// exact +/- 4 bytes in length

	if (lAbs < lpBM->m_stConfig.wIPEOFThreshold)
	{
	    lpBM->m_byBehaviorArray[BF6_EXACT_EP_TO_EOF_WRITE_MATCH] = TRUE;
	    lpBM->m_wVirusSize = wLength;
	    break;
	}

	// close is +/- 10% in length

	if (lAbs < (wLength / lpBM->m_stConfig.wIPEOFWriteClosePercent))
	{
	    lpBM->m_byBehaviorArray[BF6_CLOSE_EP_TO_EOF_WRITE_MATCH] = TRUE;
	    break;
	}
    }

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitFileReadE9
(
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_byBehaviorArray[BF4_FOUND_FILE_READ_E9] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitFileReadMZ
(
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_byBehaviorArray[BF4_FOUND_FILE_READ_MZ] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitFileWriteE9
(
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_byBehaviorArray[BF4_FOUND_FILE_WRITE_E9] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitFileWriteMZ
(
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_byBehaviorArray[BF4_FOUND_FILE_WRITE_MZ] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitMemWriteAfterE9
(
    LPBEHAVE_MONITOR            lpBM,
    BOOL                        bBait
)
{
    lpBM->m_byBehaviorArray[BF4_FOUND_MEM_WRITE_AFTER_E9] = TRUE;

    if (bBait == TRUE)
	lpBM->m_byBehaviorArray[BF4_FOUND_BAIT_WRITE_AFTER_E9] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitMemWriteAfterMZ
(
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_byBehaviorArray[BF4_FOUND_MEM_WRITE_AFTER_MZ] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitPushRet
(
    LPBEHAVE_MONITOR            lpBM,
    BOOL                        bRet100
)
{
    if (bRet100)
	lpBM->m_byBehaviorArray[BF2_PUSH_RET_100] = TRUE;

    lpBM->m_byBehaviorArray[BF2_PUSH_RET] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitFoundHeaderInBody
(
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_byBehaviorArray[BF7_FOUND_HEADER_IN_BODY] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitFindFirstExec
(
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_byBehaviorArray[BF7_FIND_FIRST_EXEC] = TRUE;

    return(BEHAVE_STATUS_OK);
}


BEHAVE_STATUS BMSubmitOriginalEntry
(
    LPBEHAVE_MONITOR            lpBM,
    BOOL                        bReachable
)
{
    if (bReachable == TRUE)
	lpBM->m_dwPersistFlags |= PSF_REACHED_FROM_ORIG_EP;
    else
	lpBM->m_dwPersistFlags &= ~(DWORD)PSF_REACHED_FROM_ORIG_EP;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitFileWriteCS100
(
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_byBehaviorArray[BF1_FILE_WRITE_CS100] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitMOVSCS100
(
    LPBEHAVE_MONITOR            lpBM
)
{
    lpBM->m_byBehaviorArray[BF1_REPMOVS_CS100] = TRUE;

    return(BEHAVE_STATUS_OK);
}

BEHAVE_STATUS BMSubmitRevector0
(
    LPBEHAVE_MONITOR            lpBM
)
{
    if (lpBM->m_dwPersistFlags & PSF_REACHED_FROM_ORIG_EP)
	lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_0] = TRUE;

    return(BEHAVE_STATUS_OK);
}




BEHAVE_STATUS BMPrintBehavior
(
    LPBEHAVE_MONITOR            lpBM,
    int                         nFileNum
)
{

#ifdef PRINT_DEBUG

    if (lpBM->m_byBehaviorArray[IF1_FILE_OPEN_READ_WRITE] == TRUE)
	printf("%d: IF1_FILE_OPEN_READ_WRITE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF1_FILE_OPEN_READ_ONLY] == TRUE)
	printf("%d: IF1_FILE_OPEN_READ_ONLY\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF1_FILE_OPEN_WRITE_ONLY] == TRUE)
	printf("%d: IF1_FILE_OPEN_WRITE_ONLY\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF1_FILE_OPEN_UNKNOWN_MODE] == TRUE)
	printf("%d: IF1_FILE_OPEN_UNKNOWN_MODE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF2_FILE_READ_LESS_8] == TRUE)
	printf("%d: IF2_FILE_READ_LESS_8\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF2_FILE_READ_1C_TO_1E] == TRUE)
	printf("%d: IF2_FILE_READ_1C_TO_1E\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF2_FILE_READ_BAIT] == TRUE)
	printf("%d: IF2_FILE_READ_BAIT\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF2_FILE_READ_128_PLUS] == TRUE)
	printf("%d: IF2_FILE_READ_128_PLUS\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF2_FILE_READ_GENERAL] == TRUE)
	printf("%d: IF2_FILE_READ_GENERAL\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF2_FILE_READ_3_OR_4] == TRUE)
	printf("%d: IF2_FILE_READ_3_OR_4\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF2_FILE_READ_FFFF] == TRUE)
	printf("%d: IF2_FILE_READ_FFFF\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF3_FILE_WRITE_LESS_8] == TRUE)
	printf("%d: IF3_FILE_WRITE_LESS_8\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF3_FILE_WRITE_1C_TO_1E] == TRUE)
	printf("%d: IF3_FILE_WRITE_1C_TO_1E\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF3_FILE_WRITE_128_PLUS] == TRUE)
	printf("%d: IF3_FILE_WRITE_128_PLUS\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF3_FILE_WRITE_GENERAL] == TRUE)
	printf("%d: IF3_FILE_WRITE_GENERAL\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF3_FILE_WRITE_3_OR_4] == TRUE)
	printf("%d: IF3_FILE_WRITE_3_OR_4\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF3_FILE_WRITE_BAIT] == TRUE)
	printf("%d: IF3_FILE_WRITE_BAIT\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF4_FILE_SEEK_TOF] == TRUE)
	printf("%d: IF4_FILE_SEEK_TOF\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF4_FILE_SEEK_EOF] == TRUE)
	printf("%d: IF4_FILE_SEEK_EOF\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF4_FILE_SEEK_GENERAL] == TRUE)
	printf("%d: IF4_FILE_SEEK_GENERAL\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF5_FILE_CLOSE] == TRUE)
	printf("%d: IF5_FILE_CLOSE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF6_FILE_GET_ATTR] == TRUE)
	printf("%d: IF6_FILE_GET_ATTR\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF6_FILE_SET_ATTR] == TRUE)
	printf("%d: IF6_FILE_SET_ATTR\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF7_FILE_GET_FILE_DATE] == TRUE)
	printf("%d: IF7_FILE_GET_FILE_DATE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF7_FILE_SET_FILE_DATE] == TRUE)
	printf("%d: IF7_FILE_SET_FILE_DATE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF8_SYSTEM_GET_TIME] == TRUE)
	printf("%d: IF8_SYSTEM_GET_TIME\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF8_SYSTEM_GET_DATE] == TRUE)
	printf("%d: IF8_SYSTEM_GET_DATE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF9_FILE_FIND_FIRST] == TRUE)
	printf("%d: IF9_FILE_FIND_FIRST\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF9_FILE_FIND_NEXT] == TRUE)
	printf("%d: IF9_FILE_FIND_NEXT\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF10_FILE_DELETE] == TRUE)
	printf("%d: IF10_FILE_DELETE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF10_FILE_CREATE] == TRUE)
	printf("%d: IF10_FILE_CREATE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF10_FILE_RENAME] == TRUE)
	printf("%d: IF10_FILE_RENAME\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF10_FILE_CHDIR] == TRUE)
	printf("%d: IF10_FILE_CHDIR\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF10_FILE_GET_CUR_DIR] == TRUE)
	printf("%d: IF10_FILE_GET_CUR_DIR\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF11_FILE_GET_DTA] == TRUE)
	printf("%d: IF11_FILE_GET_DTA\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF11_FILE_SET_DTA] == TRUE)
	printf("%d: IF11_FILE_SET_DTA\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF12_SYSTEM_GET_VECTOR_21] == TRUE)
	printf("%d: IF12_SYSTEM_GET_VECTOR_21\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_21] == TRUE)
	printf("%d: IF12_SYSTEM_SET_VECTOR_21\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF12_SYSTEM_GET_VECTOR_13] == TRUE)
	printf("%d: IF12_SYSTEM_GET_VECTOR_13\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_13] == TRUE)
	printf("%d: IF12_SYSTEM_SET_VECTOR_13\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF12_SYSTEM_GET_VECTOR_24] == TRUE)
	printf("%d: IF12_SYSTEM_GET_VECTOR_24\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_24] == TRUE)
	printf("%d: IF12_SYSTEM_SET_VECTOR_24\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF12_SYSTEM_GET_VECTOR_LOW] == TRUE)
	printf("%d: IF12_SYSTEM_GET_VECTOR_LOW\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_LOW] == TRUE)
	printf("%d: IF12_SYSTEM_SET_VECTOR_LOW\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF12_SYSTEM_GET_VECTOR_HIGH] == TRUE)
	printf("%d: IF12_SYSTEM_GET_VECTOR_HIGH\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_HIGH] == TRUE)
	printf("%d: IF12_SYSTEM_SET_VECTOR_HIGH\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_DIRECT] == TRUE)
	printf("%d: IF12_SYSTEM_SET_VECTOR_DIRECT\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF12_SYSTEM_SET_VECTOR_0] == TRUE)
	printf("%d: IF12_SYSTEM_SET_VECTOR_0\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF13_SYSTEM_MEM_ALLOC] == TRUE)
	printf("%d: IF13_SYSTEM_MEM_ALLOC\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF13_SYSTEM_PROG_EXEC] == TRUE)
	printf("%d: IF13_SYSTEM_PROG_EXEC (4B)\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF14_SYSTEM_AM_I_THERE] == TRUE)
	printf("%d: IF14_SYSTEM_AM_I_THERE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF15_INT_13_READ_MBR] == TRUE)
	printf("%d: IF15_INT_13_READ_MBR\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF15_INT_13_WRITE_MBR] == TRUE)
	printf("%d: IF15_INT_13_WRITE_MBR\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF15_INT_13_READ_GENERAL] == TRUE)
	printf("%d: IF15_INT_13_READ_GENERAL\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF15_INT_13_WRITE_GENERAL] == TRUE)
	printf("%d: IF15_INT_13_WRITE_GENERAL\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF15_INT_13_FORMAT] == TRUE)
	printf("%d: IF15_INT_13_FORMAT\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF15_INT_13_GENERAL] == TRUE)
	printf("%d: IF15_INT_13_GENERAL\n",nFileNum);

    if (lpBM->m_byBehaviorArray[ IF16_FOUND_PRINT_STRING] == TRUE)
	printf("%d: IF16_FOUND_PRINT_STRING\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF16_FOUND_TERMINATE] == TRUE)
	printf("%d: IF16_FOUND_TERMINATE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF16_FOUND_KEYBOARD_INPUT] == TRUE)
	printf("%d: IF16_FOUND_KEYBOARD_INPUT\n",nFileNum);

    if (lpBM->m_byBehaviorArray[IF16_FOUND_PRINT_CHAR] == TRUE)
	printf("%d: IF16_FOUND_PRINT_CHAR\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF1_WRITE_TO_CS100] == TRUE)
	printf("%d: BF1_WRITE_TO_CS100\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF1_READ_MCB] == TRUE)
	printf("%d: BF1_READ_MCB\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF1_WRITE_MCB] == TRUE)
	printf("%d: BF1_WRITE_MCB\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF1_FILE_WRITE_CS100] == TRUE)
	printf("%d: BF1_FILE_WRITE_CS100\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF1_REPMOVS_CS100] == TRUE)
	printf("%d: BF1_REPMOVS_CS100\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF2_JUMP_TO_CS100] == TRUE)
	printf("%d: BF2_JUMP_TO_CS100\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF2_FAR_JUMP_BEFORE_ENTRY_EXE] == TRUE)
	printf("%d: BF2_FAR_JUMP_BEFORE_ENTRY_EXE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF2_PUSH_RET] == TRUE)
	printf("%d: BF2_PUSH_RET\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF2_PUSH_RET_100] == TRUE)
	printf("%d: BF2_PUSH_RET_100\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_4B] == TRUE)
	printf("%d: BF3_FOUND_CMP_AH_4B\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_3D] == TRUE)
	printf("%d: BF3_FOUND_CMP_AH_3D\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF3_FOUND_CMP_ANY_E9] == TRUE)
	printf("%d: BF3_FOUND_CMP_ANY_E9\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_11] == TRUE)
	printf("%d: BF3_FOUND_CMP_AH_11\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_12] == TRUE)
	printf("%d: BF3_FOUND_CMP_AH_12\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_4E] == TRUE)
	printf("%d: BF3_FOUND_CMP_AH_4E\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AH_4F] == TRUE)
	printf("%d: BF3_FOUND_CMP_AH_4F\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF3_FOUND_CMP_ANY_MZ] == TRUE)
	printf("%d: BF3_FOUND_CMP_ANY_MZ\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF3_FOUND_CMP_AM_I_THERE] == TRUE)
	printf("%d: BF3_FOUND_CMP_AM_I_THERE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF4_FOUND_FILE_READ_E9] == TRUE)
	printf("%d: BF4_FOUND_FILE_READ_E9\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF4_FOUND_FILE_READ_MZ] == TRUE)
	printf("%d: BF4_FOUND_FILE_READ_MZ\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF4_FOUND_FILE_WRITE_E9] == TRUE)
	printf("%d: BF4_FOUND_FILE_WRITE_E9\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF4_FOUND_FILE_WRITE_MZ] == TRUE)
	printf("%d: BF4_FOUND_FILE_WRITE_MZ\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF4_FOUND_MEM_WRITE_AFTER_E9] == TRUE)
	printf("%d: BF4_FOUND_MEM_WRITE_AFTER_E9\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF4_FOUND_MEM_WRITE_AFTER_MZ] == TRUE)
	printf("%d: BF4_FOUND_MEM_WRITE_AFTER_MZ\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF4_FOUND_BAIT_WRITE_AFTER_E9] == TRUE)
	printf("%d: BF4_FOUND_BAIT_WRITE_AFTER_E9\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF5_FOUND_SUSPICIOUS_OPCODES] == TRUE)
	printf("%d: BF5_FOUND_SUSPICIOUS_OPCODES\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF5_FOUND_PUSHF_CALL_FAR] == TRUE)
	printf("%d: BF5_FOUND_PUSHF_CALL_FAR\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF5_FOUND_CALL_POP] == TRUE)
	printf("%d: BF5_FOUND_CALL_POP\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF5_FOUND_REPMOVS] == TRUE)
	printf("%d: BF5_FOUND_REPMOVS\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF5_FOUND_REPMOVS_CODE] == TRUE)
	printf("%d: BF5_FOUND_REPMOVS_CODE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF5_FOUND_E9_AT_TOF] == TRUE)
	printf("%d: BF5_FOUND_E9_AT_TOF\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF5_FOUND_SUSPICIOUS_INT] == TRUE)
	printf("%d: BF5_FOUND_SUSPICIOUS_INT\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF5_FOUND_E80000] == TRUE)
	printf("%d: BF5_FOUND_E80000\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF5_FOUND_IRET] == TRUE)
	printf("%d: BF5_FOUND_IRET\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF5_FOUND_PUSHF_CALL_FAR_21] == TRUE)
	printf("%d: BF5_FOUND_PUSHF_CALL_FAR21\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF5_FOUND_SUSPICIOUS_INT_21] == TRUE)
	printf("%d: BF5_FOUND_SUSPICIOUS_INT_21\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF6_FOUND_SELF_MOD_CODE] == TRUE)
	printf("%d: BF6_FOUND_SELF_MOD_CODE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF6_CLOSE_MOVS_WRITE_MATCH] == TRUE)
	printf("%d: BF6_CLOSE_MOVS_WRITE_MATCH\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF6_EXACT_MOVS_WRITE_MATCH] == TRUE)
	printf("%d: BF6_EXACT_MOVS_WRITE_MATCH\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF6_EXACT_EP_TO_EOF_WRITE_MATCH] == TRUE)
	printf("%d: BF6_EXACT_EP_TO_EOF_WRITE_MATCH\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF6_CLOSE_EP_TO_EOF_WRITE_MATCH] == TRUE)
	printf("%d: BF6_CLOSE_EP_TO_EOF_WRITE_MATCH\n",nFileNum);
 
    if (lpBM->m_byBehaviorArray[BF7_ENTRY_AFTER_FIXUPS] == TRUE)
	printf("%d: BF7_ENTRY_AFTER_FIXUPS\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF7_WINDOWS_EXE] == TRUE)
	printf("%d: BF7_WINDOWS_EXE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF7_STOP_SCANNING] == TRUE)
	printf("%d: BF7_STOP_SCANNING\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF7_COM_FILE] == TRUE)
	printf("%d: BF7_COM_FILE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF7_EXE_FILE] == TRUE)
	printf("%d: BF7_EXE_FILE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF7_FOUND_HEADER_IN_BODY] == TRUE)
	printf("%d: BF7_FOUND_HEADER_IN_BODY\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF7_FIND_FIRST_EXEC] == TRUE)
	printf("%d: BF7_FIND_FIRST_EXEC\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF8_STRING_CUSS_WORD] == TRUE)
	printf("%d: BF8_STRING_CUSS_WORD\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF8_STRING_STAR_COM] == TRUE)
	printf("%d: BF8_STRING_STAR_COM\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF8_STRING_STAR_EXE] == TRUE)
	printf("%d: BF8_STRING_STAR_EXE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF8_STRING_COM] == TRUE)
	printf("%d: BF8_STRING_COM\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF8_STRING_EXE] == TRUE)
	printf("%d: BF8_STRING_EXE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF8_STRING_COPYRIGHT] == TRUE)
	printf("%d: BF8_STRING_COPYRIGHT\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF8_STRING_COMMAND_COM] == TRUE)
	printf("%d: BF8_STRING_COMMAND_COM\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF8_STRING_VIRUS] == TRUE)
	printf("%d: BF8_STRING_VIRUS\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF8_STRING_ALTERNATING_UP_LOW] == TRUE)
	printf("%d: BF8_STRING_ALTERNATING_UP_LOW\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF8_STRING_BY] == TRUE)
	printf("%d: BF8_STRING_BY\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF9_JUMP_AT_TOF_TO_MIDDLE] == TRUE)
	printf("%d: BF9_JUMP_AT_TOF_TO_MIDDLE\n",nFileNum);

    if (lpBM->m_byBehaviorArray[BF9_CALL_WITH_LARGE_DISP] == TRUE)
	printf("%d: BF9_CALL_WITH_LARGE_DISP\n",nFileNum);
 #endif

    return(BEHAVE_STATUS_OK);

}


#if defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_UNIX)


int mymemicmp
(
    LPVOID              lpvA,
    LPVOID              lpvB,
    int                 nLen
)
{
    int                 i, delta;
    char                chA, chB;

    for (i=0;i<nLen;i++)
    {
	chA = ((LPBYTE)lpvA)[i];
	if (chA >= 'a' && chA <= 'z')
	chA &= 0x5F;

	chB = ((LPBYTE)lpvB)[i];
	if (chB >= 'a' && chB <= 'z')
	    chB &= 0x5F;

	delta = chA - chB;
	if (delta)
	    return(delta);
    }

    return(0);
}

#endif
