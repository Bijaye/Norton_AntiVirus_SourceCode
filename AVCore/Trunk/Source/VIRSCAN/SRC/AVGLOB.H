//**********************************************************************
// Copyright (C) 1992-1995 Symantec, Peter Norton Group
//**********************************************************************
//
// $Header:   S:/VIRSCAN/VCS/avglob.h_v   1.0   10 Dec 1996 16:33:32   MZAREMB  $
//
// Description: C include file for InitCertlib()
//
//
//**********************************************************************
// $Log:   S:/VIRSCAN/VCS/avglob.h_v  $
// 
//    Rev 1.0   10 Dec 1996 16:33:32   MZAREMB
// Initial revision.
// 
//    Rev 1.0.1.0   10 Oct 1995 14:38:36   DCHI
// Branch base for QAK2.
// 
//    Rev 1.0   10 Oct 1995 14:37:34   DCHI
// Initial revision.
//
//**********************************************************************

#ifndef _AVGLOB_H

#define _AVGLOB_H

#include "cts4.h"

//----------------------------------------------------------------------------
// flags to initialize certlibn
//----------------------------------------------------------------------------
#define INIT_CERTLIB_USE_DEFAULT 0x0001


#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// flags to initialize certlibn defined near the top in this file
//----------------------------------------------------------------------------
//  INIT_CERTLIB_USE_DEFAULT  0x0001
//      Use this flag if you do NOT wish to provide your own functions.
//

BOOL WINAPI InitCertlib (    // To initialize CERTLIB library
    LPCALLBACKREV1  lpCallback,         // [in] pointer to callback struct
    WORD            wFlags              // [in] Sami knows what flags.
    );

#ifdef __cplusplus
}
#endif

#endif

