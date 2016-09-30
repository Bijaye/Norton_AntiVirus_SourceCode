// AVScanner.h: interface for the CAVScanner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVSCANNER_H__0E68FB20_D72A_11D2_ADDE_00104BF3F36A__INCLUDED_)
#define AFX_AVSCANNER_H__0E68FB20_D72A_11D2_ADDE_00104BF3F36A__INCLUDED_

#include "navapi.h"
#include "defutils.h"
#include "navapicallbacks.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAVScanner  
{
public:
    CAVScanner();
    virtual ~CAVScanner();

    HRESULT Initialize();
    HRESULT ScanFile( LPCTSTR pszFileName, LPVOID* pVirus );
    HRESULT DeleteFile( LPCTSTR pszFileName );
    HRESULT RepairFile( LPCTSTR pszFileName );

    HRESULT GetVirusName( LPVOID pVirus, LPTSTR szVirusName, int nBufferLen );
    HRESULT GetVirusID( LPVOID pVirus, DWORD& dwVirusID );
    HRESULT FreeVirus( LPVOID pVirus );

    HRESULT LoadVirusDef( DWORD dwVirusID, LPVOID* pVirusDef );
    DWORD GetVirusDefInfo( LPVOID pVirusDef, 
                             NAVVIRINFO enumVirInfo,
                             LPSTR       lpszResultBuffer,
                             LPDWORD      lpdwBufferSize );
    HRESULT FreeVirusDef( LPVOID pVirusDef );
    LPCTSTR GetVirusDefsDir() { return m_sVirusDefs; }

    HRESULT GetVirusDefsDate( SYSTEMTIME* pst );

    operator HNAVENGINE() {return m_hEngine; }

private:
    // 
    // Handle to NAVAPI32
    // 
    static HNAVENGINE  m_hEngine;

    // 
    // Hawkings object.
    // 
    static CDefUtils   defUtils;

    // 
    // Virus defs path.
    // 
    static CString       m_sVirusDefs;


    // 
    // Memory based callbacks
    // 
    CNavAPIMemCallbacks m_MemCallbacks;


    // 
    // Memory scan flag.
    // 
    BOOL        m_bMemoryScan;

 
};

#endif // !defined(AFX_AVSCANNER_H__0E68FB20_D72A_11D2_ADDE_00104BF3F36A__INCLUDED_)
