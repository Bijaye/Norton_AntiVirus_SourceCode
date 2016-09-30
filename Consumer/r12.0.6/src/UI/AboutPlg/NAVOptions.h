// NAVOptions.h: interface for the CNAVOptions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVOPTIONS_H__0E25441D_CEFE_4790_9F22_12CE6CC71926__INCLUDED_)
#define AFX_NAVOPTIONS_H__0E25441D_CEFE_4790_9F22_12CE6CC71926__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NAVSettingsHelperEx.h"

#include "NAVInfo.h"

class CNAVOptions  
{
public:
	CNAVOptions();          // throws false
	virtual ~CNAVOptions();

    void GetString ( LPCTSTR lpszOptName,
                     LPTSTR lpszBuffer, 
                     DWORD dwSize, 
                     LPTSTR lpszDefault ); // throws false

protected:
    CNAVOptSettingsEx* m_pNavOpts;
	NAVToolbox::CCSettings m_ccSettings;
};

#endif // !defined(AFX_NAVOPTIONS_H__0E25441D_CEFE_4790_9F22_12CE6CC71926__INCLUDED_)
