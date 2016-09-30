// ThemeBackground.h: interface for the CThemeBackground class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THEMEBACKGROUND_H__B1946936_04B6_4163_84E9_F2E58A3935AC__INCLUDED_)
#define AFX_THEMEBACKGROUND_H__B1946936_04B6_4163_84E9_F2E58A3935AC__INCLUDED_

#include "uxtheme.h"
#include "tmschema.h"

typedef HRESULT (STDAPICALLTYPE * pfnDRAWTHEMEPARENTBACKGROUND)(HWND hwnd, HDC hdc, OPTIONAL RECT* prc);
//THEMEAPI DrawThemeParentBackground(HWND hwnd, HDC hdc, OPTIONAL RECT* prc);

class CThemeBackground  
{
public:
	CThemeBackground();
	bool ThemeAvailable();
	HRESULT PaintThemeBackground(HWND hwnd, HDC hdc, RECT *prc);
	virtual ~CThemeBackground();

private:
	HMODULE m_hDll;
	pfnDRAWTHEMEPARENTBACKGROUND m_pfnDrawThemeParentBackground;
};

#endif // !defined(AFX_THEMEBACKGROUND_H__B1946936_04B6_4163_84E9_F2E58A3935AC__INCLUDED_)
