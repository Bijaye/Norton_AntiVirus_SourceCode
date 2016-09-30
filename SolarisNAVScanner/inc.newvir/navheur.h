// Copyright 1996 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/navheur.h_v   1.0   05 Jun 1998 16:26:00   MKEATIN  $
//
// Description:
//      This file contains a structure def for engine global data.
//
// Contains:
//
// See Also:
//
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/navheur.h_v  $
// 
//    Rev 1.0   05 Jun 1998 16:26:00   MKEATIN
// Initial revision.
// 
//    Rev 1.3   26 Jan 1998 16:44:18   DDREW
// Added an element to a structure so that we can avoid a malloc in NAV
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
    TCHAR                   szExt[4];
    struct tag_EXT_NODE *   lpstNext;
} EXT_NODE_T, FAR *LPEXT_NODE;

typedef struct
{
    BOOL                bEnabled;
    LPCALLBACKREV2      lpstCallBacks;
    PAMGHANDLE          hGHeur;
    int                 nHeurLevel;
    LPEXT_NODE          lpstExtList;

#ifdef SYM_NLM
    PAMLHANDLE          hLHeur;     // new for NLM
#endif

} NAVEX_HEUR_T, FAR *LPNAVEX_HEUR;

#endif


