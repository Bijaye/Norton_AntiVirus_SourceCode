// MyUtils.h: interface for the MyUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYUTILS_H__3C2E74AE_887E_11D2_B40A_00600831DD76__INCLUDED_)
#define AFX_MYUTILS_H__3C2E74AE_887E_11D2_B40A_00600831DD76__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMyUtils  
{
public:
	int GetRandomNum() const;
	void SeedRand() const;
	CMyUtils();
	virtual ~CMyUtils();

};

#endif // !defined(AFX_MYUTILS_H__3C2E74AE_887E_11D2_B40A_00600831DD76__INCLUDED_)
