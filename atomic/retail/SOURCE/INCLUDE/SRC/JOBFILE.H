// Copyright (C) 1997 Symantec Corporation
//****************************************************************************
//
// $Header:   S:/INCLUDE/VCS/jobfile.h_v   1.11   13 Jul 1997 13:01:28   TSmith  $
//
// Description:
//
//****************************************************************************
// $Log:   S:/INCLUDE/VCS/jobfile.h_v  $
// 
//    Rev 1.11   13 Jul 1997 13:01:28   TSmith
// Added inline function 'GetFileName'.
// 
//    Rev 1.10   24 Jun 1997 07:55:40   TSmith
// Changed several member function return types from void to boolean.
// 
//    Rev 1.9   20 Jun 1997 11:30:32   TSmith
// Added declaration for 'Init' member function.
// 
//    Rev 1.8   17 Jun 1997 09:46:40   TSmith
// Removed inline overloaded constructor, moved to seperate implementation.
// 
//    Rev 1.7   16 Jun 1997 06:40:18   TSmith
// Added declaration for CJobFile member function 'UpdateStatus', replaced
// references to STL objects & types with MFC objects & types.
// 
//    Rev 1.6   08 Jun 1997 22:20:10   TSmith
// Added declarations for several overloaded target retrieval functions:
// a couple of 'GetTarget' overloads and the like.
// 
//    Rev 1.5   06 Jun 1997 15:26:10   TSmith
// Added declaration for function 'SetGlobalStatus'.
// 
//    Rev 1.4   06 Jun 1997 13:02:52   TSmith
// Added constructor that accepts job file name as parameter.
// 
//    Rev 1.3   31 May 1997 17:20:20   TSmith
// Added job status string defines (moved here from Descriptor.h)
// 
//    Rev 1.2   31 May 1997 16:11:04   TSmith
// Added declaration for function 'SetProduct'.
// 
//    Rev 1.1   25 May 1997 19:36:36   TSmith
// Added declaration of 'UpdateCurrentTarget' to CJobFile class.
//
//    Rev 1.0   24 May 1997 10:42:14   TSmith
// Initial revision.
//****************************************************************************
#if !defined( __JobFile_h__ )
#define       __JobFile_h__

#include <afx.h>
#include <afxcoll.h>
#include "Descriptor.h"
#include "ProductDescriptor.h"
#include "TargetDescriptor.h"

class CJobFile
    {
private:
    CMapStringToString m_TargetNameToSection;

    CString     m_szFileName;
    LPTSTR      m_szzSectionList;
    LPTSTR      m_psz;
    BOOL        m_bIterating;

public:
    CJobFile( );
    CJobFile( LPCTSTR pszFileName );
    ~CJobFile( );

    BOOL SetFileName( LPCTSTR pszFileName );
    LPCTSTR GetFileName( ) { return m_szFileName; }
    BOOL Init( LPCTSTR pszFileName = NULL );
    BOOL GetNextTarget( CTargetDescriptor& cTarget );
    BOOL UpdateCurrentTarget( CTargetDescriptor& cTarget );
    BOOL UpdateTarget( CTargetDescriptor& td );
    BOOL GetTarget( int nTargetIndex, CTargetDescriptor& td );
    BOOL GetTarget( LPCTSTR pszTargetName, CTargetDescriptor& td );
    void ResetIteration( );
    BOOL GetProduct( CProductDescriptor& cProduct );
    BOOL SetProduct( CProductDescriptor& cProduct );
    int  TargetCount( );
    DWORD UpdateStatus( );

protected:
    BOOL CreateTargetList( );
    BOOL CreateTargetIniMap( );
    };

#endif // __JobFile_h__
