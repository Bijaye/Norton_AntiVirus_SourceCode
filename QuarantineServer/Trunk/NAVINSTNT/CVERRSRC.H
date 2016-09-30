///////////////////////////////////////////////////////////////////////////
// Copyright (c)1997 - Bruce Hellstrom All Rights Reserved
///////////////////////////////////////////////////////////////////////////
//
//  $Header: CVerrsrc.h  Revision:1.2  Mon Feb 01 19:02:52 1999  BRUCEH $
//
//  CVERRSRC.H    - This file contains class definitions for the
//                  CVersionResource Class Object.
//
//
//
///////////////////////////////////////////////////////////////////////////

#ifndef _CVERRSRC_H_
#define _CVERRSRC_H_


#define CVER_STR_SIZE               128
#define CVER_MAX_PATH               265


#define CVER_NODATA_IN_FILE         0xFFFF
#define CVER_NO_FIXED_INFO          0xFFFE
#define CVER_NOT_INITIALIZED        0xFFFD
#define CVER_LANG_NOT_AVAIL         0xFFFC

#define CVER_COMPARE_EQUAL          0x00000000
#define CVER_COMPARE_NEWER          0x00000001
#define CVER_COMPARE_OLDER          0x00000002
#define CVER_COMPARE_NORESRC        0x00000004

#define CVER_SINFO_COMPNAME         1
#define CVER_SINFO_FILEDESCRIP      2
#define CVER_SINFO_FILEVER          3
#define CVER_SINFO_INTERNALNAME     4
#define CVER_SINFO_COPYRIGHT        5
#define CVER_SINFO_ORIGFNAME        6
#define CVER_SINFO_PRODNAME         7
#define CVER_SINFO_PRODVER          8


///////////////////////////////////////////////////////////////////////////
//  CVersionResource class definition
///////////////////////////////////////////////////////////////////////////

class CVersionResource
{
                                        // Construction/Destruction
public:
    CVersionResource( LPTSTR lpFileName );
    ~CVersionResource();

                                        // Implementation

public:
    BOOL IsInitialized( void ) const
        { return( m_bInitialized ); }

    DWORD InitializeData( void );
    DWORD GetLanguageList( LPDWORD *lplpdwLangList, LPDWORD lpdwElements );
    DWORD SetCurrentLanguage( DWORD dwLang = 0 );
    DWORD GetBinaryFileVer( LPDWORD lpdwFileVerMS, LPDWORD lpdwFileVerLS ) const;
    DWORD GetBinaryProductVer( LPDWORD lpdwProdVerMS, LPDWORD lpdwProdVerLS ) const;

    const VS_FIXEDFILEINFO *GetAllBinaryInfo( void )
        { return( (const VS_FIXEDFILEINFO*)m_pvsff ); }

    DWORD CompareFileVer( DWORD dwCompareMS, DWORD dwCompareLS );
    DWORD CompareFileVer( const CVersionResource *cver );
    LPCTSTR GetStringInfo( DWORD dwItem );

    BOOL operator==( const CVersionResource *cver );

                                        // Protected member functions

protected:

    DWORD VersionCompare( DWORD dwCurrentMS, DWORD dwCurrentLS,
                          DWORD dwNewMS, DWORD dwNewLS );

    BOOL FileExists( LPTSTR lpFile );


                                        // Data members
protected:
    BOOL                m_bInitialized;
    UINT                m_uFixedSize;
    UINT                m_uLangSize;
    DWORD               m_dwCurLang;
    LPTSTR              m_lpFileName;
    LPBYTE              m_lpBuff;
    LPDWORD             m_lpLangs;
    VS_FIXEDFILEINFO   *m_pvsff;

};











#endif                                  // ifndef _this_file_



///////////////////////////////////////////////////////////////////////////
//  $Log: G:\QVCS\Arch\LOGS\BHUtils32\CVerrsrc.i $
//  
//    
//  
//  Revision 1.2  by: BRUCEH  Rev date: Mon Feb 01 19:02:52 1999
//    Remove error checking on GetFileVersionInfo because of a bug in Windows
//    that makes it fail on VXDs when it really worked.
//  
//  Revision 1.1  by: BRUCEH  Rev date: Mon Aug 25 03:48:22 1997
//    Added CCommandLine Object
//  
//  Revision 1.0  by: BRUCEH  Rev date: Sun Jun 29 01:03:20 1997
//    Initial revision.
//  
//  $Endlog$
//
//
///////////////////////////////////////////////////////////////////////////

