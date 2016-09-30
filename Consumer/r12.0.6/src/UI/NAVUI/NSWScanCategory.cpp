// NSWScanCategory.cpp : Implementation of CNSWScanCategory
#include "stdafx.h"
#include "NAVUI.h"
#include "NSWScanCategory.h"

/////////////////////////////////////////////////////////////////////////////
// CNSWScanCategory
CNSWScanCategory::CNSWScanCategory ()
{
	// Format our initial URL.
    wsprintf(m_szPanelURL,_T("res://%s\\NAVUIHTM.DLL/%s"), g_NAVInfo.GetNAVDir(), _T("scan.htm"));
    m_uCategoryTitleID = IDS_SCAN_CATEGORYTITLE;
}	