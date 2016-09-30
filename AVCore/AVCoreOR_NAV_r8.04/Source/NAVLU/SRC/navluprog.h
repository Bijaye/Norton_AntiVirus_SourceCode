// Copyright 1996-1998 Symantec, Peter Norton Product Group
//****************************************************************************
//
// $Header:   S:/NAVLU/VCS/navluprog.h_v   1.0   20 May 1998 16:45:24   tcashin  $
//
// Description: Header file for the CLuPatchProgress class.
//
// Contains:    CLuPatchProgress class definition
//
// See Also:
//****************************************************************************
// $Log:   S:/NAVLU/VCS/navluprog.h_v  $
// 
//    Rev 1.0   20 May 1998 16:45:24   tcashin
// Initial revision.
// 
/////////////////////////////////////////////////////////////////////////////
#ifndef _NAVLUPROG_H_
#define _NAVLUPROG_H_

#include "pa.h"
#include "progdlg.h"

//////////////////////////////////////////////////////////////////////////
//
// Description: This class is the callback mechanism used by the ApplyPatch
//              method in the CPatchApp class.
//
// See Also:    pa.h
//
//////////////////////////////////////////////////////////////////////////
class CLuPatchProgress : public CPatchApplicationNotify
{
public:

    CLuPatchProgress();
    ~CLuPatchProgress();

    BOOL Create();
    BOOL DestroyWindow();

    CPatchProgressDlg* m_pCPatchProgressDlg;

    // virtual void Sink
    void Sink
    (
        DWORD               dwPercentDone,
        LPBOOL              lpbAbort
    );
};

#endif


