// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/heurint.h_v   1.0   14 May 1997 13:28:56   CNACHEN  $
//
// Description:
//      Heuristic interpreter data, defines, prototypes
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/heurint.h_v  $
// 
//    Rev 1.0   14 May 1997 13:28:56   CNACHEN
// Initial revision.
// 
//************************************************************************

#ifndef _HEURINT_H

#define _HEURINT_H

#define MAX_RPN_STACK_SIZE  32
#define MAX_RPN_VARIABLES   256

typedef struct
{
    long                lValue;
    int                 nVariableIndex;
} STACK_ELEMENT_T,  *LPSTACK_ELEMENT;

typedef struct
{
    STACK_ELEMENT_T     astStack[MAX_RPN_STACK_SIZE];
    int                 nStackTop;
    DWORD               adwVariables[MAX_RPN_VARIABLES];
    WORD                wStreamOff;
    WORD                wVirusID;
    BOOL                bVirusFound;
    BOOL                bStop;
} RPN_STATE_T, * LPRPN_STATE;

BOOL InterpretStream
(
    WORD            wStreamLen,
    LPBYTE          lpbyCode,
    LPBYTE			lpbBehaviorArray,
    LPBOOL          lpbFoundVirus,
    LPWORD          lpwVirusID
);

#endif
