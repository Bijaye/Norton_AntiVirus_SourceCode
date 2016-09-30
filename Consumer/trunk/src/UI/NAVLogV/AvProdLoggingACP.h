////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <atlsecurity.h>

// AvProd logging Access Control Policy class
namespace AvProdLogging
{

class CAvProdLoggingACP
{
public:
	CAvProdLoggingACP(void);
	virtual ~CAvProdLoggingACP(void);

public: // methods
	
	// Global Access
	virtual bool AllowGlobalClearLogs(bool bPromptForElevation = true);

	// Context Access
	virtual bool AllowDeleteThisItem(LPCTSTR szSIDString, bool bPromptForElevation = true);
	virtual bool AllowRestoreThisItem(LPCTSTR szSIDString, bool bPromptForElevation = true);
	virtual bool AllowViewItemDetails(LPCTSTR szSIDString, bool bPromptForElevation = true);
	virtual bool AllowExcludeItem(LPCTSTR szSIDString, bool bPromptForElevation = true);

	// Utility methods
	virtual bool HasAdminPrivilege(bool bPromptForElevation = false);
	virtual const SID* GetUserSID();
    virtual LPCTSTR GetUserSIDString();

    // Comparison
    bool EqualSID(LPCTSTR szSID1);

protected: // methods

	void CalcAdminPrivilege(bool bPromptForElevation);
	void CalcUserSID();

protected:
    ATL::CSid m_sidCurrentUserSID;
    DWORD m_dwHasAdminPrivilege;

};


};