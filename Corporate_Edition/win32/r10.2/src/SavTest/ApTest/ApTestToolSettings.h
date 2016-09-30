// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
//////////////////////////////////////////////////////////////////////
#pragma once
#include <vector>

/**
 * ApTestToolSettings encapsulates all tool (not product) settings.
 *
 * Currently, the only tool setting is the list of actions.
 */
class ApTestToolSettings
{
public:
	ApTestToolSettings(void);
	virtual ~ApTestToolSettings(void);

	virtual DWORD SetFileActions( LPCTSTR actions );
	virtual ActionIter GetActionBegin();
	virtual ActionIter GetActionEnd();

protected:
	ActionVector m_fileActions;

	// Declared but not implemented to prevent callers from using default implementations
	ApTestToolSettings(const ApTestToolSettings& oRhs);
	ApTestToolSettings& operator= (const ApTestToolSettings&);
};
