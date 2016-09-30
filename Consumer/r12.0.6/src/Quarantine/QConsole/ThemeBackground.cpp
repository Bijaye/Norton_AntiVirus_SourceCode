// ThemeBackground.cpp: implementation of the CThemeBackground class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ThemeBackground.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThemeBackground::CThemeBackground() : 
	m_hDll(NULL), 
	m_pfnDrawThemeParentBackground(NULL)
{
	m_hDll = LoadLibrary("uxtheme.dll");
	if(m_hDll)
	{
		m_pfnDrawThemeParentBackground = (pfnDRAWTHEMEPARENTBACKGROUND)GetProcAddress(m_hDll, "DrawThemeParentBackground");
	}
}

CThemeBackground::~CThemeBackground()
{
	if(m_hDll)
		FreeLibrary(m_hDll);
}

bool CThemeBackground::ThemeAvailable()
{
	if(m_pfnDrawThemeParentBackground)
		return true;
	else
		return false;
}

HRESULT CThemeBackground::PaintThemeBackground(HWND hwnd, HDC hdc, RECT *prc)
{
	return m_pfnDrawThemeParentBackground( hwnd, hdc, prc );
}