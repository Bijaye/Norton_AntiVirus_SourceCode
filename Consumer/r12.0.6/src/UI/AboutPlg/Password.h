// Password.h: interface for the CPassword class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PASSWORD_H__68DF1F4B_D66B_4616_B660_32F329B274AC__INCLUDED_)
#define AFX_PASSWORD_H__68DF1F4B_D66B_4616_B660_32F329B274AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NAVPwdExp.h"
#include "NAVPwd_h.h"
#include "resource.h"

class CPassword  
{
public:
	bool IsPasswordCheckEnabled();
	bool ResetPassword();
    bool CheckPassword();

	CPassword();
	virtual ~CPassword();

private:
    CComPtr<IPassword> m_spNAVPass;

};

#endif // !defined(AFX_PASSWORD_H__68DF1F4B_D66B_4616_B660_32F329B274AC__INCLUDED_)
