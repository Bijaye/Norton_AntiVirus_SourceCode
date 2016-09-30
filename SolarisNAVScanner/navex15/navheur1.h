// Copyright 1996 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVEX/VCS/navheur1.h_v   1.0   04 Nov 1997 15:40:22   CNACHEN  $
//
// Description:
//      This file contains a structure def for engine global data.
//
// Contains:
//
// See Also:
//
//***************************************************************************
// $Log:   S:/NAVEX/VCS/navheur1.h_v  $
// 
//    Rev 1.0   04 Nov 1997 15:40:22   CNACHEN
// Initial revision.
// 
//    Rev 1.2   11 Jun 1997 17:35:22   CNACHEN
// 
//    Rev 1.1   04 Jun 1997 12:04:36   CNACHEN
// 
//***************************************************************************


#ifndef _NAVHEUR_H
#define _NAVHEUR_H

typedef struct tag_EXT_NODE
{
    TCHAR                       szExt[4];
    struct tag_EXT_NODE FAR *   lpstNext;
} EXT_NODE_T, FAR *LPEXT_NODE;

typedef struct
{
    BOOL                bEnabled;
    LPCALLBACKREV1      lpstCallBacks;
    PAMGHANDLE          hGHeur;
    int                 nHeurLevel;
    LPEXT_NODE          lpstExtList;
} NAVEX_HEUR_T, FAR *LPNAVEX_HEUR;

#endif


