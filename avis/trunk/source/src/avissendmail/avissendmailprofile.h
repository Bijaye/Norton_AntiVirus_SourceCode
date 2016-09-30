// AVISSendMailProfile.h: interface for the CAVISSendMailProfile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISSENDMAILPROFILE1_H__A61450F8_F65D_11D2_913F_0004ACEC70EC__INCLUDED_)
#define AFX_AVISSENDMAILPROFILE1_H__A61450F8_F65D_11D2_913F_0004ACEC70EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning (disable:4786 )

#include "AVISSendMailDLL.h"

#include "cfgprof.h"

#include "utilexception.h"

//////////////////////////////////////////////////////////////////////////////
//
// Class Name: CAVISSendMailProfile 
//
// Author: C. Falterer.
//
// Purpose: 
//			.
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
class AVISSENDMAIL_API CAVISSendMailProfile  
{

public:

	static BOOL OpenProfile( LPCSTR pszProfileFileName );

	static void CloseProfile( );

	static std::string GetStringValue( LPCSTR pszKey, const std::string& strDefault = "" );

	static int GetIntValue( LPCSTR pszKey, int iDefault = -1 );
	
	// constructor/destructors
	CAVISSendMailProfile( );

	virtual ~CAVISSendMailProfile( );

private:

	static ConfigProfile* s_pCfgProfile;

};

#endif // !defined(AFX_AVISSENDMAILPROFILE1_H__A61450F8_F65D_11D2_913F_0004ACEC70EC__INCLUDED_)
