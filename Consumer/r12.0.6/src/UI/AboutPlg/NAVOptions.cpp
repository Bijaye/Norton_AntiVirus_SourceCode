// NAVOptions.cpp: implementation of the CNAVOptions class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"


#include "StahlSoft.h"
//#define _INIT_COSVERSIONINFO
#include "SSOsinfo.h"

#include "NAVOptions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNAVOptions::CNAVOptions()
{
    m_pNavOpts = new CNAVOptSettingsEx(m_ccSettings);

	if (!m_pNavOpts->Init())
        throw false;  
}

CNAVOptions::~CNAVOptions()
{
	if (m_pNavOpts != NULL)
	    delete m_pNavOpts;
}

void CNAVOptions::GetString(LPCTSTR lpszOptName, LPTSTR lpszBuffer, DWORD dwSize, LPTSTR lpszDefault)
{
    if (FAILED(m_pNavOpts->GetValue(lpszOptName, lpszBuffer, dwSize, lpszDefault)))
        throw false;
}
