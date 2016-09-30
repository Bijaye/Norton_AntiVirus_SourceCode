// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "ScanCouldntShowNotify.h"


// These are the dimensions for the alert window.
// They should leave enough room for internationalization.
#define ALERT_WIDTH   275
#define ALERT_HEIGHT  180


CScanCouldntShowNotify::CScanCouldntShowNotify()
{
}



BOOL CScanCouldntShowNotify::Start()
{
	// Set the alert text.
	try
	{
		m_sNotifyText.LoadString(IDS_SCANFOUNDRISKS);
	}
	VP_CATCH_MEMORYEXCEPTIONS
	(
		CCTRACEE(_T("%s : Caught memory exception.\n"), __FUNCTION__);
	)
	// Start!
	return CAVNotify::Start();
}


int CScanCouldntShowNotify::Run()
{
	// Turn on the ok button.
	CString sButtonOk;

	m_pNotify->SetProperty(cc::INotify::PROPERTIES::PROPERTY_OK_BUTTON, true);
	sButtonOk.LoadString(IDS_OK);
	m_pNotify->SetButton(cc::INotify::BUTTONTYPE_OK, sButtonOk);
	// Make the alert larger.
	m_pNotify->SetSize(ALERT_WIDTH, ALERT_HEIGHT);
	// Display the alert.
	int iRet = CAVNotify::Run();
	// Delete myself!
	delete this;

	return iRet;
}
