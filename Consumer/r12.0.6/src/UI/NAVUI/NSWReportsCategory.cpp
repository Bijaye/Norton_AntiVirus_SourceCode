// NSWReportsCategory.cpp : Implementation of CNSWReportsCategory
#include "stdafx.h"
#include "NAVUI.h"
#include "NSWReportsCategory.h"

/////////////////////////////////////////////////////////////////////////////
// CNSWReportsCategory

CNSWReportsCategory::CNSWReportsCategory ()
{
	// Format our initial URL.
    wsprintf(m_szPanelURL, _T("res://%s\\NAVUIHTM.DLL/%s"), g_NAVInfo.GetNAVDir(), _T("reports.htm"));
    m_uCategoryTitleID = IDS_REPORTS_CATEGORYTITLE;
}