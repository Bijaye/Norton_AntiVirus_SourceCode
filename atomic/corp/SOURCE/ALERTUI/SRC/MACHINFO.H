/////////////////////////////////////////////////////////////////////
// MachInfo.h : header file (general machine alert target info class)
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/MACHINFO.H_v   1.1   25 Apr 1997 11:03:52   JBRENNA  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/MACHINFO.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:03:52   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   13 Mar 1997 15:06:24   RFULLER
// Initial revision
// 
//    Rev 1.1   25 Nov 1996 15:45:46   SEDWARD
// No change.
// 
//    Rev 1.0   21 Nov 1996 17:26:10   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#ifndef MACH_INFO_HEADER
#define MACH_INFO_HEADER

// general defines
const   int         TARGET_PATH_MAX_STRLEN  = 1024;


// This email class and list are temporary; the real thing will be handled by
// Ken's global options manager class.
class   CMachineTargetInfo
{
public:
    // constructor
    CMachineTargetInfo(CString  inputStr = NULL);

    // functions
    int     GetBitmapValue(void)  { return  m_bitmap; }
    CString GetTargetPath(void)  { return  m_targetPath; }
    void    SetBitmapValue(int  newValue)  { m_bitmap = newValue; }
    void    SetTargetPath(CString  newPath)  { m_targetPath = newPath; }

    // operator overloads
    void    operator=(CMachineTargetInfo  &sourceRec);

protected:
    // data members
    int         m_bitmap;
    CString     m_targetPath;

private:

};

#endif  // MACH_INFO_HEADER
