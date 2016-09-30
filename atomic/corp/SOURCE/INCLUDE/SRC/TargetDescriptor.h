// Copyright (C) 1997 Symantec Corporation
//****************************************************************************
// $Header:   S:/INCLUDE/VCS/TargetDescriptor.h_v   1.6   05 Aug 1997 13:09:38   DSACKIN  $
//
// Description:
//
//****************************************************************************
// $Log:   S:/INCLUDE/VCS/TargetDescriptor.h_v  $
// 
//    Rev 1.6   05 Aug 1997 13:09:38   DSACKIN
// Reverted last change.
//
//    Rev 1.5   05 Aug 1997 12:55:52   DSACKIN
//  Added a dwFlags member variable and support functions.
//
//    Rev 1.4   13 Jul 1997 11:30:48   TSmith
// Added member variable 'm_szDirectory' along with appropriate access
// functions. This variable will store the directory to which deployed products
// files will be copied to.
//
//    Rev 1.3   16 Jun 1997 06:50:38   TSmith
// Replaced references to STL ojects & types with MFC objects & types.
//
//    Rev 1.2   29 May 1997 20:25:58   TSmith
// Reworked copy constructor and operator= to call the base class version
// of those classes.
//
//    Rev 1.1   24 May 1997 10:51:52   TSmith
// Added m_szStatus and it's access funcs.
//
//    Rev 1.0   21 May 1997 12:54:58   TSmith
// Initial revision.
//****************************************************************************
#if !defined( __TargetDescriptor_h__ )
#define       __TargetDescriptor_h__

#include <afx.h>
#include "Descriptor.h"

//
// Do not translate the following string defines
//
#define INFVALUE_PRODUCT    _T("Product")
#define INFVALUE_COMMAND    _T("Command")
#define INFVALUE_DIRECTORY  _T("Directory")

class CTargetDescriptor : public CDescriptor
    {
private:
    CString m_szProduct;
    CString m_szCommand;
    CString m_szDirectory;

public:
    CTargetDescriptor( ) { };
    ~CTargetDescriptor( ) { };

    CTargetDescriptor( const CTargetDescriptor& pObject );
    CTargetDescriptor& operator=( const CTargetDescriptor& pRhs );

    void FillFromIniFile( LPCTSTR pszIniFile, LPCTSTR pszSection );
    void WriteToIniFile( LPCTSTR pszIniFile, LPCTSTR pszSection );

    void SetProduct( LPCTSTR pszProduct ) { m_szProduct = pszProduct; }
    LPCTSTR GetProduct( ) { return m_szProduct; }

    void SetCommand( LPCTSTR pszCommand ) { m_szCommand = pszCommand; }
    LPCTSTR GetCommand( ) { return m_szCommand; }

    void SetDirectory( LPCTSTR pszDirectory ) { m_szDirectory = pszDirectory; }
    LPCTSTR GetDirectory( ) { return m_szDirectory; }

    };

#endif // __TargetDescriptor_h__

