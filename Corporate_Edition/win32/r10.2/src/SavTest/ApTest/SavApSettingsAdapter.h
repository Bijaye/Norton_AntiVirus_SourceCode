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

#include "ISettings.h"

/**
 * SavApSettingsAdapter manages settings for SavApTestTool and allows for manipulation of SAV AP product
 * settings as well.
 */
class SavApSettingsAdapter : public ISettings
{
public:
	SavApSettingsAdapter(void);
	virtual ~SavApSettingsAdapter(void);

	virtual DWORD GetDword( LPCTSTR pszName, DWORD* pdwValue );
	virtual DWORD GetString( LPCTSTR pszName, LPCTSTR pszValue );
	virtual SYMTESTRESULT Set( LPCTSTR pszName, DWORD dwValue );
	virtual SYMTESTRESULT Set( LPCTSTR pszName, LPCTSTR pszValue );

protected:
	// Declared but not implemented to prevent callers from using default implementations
	SavApSettingsAdapter(const SavApSettingsAdapter& oRhs);
	SavApSettingsAdapter& operator= (const SavApSettingsAdapter&);
};