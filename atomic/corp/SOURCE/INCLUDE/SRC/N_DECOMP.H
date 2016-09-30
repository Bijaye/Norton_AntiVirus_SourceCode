// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/N_DECOMP.H_v   1.2   15 Jun 1998 17:15:22   DBuches  $
//
// Description:  Header for n_decomp.cpp
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/N_DECOMP.H_v  $
// 
//    Rev 1.2   15 Jun 1998 17:15:22   DBuches
// Added override of OnBadChild().
// 
//    Rev 1.1   04 May 1998 20:07:42   DBuches
// Fixed problems with loading CAB decomposer.  Also added override of OnBusy().
//
//    Rev 1.0   16 Mar 1998 19:02:16   MKEATIN
// Initial revision.
//************************************************************************
#ifndef N_DECOMP_H
#define N_DECOMP_H

#include "dec2.h"

// our "sink" class

class CNAVEventSink : public CDecEventSink
{
  public:

    CNAVEventSink
    (
        LPNONAV lpNav
    )
    {
        m_lpNav = lpNav;
    }

  private:

    HRESULT OnNew
    (
        IDecObject* pObject,
        WORD* pwResult,
        LPTSTR pszNewDataFile,
        BOOL* pbContinue
    );

    HRESULT OnChildFound
    (
        IDecContainerObject* pObject,
        LPCTSTR pszChildName,
        BOOL* pbContinue
    );
    
	HRESULT OnBadChild(
		IDecContainerObject* pObject, 
		HRESULT hr,
		LPCTSTR pszChildName,
		BOOL* pbContinue);

    HRESULT OnNotIdentified
    (
        IDecObject* pObject,
        WORD* pwResult,
        LPTSTR pszNewDataFile
    );

    HRESULT OnBusy();

    LPNONAV m_lpNav;
};

// function prototypes

BOOL SYM_EXPORT NAVDecomposerInit(void);
void SYM_EXPORT NAVDecomposerEnd(void);

#endif
