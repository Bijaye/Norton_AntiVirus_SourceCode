// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1996 - 2003, 2005 Symantec Corporation. All rights reserved.
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/navluprog.cpv   1.0.1.0   22 Jun 1998 19:51:26   RFULLER  $
//
// Description: CLuPatchProgress implementation
//
//              This class, derived from CPatchApplicationNotify, is used as
//              callback mechanism by the ApplyPatch method in CPatchApp.
//
//              These member functions are wrappers over corresponding
//              member functions in CPatchProgressDlg, the real dialog class.
//
// Contains:    CLuPatchProgress class definition
//
// See Also:    NavLuProg.h
//
//****************************************************************************
// $Log:   S:/NAVLU/VCS/navluprog.cpv  $
// 
//    Rev 1.0.1.0   22 Jun 1998 19:51:26   RFULLER
// Branch base for version QAKN
// 
//    Rev 1.0   20 May 1998 16:45:20   tcashin
// Initial revision.
// 
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "navluprog.h"

//////////////////////////////////////////////////////////////////////////
//
// Function:    CLuPatchProgress()
//
// Description: Constructor
//
// Parameters:  None
//
// Returns:     None
//
//////////////////////////////////////////////////////////////////////////
CLuPatchProgress::CLuPatchProgress(BOOL bSilent)
{
	m_bSilent = bSilent;

	if ( m_bSilent )
		m_pCPatchProgressDlg = NULL;
	else
    {
        try
        {
		    m_pCPatchProgressDlg = new CPatchProgressDlg;
        }
        catch (...)
        {
        }
    }
};


//////////////////////////////////////////////////////////////////////////
//
// Function:    ~CLuPatchProgress()
//
// Description: Destructor
//
// Parameters:  None
//
// Returns:     None
//
//////////////////////////////////////////////////////////////////////////
CLuPatchProgress::~CLuPatchProgress()
{
	if ( m_pCPatchProgressDlg )
        delete m_pCPatchProgressDlg;
};


//////////////////////////////////////////////////////////////////////////
//
// Function:    Create()
//
// Description: Calls the Create function of the CPatchProgressDlg class.
//
// Parameters:  None
//
// Returns:     
//
//////////////////////////////////////////////////////////////////////////
BOOL CLuPatchProgress::Create()
{
	BOOL bReturn = TRUE;

	if ( m_pCPatchProgressDlg )
        bReturn = m_pCPatchProgressDlg->Create();

	return bReturn;
};


//////////////////////////////////////////////////////////////////////////
//
// Function:    DestroyWindow()
//
// Description: Calls the DestroyWindow() function of the CPatchProgressDlg 
//              class.
//
// Parameters:  None
//
// Returns:     
//
//////////////////////////////////////////////////////////////////////////
BOOL CLuPatchProgress::DestroyWindow()
{
	BOOL bReturn = TRUE;

	if ( m_pCPatchProgressDlg )
        bReturn = m_pCPatchProgressDlg->DestroyWindow();

	return bReturn;
};


//////////////////////////////////////////////////////////////////////////
//
// Function:    Sink()
//
// Description: Callback from ApplyPatch to increment the progress bar.
//
// Parameters:  dwPercentDone - Percent complete
//              lpbAbort      - Set to TRUE to stop the patching process
//
// Returns:     None
//
//////////////////////////////////////////////////////////////////////////
void CLuPatchProgress::Sink (double dPercentDone, bool *pbAbort) throw()
{
	if ( m_pCPatchProgressDlg )
	{
        m_pCPatchProgressDlg->SetPos((int)dPercentDone);
        *pbAbort = false;
	}
};
