// Copyright 1996-1998 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLUCBK/VCS/navluprog.cpv   1.0   08 Jan 1999 19:23:36   CEATON  $
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
// $Log:   S:/NAVLUCBK/VCS/navluprog.cpv  $
// 
//    Rev 1.0   08 Jan 1999 19:23:36   CEATON
// Initial revision.
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
CLuPatchProgress::CLuPatchProgress()
{
        m_pCPatchProgressDlg = new CPatchProgressDlg;
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
        return m_pCPatchProgressDlg->Create();
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
        return m_pCPatchProgressDlg->DestroyWindow();
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
void CLuPatchProgress::Sink
    (
        DWORD               dwPercentDone,
        LPBOOL              lpbAbort
    )

{
        m_pCPatchProgressDlg->StepIt();
       *lpbAbort = FALSE;
};

