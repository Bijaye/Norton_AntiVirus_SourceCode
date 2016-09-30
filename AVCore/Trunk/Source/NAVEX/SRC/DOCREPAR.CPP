//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/docrepar.cpv   1.5   26 Dec 1996 15:22:26   AOONWAL  $
//
// Description:
//      Contains special include system for DOS compilation.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/docrepar.cpv  $
// 
//    Rev 1.5   26 Dec 1996 15:22:26   AOONWAL
// No change.
// 
//    Rev 1.4   02 Dec 1996 14:02:24   AOONWAL
// No change.
// 
//    Rev 1.3   29 Oct 1996 12:59:10   AOONWAL
// No change.
// 
//    Rev 1.2   28 Aug 1996 16:01:40   DCHI
// Added definition of DOS_DOC_REPAIR.
// 
//    Rev 1.1   05 Aug 1996 10:41:56   DCHI
// Added include of files needed for excel repair.
// 
//    Rev 1.0   03 Jan 1996 17:14:56   DCHI
// Initial revision.
// 
//************************************************************************

//////////////////////////////////////////////////////////////////////
// Define DOS_DOC_REPAIR for plain non-DX DOS macro repair
//////////////////////////////////////////////////////////////////////

#if defined(SYM_DOS) && !defined(SYM_DOSX)
#define DOS_DOC_REPAIR
#endif // #if defined(SYM_DOS) && !defined(SYM_DOSX)

//////////////////////////////////////////////////////////////////////

// Word macro virus repair

#include "macrodel.cpp"
#include "macrorep.cpp"
#include "mcrscndf.cpp"
#include "mcrrepdf.cpp"
#include "olewrite.cpp"
#include "storobj.cpp"
#include "worddoc.cpp"

// Excel macro virus repair

#include "excelshr.cpp"
#include "excelrep.cpp"

