
// NSWAdvancedCategory.cpp : Implementation of CNSWAdvancedCategory

#include "stdafx.h"
#include "NAVUI.h"
#include "NSWAdvancedCategory.h"

/////////////////////////////////////////////////////////////////////////////
// CNSWAdvancedCategory

CNSWAdvancedCategory::CNSWAdvancedCategory ()
{
	// Format our initial URL.
    wsprintf(m_szPanelURL, _T("res://%s\\NAVUIHTM.DLL/%s"), g_NAVInfo.GetNAVDir(), _T("advanced.htm"));
    m_uCategoryTitleID = IDS_ADVANCED_CATEGORYTITLE;
}
