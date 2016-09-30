// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1996 - 2003, 2005 Symantec Corporation. All rights reserved.
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/navluprog.h_v   1.0.1.0   22 Jun 1998 19:51:26   RFULLER  $
//
// Description: Header file for the CLuPatchProgress class.
//
// Contains:    CLuPatchProgress class definition
//
// See Also:
//****************************************************************************
// $Log:   S:/NAVLU/VCS/navluprog.h_v  $
// 
//    Rev 1.0.1.0   22 Jun 1998 19:51:26   RFULLER
// Branch base for version QAKN
// 
//    Rev 1.0   20 May 1998 16:45:24   tcashin
// Initial revision.
// 
/////////////////////////////////////////////////////////////////////////////
#ifndef _NAVLUPROG_H_
#define _NAVLUPROG_H_

#include "defutilsinterface.h"
#include "progdlg.h"

//////////////////////////////////////////////////////////////////////////
//
// Description: This class is the callback mechanism used by the ApplyPatch
//              method in the CPatchApp class.
//
// See Also:    pa.h
//
//////////////////////////////////////////////////////////////////////////
class CLuPatchProgress : public IDefUtilsCallbackSink,
                         public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:

    CLuPatchProgress(BOOL bSilent = FALSE);
    ~CLuPatchProgress();

    BOOL Create();
    BOOL DestroyWindow();

    CPatchProgressDlg* m_pCPatchProgressDlg;

    virtual void Sink (double dPercentDone, bool *pbAbort) throw();

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY( IID_DefUtilsCallbackSink, IDefUtilsCallbackSink )
    SYM_INTERFACE_MAP_END()

protected:
    BOOL m_bSilent;

private: // don't implement to prevent use
	CLuPatchProgress( const CLuPatchProgress& );
	CLuPatchProgress& operator = ( const CLuPatchProgress& );
};

#endif // _NAVLUPROG_H_



