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

class IOEHeurUI : public ISymBase
{
public:
	// Set the process name to be display.
	virtual void SetProcessName(const char* szProcessName) = 0;

	// Set the email information (sender, recipient, and subject).
	virtual void SetEmailInfo(const char* szSubject, const char* szSender, 
		const char* szRecipient) = 0;

	// Display the OEH dialog.
	virtual bool Show(OEHUI_TYPE UIType, OEHACTION* pAction) = 0;

	// Delete the OEH UI object
	virtual void DeleteObject() = 0;
};
SYM_DEFINE_INTERFACE_ID(IID_IOEHeurUI, 
0x572351e5, 0x3e0, 0x46b4, 0xbc, 0x1e, 0x36, 0x14, 0xf1, 0x58, 0x92, 0x83);

extern "C"
{
	bool WINAPI CreateOEHeurUI(IOEHeurUI** ppOEHeurUI);
	typedef bool (WINAPI *pfnCREATEOEHEURUI)(IOEHeurUI**);
};

#endif

