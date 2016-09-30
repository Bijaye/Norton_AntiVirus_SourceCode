// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/behav.h_v   1.8   13 Jun 1997 13:34:40   CNACHEN  $
//
// Description:
//      Behavior Monitor defines, data and prototypes.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/behav.h_v  $
// 
//    Rev 1.8   13 Jun 1997 13:34:40   CNACHEN
// Added support for keyboard and vector 0 behaviors
// 
//    Rev 1.7   10 Jun 1997 13:36:00   CNACHEN
// Added two new BM functions for prependers
// 
//    Rev 1.6   06 Jun 1997 15:32:04   CNACHEN
// Added new function to tell whether or not we're in search mode or execute/branch
// mode
// 
//    Rev 1.5   02 Jun 1997 17:19:56   CNACHEN
// Added bait-write check after e9
// 
// 
//    Rev 1.4   29 May 1997 13:30:36   CNACHEN
// Added support for heuristic sensitivity level
// 
//    Rev 1.3   27 May 1997 13:51:42   CNACHEN
// Added support for FILE_FIND_EXEC
// 
//    Rev 1.2   23 May 1997 18:14:44   CNACHEN
// Added tracking of header found in virus body.
// 
//    Rev 1.1   22 May 1997 15:07:10   CNACHEN
// 
//    Rev 1.0   14 May 1997 16:49:36   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef _BEHAV_H_

#define _BEHAV_H_


typedef unsigned int BEHAVE_STATUS;

#define BEHAVE_STATUS_OK            0
#define BEHAVE_STATUS_ERROR         1

// opcode queue

#define OPCODE_QUEUE_SIZE           16

// int stuff

#define NORMAL_INTERRUPT_VECTOR_MAX 0x33
#define MAX_LEGIT_INT_21_SUB_FUNC   0x80

// bait stuff

#define FILE_SIZE_BAIT_VALUE        0xDEAD

// subroutine size at destination of call instructions

#define CALL_SUBROUTINE_SIZE_MAX    256     // max size to search subroutines

// repmovs array size

#define MAX_REPMOVS_SUBMISSIONS     16
#define MAX_FILE_WRITE_SUBMISSIONS  16
#define MAX_IP_SUBMISSIONS          16

// interrupt call types

#define INT_TYPE_INT_21             0
#define INT_TYPE_INT_OTHER          1
#define INT_TYPE_FAR_CALL           2

#define BEHAVE_FILE_TYPE_UNKNOWN    0
#define BEHAVE_FILE_TYPE_COM        1
#define BEHAVE_FILE_TYPE_EXE        2

#define MIN_VIRUS_SIZE              128     // also see MIN_EXE_VIRUS_SIZE
                                            // in heur2.h

#define EXACT_MOVS_WRITE_THRESHOLD      16      // for EXE roundoff
#define EXACT_IP_EOF_THRESHOLD          16
#define FILE_READ_WRITE_BAIT_THRESHOLD  16      // just in case...

// string length for alternating check

#define MAX_ALTERNATING_STRING_LEN  8
#define MIN_ALTERNATING_STRING_LEN  6

#define ALTERNATE_STATE_LOWER                 0
#define ALTERNATE_STATE_UPPER                 1
#define ALTERNATE_STATE_NEITHER               2

#define SUSPICIOUS_OPCODE_THRESHOLD           5
#define SUSPICIOUS_INT_THRESHOLD              3

// instr counts before which we expect to find prints/terminates in clean
// programs

#define DISREGUARD_CLEAN_BEHAVIOR_INSTR_COUNT   32

// flags for interrupt behavior

#include "heurtok.h"

// state flags used for detecting behavior

#define SF1_FOUND_CALL                  0x00000001UL    // call, pop
#define SF1_FOUND_PUSHF                 0x00000002UL    // pushf, call far
#define SF1_FOUND_PUSH                  0x00000004UL

#define PSF_REACHED_FROM_ORIG_EP        0x00000001UL

// int #'s

#define INT_21_PRINT_STRING             0x09
#define INT_21_OPEN_FCB                 0x0F
#define INT_21_CLOSE_FCB                0x10
#define INT_21_FIND_FIRST_FCB           0x11
#define INT_21_FIND_NEXT_FCB            0x12
#define INT_21_DELETE_FCB               0x13
#define INT_21_CREATE_FCB               0x16
#define INT_21_RENAME_FCB               0x17
#define INT_21_SET_DTA                  0x1A
#define INT_21_SET_VECTOR               0x25
#define INT_21_GET_DATE                 0x2A
#define INT_21_GET_TIME                 0x2C
#define INT_21_GET_DTA                  0x2F
#define INT_21_GET_DOS_VERSION          0x30
#define INT_21_GET_VECTOR               0x35
#define INT_21_CHDIR                    0x3B
#define INT_21_CREATE_FILE              0x3C
#define INT_21_OPEN_FILE                0x3D
#define INT_21_CLOSE_FILE               0x3E
#define INT_21_READ_FILE                0x3F
#define INT_21_WRITE_FILE               0x40
#define INT_21_DELETE_FILE              0x41
#define INT_21_SEEK_FILE                0x42
#define INT_21_CHMOD_FILE               0x43
#define INT_21_GET_CUR_DIR              0x47
#define INT_21_MEM_ALLOC                0x48
#define INT_21_EXEC                     0x4B
#define INT_21_TERMINATE                0x4C
#define INT_21_FIND_FIRST               0x4E
#define INT_21_FIND_NEXT                0x4F
#define INT_21_RENAME_FILE              0x56
#define INT_21_TIME_STAMP               0x57
#define INT_21_OPEN_FILE_EXT            0x6C

// int 21 subfuncs

#define INT_21_FILE_OPEN_READ_ONLY      0x00
#define INT_21_FILE_OPEN_WRITE_ONLY     0x01
#define INT_21_FILE_OPEN_READ_WRITE     0x02

#define INT_21_FILE_OPEN_MODE_MASK      0x03

#define INT_21_FILE_SEEK_SET            0
#define INT_21_FILE_SEEK_CUR            1
#define INT_21_FILE_SEEK_END            2

#define INT_21_FILE_CHMOD_GET           0
#define INT_21_FILE_CHMOD_SET           1

#define INT_21_FILE_TIME_GET            0
#define INT_21_FILE_TIME_SET            1

// int 13 subfuncs

#define INT_13_READ_FUNC                0x02
#define INT_13_WRITE_FUNC               0x03
#define INT_13_FORMAT_FUNC              0x05

// registers

#define BEHAVE_REG_AX          0
#define BEHAVE_REG_CX          1
#define BEHAVE_REG_DX          2
#define BEHAVE_REG_BX          3
#define BEHAVE_REG_SP          4
#define BEHAVE_REG_BP          5
#define BEHAVE_REG_SI          6
#define BEHAVE_REG_DI          7

#define BEHAVE_REG_AL          0
#define BEHAVE_REG_CL          1
#define BEHAVE_REG_DL          2
#define BEHAVE_REG_BL          3
#define BEHAVE_REG_AH          4
#define BEHAVE_REG_CH          5
#define BEHAVE_REG_DH          6
#define BEHAVE_REG_BH          7

#define BEHAVE_REG_MEM         8

typedef struct
{
        // interrupts

        BYTE                m_byBehaviorArray[MAX_LITERAL_INDEX];

        // state information

        DWORD               m_dwStateFlags1;
        DWORD               m_dwCallIterNum;
        DWORD               m_dwLinearPUSHFOff;
        WORD                m_wAXFromAmIThere;
        WORD                m_wREPMOVSSubmissions[MAX_REPMOVS_SUBMISSIONS];
        WORD                m_wFileWriteSubmissions[MAX_FILE_WRITE_SUBMISSIONS];
        int                 m_nNumREPMOVSSubmissions;
        int                 m_nNumFileWriteSubmissions;
        WORD                m_wIPSubmissions[MAX_IP_SUBMISSIONS];
        int                 m_nNumIPSubmissions;
        DWORD               m_dwSuspiciousOpcodeCount;
        DWORD               m_dwSuspiciousIntCount;
        DWORD               m_dwSuspiciousInt21Count;
        DWORD               m_dwPersistFlags;

        // misc information

        int                 m_nFileType;
        DWORD               m_dwLastInstrNum;
        BYTE                m_byOpcodeQueue[OPCODE_QUEUE_SIZE];
        int                 m_nOpcodeIndex;
        WORD                m_wEPCS, m_wEPIP;
        DWORD               m_dwEPToEOFDist;
        DWORD               m_dwImageSize;
        WORD                m_wVirusSize;

        // config information

        PAMConfigType       m_stConfig;
} BEHAVE_MONITOR_T, FAR *LPBEHAVE_MONITOR;



////////////////////////////////////////////////////////////////////////////

void BMReset
(
    LPPAMCONFIG                 lpConfig,
    LPBEHAVE_MONITOR            lpBM,
    int                         nHeuristicLevel
);

BOOL BMStopScanning
(
    LPBEHAVE_MONITOR            lpBM
);

BOOL BMScanningWindowsFile
(
    LPBEHAVE_MONITOR            lpBM
);

BOOL BMStopEmulating
(
    LPBEHAVE_MONITOR            lpBM
);


BEHAVE_STATUS BMSubmitFileType
(
    LPBEHAVE_MONITOR            lpBM,
    int                         nFileType
);

BEHAVE_STATUS BMGetFileType
(
    LPBEHAVE_MONITOR            lpBM,
    LPINT                       lpnFileType
);

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
);


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
);

BEHAVE_STATUS BMSubmitDirectGetVector
(
    LPBEHAVE_MONITOR            lpBM,
    BYTE                        byIntNum
);

BEHAVE_STATUS BMSubmitDirectSetVector
(
    LPBEHAVE_MONITOR            lpBM,
    BYTE                        byIntNum
);

BEHAVE_STATUS BMSubmitMCBRead
(
    LPBEHAVE_MONITOR            lpBM
);

BEHAVE_STATUS BMSubmitMCBWrite
(
    LPBEHAVE_MONITOR            lpBM
);

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
);

BEHAVE_STATUS BMSubmitE9AtTOF
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wDisp
);

BEHAVE_STATUS BMSubmitIP100
(
    LPBEHAVE_MONITOR            lpBM,
    DWORD                       dwInstrNum
);

BEHAVE_STATUS BMSubmitWriteTo100
(
    LPBEHAVE_MONITOR            lpBM
);

BEHAVE_STATUS BMSubmitFarJump
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wDestCS,
    WORD                        wDestIP
);


BEHAVE_STATUS BMSubmitBYTECMP     // E9
(
    LPBEHAVE_MONITOR            lpBM,
    int                         nRegister,
    BYTE                        byValue
);

BEHAVE_STATUS BMSubmitWORDCMP     // ZM or ZM
(
    LPBEHAVE_MONITOR            lpBM,
    int                         nRegister,
    WORD                        wValue
);

BEHAVE_STATUS BMSubmitREPMOVS
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wSourceSeg,
    WORD                        wSourceOff,
    WORD                        wLength,
    WORD                        wCurCS,
    WORD                        wCurIP
);

BEHAVE_STATUS BMSubmitEP
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wEPSeg,
    WORD                        wEPOff,
    DWORD                       dwEPToEOFDist
);

BEHAVE_STATUS BMSubmitFixupAfterEP
(
    LPBEHAVE_MONITOR            lpBM
);

BEHAVE_STATUS BMSubmitImageSize
(
    LPBEHAVE_MONITOR            lpBM,
    DWORD                       dwImageSize
);

BEHAVE_STATUS BMSubmitEntryAfterFixups
(
    LPBEHAVE_MONITOR            lpBM
);

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
);

BEHAVE_STATUS BMSubmitOpcode          // IRET, PUSHF, CS: with EXE
(
    LPBEHAVE_MONITOR            lpBM,
    BYTE                        byOpcode,
    WORD                        wCS,
    WORD                        wIP,
    WORD                        wAfterCS,
    WORD                        wAfterIP,
    DWORD                       dwInstrNum
);

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
);

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
);

BEHAVE_STATUS BMSubmitSelfModifyingCode
(
    LPBEHAVE_MONITOR            lpBM
);

BEHAVE_STATUS BMCheckForSuspiciousOpcodes
(
    LPBEHAVE_MONITOR            lpBM,
    BYTE                        byOpcode
);

BEHAVE_STATUS BMSubmitBuffers
(
    LPBEHAVE_MONITOR            lpBM,
    LPBYTE                      lpbyBuffer,
    DWORD                       dwBufferLen
);

BOOL MatchAlternatingString
(
    LPBEHAVE_MONITOR            lpBM,
    LPBYTE                      lpbyBuffer,
    DWORD                       dwMaxLen
);


BEHAVE_STATUS BMResetContext            // reset CALL/POP status and
(                                       //       PUSHF/CALLFAR status
    LPBEHAVE_MONITOR            lpBM
);

BEHAVE_STATUS BMCheckForCallPop
(
    LPBEHAVE_MONITOR            lpBM,
    BYTE                        byOpcode,
    WORD                        wCS,
    WORD                        wIP,
    DWORD                       dwInstrNum
);

BEHAVE_STATUS BMCheckForPUSHF
(
    LPBEHAVE_MONITOR            lpBM,
    BYTE                        byOpcode,
    WORD                        wCS,
    WORD                        wIP,
    DWORD                       dwInstrNum
);

BEHAVE_STATUS BMSubmitPush
(
    LPBEHAVE_MONITOR            lpBM
);

BEHAVE_STATUS BMSubmitPop
(
    LPBEHAVE_MONITOR            lpBM
);

BEHAVE_STATUS BMPrintBehavior
(
    LPBEHAVE_MONITOR            lpBM,
    int                         nFileNum
);

BEHAVE_STATUS BMCheckMOVSAgainstWrites
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wLength
);

BEHAVE_STATUS BMCheckWritesAgainstMOVS
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wLength
);

BEHAVE_STATUS BMCheckWritesAgainstIP
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wLength
);

BEHAVE_STATUS BMSubmitCurCSIP
(
    LPBEHAVE_MONITOR            lpBM,
    WORD                        wCS,
    WORD                        wIP
);

BEHAVE_STATUS BMSubmitFileReadE9
(
    LPBEHAVE_MONITOR            lpBM
);

BEHAVE_STATUS BMSubmitFileReadMZ
(
    LPBEHAVE_MONITOR            lpBM
);

BEHAVE_STATUS BMSubmitFileWriteE9
(
    LPBEHAVE_MONITOR            lpBM
);

BEHAVE_STATUS BMSubmitFileWriteMZ
(
    LPBEHAVE_MONITOR            lpBM
);

BEHAVE_STATUS BMSubmitMemWriteAfterE9
(
    LPBEHAVE_MONITOR            lpBM,
    BOOL                        bBait
);

BEHAVE_STATUS BMSubmitMemWriteAfterMZ
(
    LPBEHAVE_MONITOR            lpBM
);

BEHAVE_STATUS BMSubmitPushRet
(
    LPBEHAVE_MONITOR            lpBM,
    BOOL                        bRet100
);

BEHAVE_STATUS BMSubmitFoundHeaderInBody
(
    LPBEHAVE_MONITOR            lpBM
);

BEHAVE_STATUS BMSubmitFindFirstExec
(
    LPBEHAVE_MONITOR            lpBM
);

BEHAVE_STATUS BMSubmitOriginalEntry
(
    LPBEHAVE_MONITOR            lpBM,
    BOOL                        bReachable
);

BEHAVE_STATUS BMSubmitFileWriteCS100
(
    LPBEHAVE_MONITOR            lpBM
);

BEHAVE_STATUS BMSubmitMOVSCS100
(
    LPBEHAVE_MONITOR            lpBM
);

BEHAVE_STATUS BMSubmitRevector0
(
    LPBEHAVE_MONITOR            lpBM
);

#endif // #ifndef _BEHAV_H_
