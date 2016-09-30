/////////////////////////////////////////////////////////////////////
// MachInfo.cpp : implementation file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/MACHINFO.CPv   1.1   25 Apr 1997 11:03:28   JBRENNA  $
//
// Description:
//   This class is used as a "buffer" between the contents of the MFC objects
//   and the master alert target list.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/MACHINFO.CPv  $
// 
//    Rev 1.1   25 Apr 1997 11:03:28   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   13 Mar 1997 15:06:20   RFULLER
// Initial revision
// 
//    Rev 1.2   25 Nov 1996 15:46:36   SEDWARD
// No change.
// 
//    Rev 1.1   21 Nov 1996 17:07:48   SEDWARD
// Added vlog headers.
//
//    Rev 1.0   21 Nov 1996 16:55:06   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "MachInfo.h"


// constructor
CMachineTargetInfo::CMachineTargetInfo(CString  inputStr)
{
    m_targetPath = inputStr;
}



// ===== operator= ========================================================
//
// ========================================================================
//  Function created: 11/96, SEDWARD
// ========================================================================

void    CMachineTargetInfo::operator=(CMachineTargetInfo  &sourceRec)
{
    m_bitmap = sourceRec.m_bitmap;
    m_targetPath = sourceRec.m_targetPath;

}  // end of "CMachineTargetInfo::operator="
