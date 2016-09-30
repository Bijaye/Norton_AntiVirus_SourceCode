////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// OEH (Outbound Email Heuristic) UI Interface
//

#ifndef OEH_UI_INTERFACE_H
#define OEH_UI_INTERFACE_H

#include "SymInterface.h"

enum OEHACTION
{
	OEHACTION_QUARANTINE,
	OEHACTION_ALLOW,
	OEHACTION_BLOCK,
	OEHACTION_AUTHORIZE,
	OEHACTION_LAUNCH_QUARANTINE,
	OEHACTION_NONE
};

enum OEHUI_TYPE
{
	OEHUI_TYPE_INITIAL,
	OEHUI_TYPE_QUARANTINE_FAILED,
	OEHUI_TYPE_QUARANTINE_SUCCEEDED,
};

class IOEHeurUIW : public ISymBase
{
public:
	// Set the process name to be display.
	virtual void SetProcessName(const wchar_t* szProcessName) = 0;

	// Set the email information (sender, recipient, and subject).
	virtual void SetEmailInfo(const wchar_t* szSubject, const wchar_t* szSender, 
		const wchar_t* szRecipient) = 0;

	// Display the OEH dialog.
	virtual bool Show(OEHUI_TYPE UIType, OEHACTION* pAction) = 0;

	// Delete the OEH UI object
	virtual void DeleteObject() = 0;
};

// {5B34FD09-BC50-40a3-AD3F-9BA4F190EB04}
SYM_DEFINE_INTERFACE_ID(IID_IOEHeurUIW, 
0x5b34fd09, 0xbc50, 0x40a3, 0xad, 0x3f, 0x9b, 0xa4, 0xf1, 0x90, 0xeb, 0x4);

extern "C"
{
	bool WINAPI CreateOEHeurUI(IOEHeurUIW** ppOEHeurUI);
	typedef bool (WINAPI *pfnCREATEOEHEURUI)(IOEHeurUIW**);
};

#endif

