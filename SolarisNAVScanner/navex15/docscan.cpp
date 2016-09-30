//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/docscan.cpv   1.6   26 Dec 1996 15:22:14   AOONWAL  $
//
// Description:
//      Contains special include system for DOS compilation.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/docscan.cpv  $
// 
//    Rev 1.6   26 Dec 1996 15:22:14   AOONWAL
// No change.
// 
//    Rev 1.5   02 Dec 1996 14:02:00   AOONWAL
// No change.
// 
//    Rev 1.4   29 Oct 1996 12:58:46   AOONWAL
// No change.
// 
//    Rev 1.3   28 Aug 1996 16:02:34   DCHI
// Added definition of DOS_DOC_SCAN.
// 
//    Rev 1.2   05 Aug 1996 10:40:58   DCHI
// Added include of excelshr.h.
// 
//    Rev 1.1   25 Jul 1996 18:12:18   DCHI
// Added include of "exclscan.cpp".
// 
//    Rev 1.0   03 Jan 1996 17:14:58   DCHI
// Initial revision.
// 
//************************************************************************

//////////////////////////////////////////////////////////////////////
// Define DOS_DOC_SCAN for plain non-DX DOS macro scanning
//////////////////////////////////////////////////////////////////////

#if defined(SYM_DOS) && !defined(SYM_DOSX)
#define DOS_DOC_SCAN
#endif // #if defined(SYM_DOS) && !defined(SYM_DOSX)

//////////////////////////////////////////////////////////////////////

// Word macro virus scanning

#include "macrscan.cpp"
#include "mcrscndf.cpp"
#include "storobj.cpp"
#include "worddoc.cpp"

// Excel macro virus scanning

#include "excelshr.cpp"
#include "exclscan.cpp"

