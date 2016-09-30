// AVISProfile.h: interface for the CAVISProfile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISPROFILE_H__8CB09526_E09E_11D2_8A97_00203529AC86__INCLUDED_)
#define AFX_AVISPROFILE_H__8CB09526_E09E_11D2_8A97_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "impexp.h"
#include "cfgprof.h"
#include "MyTraceClient.h"

class IMPEXP CAVISProfile  
{
private:
	static ConfigProfile* s_pCfgProf;

public:
	static BOOL OpenProfile(LPCSTR pszConfigProfileName);
	static void CloseProfile();
	static std::string GetStringValue(LPCSTR pszKey, const std::string& strDefault = "");
	static int GetIntValue(LPCSTR pszKey, int iDefault = -1);
	static BOOL IsValueEnabled(LPCSTR pszKey, BOOL boDefault = FALSE);
};

#endif // !defined(AFX_AVISPROFILE_H__8CB09526_E09E_11D2_8A97_00203529AC86__INCLUDED_)
