// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/HEUR/VCS/HEURINT.CPv   1.7   14 Oct 1998 11:57:46   MKEATIN  $
//
// Description:
//
//  Contains code for the bloodhound interpreter.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/HEUR/VCS/HEURINT.CPv  $
// 
//    Rev 1.7   14 Oct 1998 11:57:46   MKEATIN
// Carey's RelinquishControl calls for NLM.
// 
//    Rev 1.6   27 Jan 1998 17:04:52   DDREW
// Changed the relinquishcontrol values
// 
//    Rev 1.5   13 Oct 1997 13:29:42   CNACHEN
// Relinquish control even more!
// 
//    Rev 1.4   13 Oct 1997 11:14:50   CNACHEN
// Modified relinquish control iterations.
// 
//    Rev 1.3   08 Sep 1997 11:08:16   CNACHEN
// Added DRelinquishControl calls/prototypes
// 
//    Rev 1.2   29 May 1997 13:56:36   CNACHEN
// Fixed some DX problems.
// 
//    Rev 1.1   22 May 1997 15:06:32   CNACHEN
// No change.
// 
//    Rev 1.0   14 May 1997 17:23:06   CNACHEN
// Initial revision.
// 
//************************************************************************

#include "heurapi.h"
#include "heurds.h"
#include "heurtok.h"
#include "heurint.h"

BOOL Operate
(
    LPRPN_STATE             lpstState,
    long					lOpcode
)
{
    long                    lOp1, lOp2, lOp3, lOp4;

    switch (lOpcode)
    {
        case OP_ADD:
            if (lpstState->nStackTop < 2)
                return(FALSE);

            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 += lOp2;
            lpstState->astStack[lpstState->nStackTop++].lValue = lOp1;
            break;

        case OP_SUB:
            if (lpstState->nStackTop < 2)
                return(FALSE);

            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 -= lOp2;
            lpstState->astStack[lpstState->nStackTop++].lValue = lOp1;
            break;

        case OP_NEG:
            if (lpstState->nStackTop < 1)
                return(FALSE);

            lOp1 = lpstState->astStack[lpstState->nStackTop-1].lValue;
            lpstState->astStack[lpstState->nStackTop-1].lValue = -lOp1;
            break;

        case OP_MULT:
            if (lpstState->nStackTop < 2)
                return(FALSE);

            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 *= lOp2;
            lpstState->astStack[lpstState->nStackTop++].lValue = lOp1;
            break;

        case OP_DIVIDE:
            if (lpstState->nStackTop < 2)
                return(FALSE);

            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 /= lOp2;
            lpstState->astStack[lpstState->nStackTop++].lValue = lOp1;
            break;

        case OP_SET:
            if (lpstState->nStackTop < 2)
                return(FALSE);

            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].nVariableIndex;
            if (lOp1 < MAX_RPN_VARIABLES)
                lpstState->adwVariables[lOp1] = lOp2;
            else
                return(FALSE);
            break;

        case OP_EQ:
            if (lpstState->nStackTop < 2)
                return(FALSE);

            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lpstState->astStack[lpstState->nStackTop++].lValue = (lOp1 == lOp2);
            break;

        case OP_GREATER:
            if (lpstState->nStackTop < 2)
                return(FALSE);

            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lpstState->astStack[lpstState->nStackTop++].lValue = (lOp1 > lOp2);
            break;

        case OP_GREATER_EQ:
            if (lpstState->nStackTop < 2)
                return(FALSE);

            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lpstState->astStack[lpstState->nStackTop++].lValue = (lOp1 >= lOp2);
            break;

        case OP_LESS:
            if (lpstState->nStackTop < 2)
                return(FALSE);

            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lpstState->astStack[lpstState->nStackTop++].lValue = (lOp1 < lOp2);
            break;

        case OP_LESS_EQ:
            if (lpstState->nStackTop < 2)
                return(FALSE);

            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lpstState->astStack[lpstState->nStackTop++].lValue = (lOp1 <= lOp2);
            break;

        case OP_LOGICAL_NOT:
            if (lpstState->nStackTop < 1)
                return(FALSE);

            lOp1 = lpstState->astStack[lpstState->nStackTop-1].lValue;
            lpstState->astStack[lpstState->nStackTop-1].lValue = !lOp1;
            break;

        case OP_LOGICAL_XOR:
            if (lpstState->nStackTop < 2)
                return(FALSE);

            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lpstState->astStack[lpstState->nStackTop++].lValue = (!!lOp1 ^ !!lOp2);
            break;

        case OP_LOGICAL_XOR3:
            if (lpstState->nStackTop < 3)
                return(FALSE);

            lOp3 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lpstState->astStack[lpstState->nStackTop++].lValue = (!!lOp1 ^ !!lOp2 ^ !!lOp3);
            break;

        case OP_LOGICAL_XOR4:
            if (lpstState->nStackTop < 4)
                return(FALSE);

            lOp4 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp3 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lpstState->astStack[lpstState->nStackTop++].lValue = (!!lOp1 ^ !!lOp2 ^ !!lOp3 ^ !!lOp4);
            break;

        case OP_LOGICAL_AND:
            if (lpstState->nStackTop < 2)
                return(FALSE);

            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lpstState->astStack[lpstState->nStackTop++].lValue = (lOp1 && lOp2);
            break;

        case OP_LOGICAL_AND3:
            if (lpstState->nStackTop < 3)
                return(FALSE);

            lOp3 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lpstState->astStack[lpstState->nStackTop++].lValue = (lOp1 && lOp2 && lOp3);

            break;

        case OP_LOGICAL_AND4:
            if (lpstState->nStackTop < 4)
                return(FALSE);

            lOp4 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp3 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lpstState->astStack[lpstState->nStackTop++].lValue =
                (lOp1 && lOp2 && lOp3 && lOp4);
            break;

        case OP_LOGICAL_OR:

            if (lpstState->nStackTop < 2)
                return(FALSE);

            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lpstState->astStack[lpstState->nStackTop++].lValue = (lOp1 || lOp2);
            break;

        case OP_LOGICAL_OR3:

            if (lpstState->nStackTop < 3)
                return(FALSE);

            lOp3 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lpstState->astStack[lpstState->nStackTop++].lValue = (lOp1 || lOp2 || lOp3);
            break;

        case OP_LOGICAL_OR4:
            if (lpstState->nStackTop < 4)
                return(FALSE);

            lOp4 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp3 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lpstState->astStack[lpstState->nStackTop++].lValue =
                (lOp1 || lOp2 || lOp3 || lOp4);

            break;

        case OP_TRUE:

            if (lpstState->nStackTop >= MAX_RPN_STACK_SIZE)
                return(FALSE);

            lpstState->astStack[lpstState->nStackTop++].lValue = TRUE;
            break;

        case OP_FALSE:

            if (lpstState->nStackTop >= MAX_RPN_STACK_SIZE)
                return(FALSE);

            lpstState->astStack[lpstState->nStackTop++].lValue = FALSE;
            break;

        case OP_GOTO:
            if (lpstState->nStackTop != 1)
                return(FALSE);

            lpstState->wStreamOff = (WORD)lpstState->astStack[--lpstState->nStackTop].lValue;
            break;

        case OP_REPORT_VIRUS:
            if (lpstState->nStackTop != 1)
                return(FALSE);

            lpstState->bVirusFound = TRUE;
            lpstState->bStop = TRUE;
            lpstState->wVirusID = (WORD)lpstState->astStack[--lpstState->nStackTop].lValue;
            break;

        case OP_IF_IS_FALSE_GOTO:
            if (lpstState->nStackTop != 2)
                return(FALSE);

            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;    // offset
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;    // result
            if (lOp1 == FALSE)
                lpstState->wStreamOff = (WORD)lOp2;
            break;

        case OP_END:
            lpstState->bStop = TRUE;
            break;

        case OP_PRINT:
            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
#ifdef PRINT_DEBUG
            printf("Out: %04X\n",lOp1);
#endif
			break;

        case OP_PLUS_EQUAL:

            if (lpstState->nStackTop < 2)
                return(FALSE);

            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].nVariableIndex;
            if (lOp1 < MAX_RPN_VARIABLES)
                lpstState->adwVariables[lOp1] += lOp2;
            else
                return(FALSE);
            break;

        case OP_MINUS_EQUAL:

            if (lpstState->nStackTop < 2)
                return(FALSE);

            lOp2 = lpstState->astStack[--lpstState->nStackTop].lValue;
            lOp1 = lpstState->astStack[--lpstState->nStackTop].nVariableIndex;
            if (lOp1 < MAX_RPN_VARIABLES)
                lpstState->adwVariables[lOp1] -= lOp2;
            else
                return(FALSE);
            break;

        case OP_FOUND_STRING:
            if (lpstState->nStackTop < 1)
                return(FALSE);

            lOp1 = lpstState->astStack[--lpstState->nStackTop].lValue;
            if (lOp1 >= MAX_USER_DEFINED_STRINGS)
                return(FALSE);

            lpstState->astStack[lpstState->nStackTop++].lValue =
                lpstState->adwVariables[lOp1 + BF8_STRING_SLOT_0];
            break;

        default:
            return(FALSE);
    }

    return(TRUE);
}

BOOL InterpretStream
(
    WORD            wStreamLen,
    LPBYTE          lpbyCode,
    LPBYTE			lpbBehaviorArray,
    LPBOOL          lpbFoundVirus,
    LPWORD          lpwVirusID
)
{
    RPN_STATE_T         stState;

    BYTE                byType;
    DWORD               dwData;
    int                 i;

#ifdef SYM_NLM
    DWORD               dwIter = 0;
#endif

    stState.wStreamOff = 0;
    stState.nStackTop = 0;
    for (i=0;i<MAX_RPN_VARIABLES;i++)
        stState.adwVariables[i] = 0;
    stState.bVirusFound = FALSE;
    stState.wVirusID = 0;
    stState.bStop = FALSE;

    *lpbFoundVirus = FALSE;

    while (stState.wStreamOff < wStreamLen && stState.bStop == FALSE)
    {
#ifdef SYM_NLM

        dwIter++;

        if (dwIter % 16 == 0)
        {
            DRelinquishControl();
        }
#endif

        // get our type and data

        byType = lpbyCode[stState.wStreamOff++];
        switch (byType)
        {
            case CETYPE_OPERATOR:

                dwData = lpbyCode[stState.wStreamOff] +
                         ((WORD)lpbyCode[stState.wStreamOff+1] << 8);
                stState.wStreamOff += sizeof(WORD);

                if (Operate(&stState,dwData) != TRUE)
                    return(FALSE);
                break;

            case CETYPE_VARIABLE:

                if (stState.nStackTop >= MAX_RPN_STACK_SIZE)
                    return(FALSE);

                dwData = lpbyCode[stState.wStreamOff] +
                         ((WORD)lpbyCode[stState.wStreamOff+1] << 8);
                stState.wStreamOff += sizeof(WORD);

                if (dwData < MAX_RPN_VARIABLES)
                {
                    stState.astStack[stState.nStackTop].lValue =
                        stState.adwVariables[dwData];
                    stState.astStack[stState.nStackTop].nVariableIndex =
                        (int)dwData;
                }
                else
                    return(FALSE);

                stState.nStackTop++;

                break;

            case CETYPE_OFFSET:

                if (stState.nStackTop >= MAX_RPN_STACK_SIZE)
                    return(FALSE);

                dwData = lpbyCode[stState.wStreamOff] +
                         ((WORD)lpbyCode[stState.wStreamOff+1] << 8);
                stState.wStreamOff += sizeof(WORD);

                stState.astStack[stState.nStackTop].lValue = dwData;
                stState.nStackTop++;

                break;

            case CETYPE_LITERAL:

                if (stState.nStackTop >= MAX_RPN_STACK_SIZE)
                    return(FALSE);

                dwData = lpbyCode[stState.wStreamOff] +
                         ((WORD)lpbyCode[stState.wStreamOff+1] << 8);
                stState.wStreamOff += sizeof(WORD);

                if (dwData < MAX_LITERAL_INDEX)
                    stState.astStack[stState.nStackTop].lValue =
                        lpbBehaviorArray[dwData];
                else
                    return(FALSE);

                stState.nStackTop++;

                break;

            case CETYPE_DWORD_CONSTANT:

                if (stState.nStackTop >= MAX_RPN_STACK_SIZE)
                    return(FALSE);

                dwData = lpbyCode[stState.wStreamOff] +
                         ((DWORD)lpbyCode[stState.wStreamOff+1] << 8) +
                         ((DWORD)lpbyCode[stState.wStreamOff+2] << 16) +
                         ((DWORD)lpbyCode[stState.wStreamOff+3] << 24);

                stState.wStreamOff += sizeof(DWORD);

                stState.astStack[stState.nStackTop].lValue = dwData;
                stState.nStackTop++;

                break;

            default:
                return(FALSE);
        }
    }

	*lpbFoundVirus = stState.bVirusFound;
	*lpwVirusID = stState.wVirusID;

    return(TRUE);
}

