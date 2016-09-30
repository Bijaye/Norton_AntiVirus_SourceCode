// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/heur2.h_v   1.1   03 Nov 1997 15:58:26   CNACHEN  $
//
// Description:
//      Heuristics manager defines, data and prototypes.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/heur2.h_v  $
// 
//    Rev 1.1   03 Nov 1997 15:58:26   CNACHEN
// Added FAR to lpstnext ...
// 
//    Rev 1.0   14 May 1997 16:49:42   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef _HEUR2_H_

#define _HEUR2_H_

typedef unsigned int EXEC_STATUS;

#define EXEC_STATUS_OK      0
#define EXEC_STATUS_ERROR   1

#define STACK_SAVE_WORDS        8
#define SEGMENT_MAP_SIZE        8192

#define MAX_ASSUMED_VIRUS_SIZE          8192

/*

// these constants should be encoded in the config data section now...

#define MIN_EXE_VIRUS_SIZE              128					
#define MAX_ENCRYPTED_VIRUS_ITERATIONS  (ASSUMED_VIRUS_SIZE * 6)

                                        // 384 used to be 256
#define MIN_ENCRYPTED_VIRUS_ITERATIONS  384     // enuf to detect decryption
#define MIN_MODIFIED_BYTE_RANGE         24      // 15 instrs per decr byte

#define MAX_ITER_FOR_EP_ADJUST          10
#define MAX_ITER_FOR_RANGE_ADJUST       128
*/

#define EXEC_FILE_TYPE_UNKNOWN       0
#define EXEC_FILE_TYPE_EXE           1
#define EXEC_FILE_TYPE_COM           2

typedef struct tagCPU_CACHE_STATE
{
	primary             preg;
	index_regs          ireg;
	WORD                IP;
	WORD                CS, DS, ES, SS, FS, GS;
	flag_type           FLAGS;
	WORD                wStackValues[STACK_SAVE_WORDS];

    struct tagCPU_CACHE_STATE FAR * lpstNext;
} CPU_CACHE_STATE_T, FAR *LPCPU_CACHE_STATE;

typedef struct
{
        PAMConfigType           m_stConfig;

        WORD                    m_wSeg;
        WORD                    m_dwSegLen;     // includes 0x100 for COM files
        WORD                    m_wLowerOff;
        WORD                    m_wUpperOff;
        DWORD                   m_dwLinearBase;
        DWORD                   m_dwLinearLower;
        DWORD                   m_dwLinearUpper;
        BYTE                    m_byExecSegmentMap[SEGMENT_MAP_SIZE];
        BYTE                    m_byWriteSegmentMap[SEGMENT_MAP_SIZE];
        BYTE                    m_byQueueMap[SEGMENT_MAP_SIZE];
        BYTE                    m_byRevecBits;
        int                     m_nFileType;
        BOOL                    m_bExplore;
        BOOL                    m_bRangeFinalStatus;

        LPCPU_CACHE_STATE       m_lpstHead;
        LPCPU_CACHE_STATE       m_lpstTail;
} EXEC_MANAGER_T, FAR *LPEXEC_MANAGER;

EXEC_STATUS     EMInit
(
    LPPAMCONFIG                     lpConfig,
    LPEXEC_MANAGER                  lpEM
);

EXEC_STATUS     EMReset
(
    LPEXEC_MANAGER                  lpEM
);

EXEC_STATUS     EMSetBaseSeg
(
    LPEXEC_MANAGER                  lpEM,
    WORD                            wSeg,
    WORD                            dwSegLen
);

EXEC_STATUS     EMUpdateRangeByCSIP
(
    LPEXEC_MANAGER                  lpEM,
    WORD                            wSeg,
    WORD                            wCurIP,
    BOOL                            bFinal
);

EXEC_STATUS     EMUpdateRange
(
    LPEXEC_MANAGER                  lpEM,
    WORD                            wSeg,
    WORD                            wStart,
    WORD                            wEnd,
    BOOL                            bFinal
);

EXEC_STATUS     EMGetRange
(
    LPEXEC_MANAGER                  lpEM,
    LPWORD                          pwSeg,
    LPWORD                          pwStartOff,
    LPWORD                          pwEndOff
);

BOOL            EMGetRangeFinalStatus
(
    LPEXEC_MANAGER                  lpEM
);

BOOL            EMInRange
(
    LPEXEC_MANAGER                  lpEM,
    WORD                            wSeg,
    WORD                            wOff
);

void            EMSetExploreMode
(
    LPEXEC_MANAGER                  lpEM,
    BOOL                            bExplore
);

BOOL            EMGetExploreMode
(
    LPEXEC_MANAGER                  lpEM
);

void            EMSetFileType
(
    LPEXEC_MANAGER                  lpEM,
    int                             nType
);


int             EMGetFileType
(
    LPEXEC_MANAGER                  lpEM
);

void            EMSetByteExecStatus
(
    LPEXEC_MANAGER                  lpEM,
    WORD                            wSeg,
    WORD                            wOff,
    BOOL                            bStatus
);

EXEC_STATUS     EMTSByteExecStatus
(
    LPEXEC_MANAGER                  lpEM,
    WORD                            wSeg,
    WORD                            wOff,
    LPBOOL                          lpbExec
);

EXEC_STATUS     EMGetByteExecStatus
(
    LPEXEC_MANAGER                  lpEM,
    WORD                            wSeg,
    WORD                            wOff,
    LPBOOL                          lpbExec
);

void            EMSetByteWriteStatus
(
    LPEXEC_MANAGER                  lpEM,
    WORD                            wSeg,
    WORD                            wOff,
    BOOL                            bStatus
);

EXEC_STATUS     EMTSByteWriteStatus
(
    LPEXEC_MANAGER                  lpEM,
    WORD                            wSeg,
    WORD                            wOff,
    LPBOOL                          lpbWrite
);

EXEC_STATUS     EMGetByteWriteStatus
(
    LPEXEC_MANAGER                  lpEM,
    WORD                            wSeg,
    WORD                            wOff,
    LPBOOL                          lpbWrite
);

void            EMSetByteQueueStatus
(
    LPEXEC_MANAGER                  lpEM,
    WORD                            wSeg,
    WORD                            wOff,
    BOOL                            bStatus
);

EXEC_STATUS     EMGetByteQueueStatus
(
    LPEXEC_MANAGER                  lpEM,
    WORD                            wSeg,
    WORD                            wOff,
    LPBOOL                          lpbWrite
);

void            EMClearWriteMap
(
    LPEXEC_MANAGER                  lpEM
);

EXEC_STATUS     EMGetIPOfNextClearBlock
(
    LPEXEC_MANAGER                  lpEM,
    LPWORD                          lpwCS,
    LPWORD                          lpwIP,
    LPBOOL                          lpbFoundBlock
);

EXEC_STATUS     EMGetMaxWriteRangeSize
(
    LPEXEC_MANAGER                  lpEM,
    LPDWORD                         dwRange
);

EXEC_STATUS     EMDequeue
(
    LPEXEC_MANAGER                  lpEM,
    LPCPU_CACHE_STATE               lpstState,
    LPBOOL                          lpbGotState
);

EXEC_STATUS     EMEnqueue
(
    LPEXEC_MANAGER                  lpEM,
    LPCPU_CACHE_STATE               lpstState
);

EXEC_STATUS     EMRecordInt21Mod
(
    LPEXEC_MANAGER                  lpEM,
    DWORD                           dwOff,
    LPBOOL                          lpbRevec
);

#endif


