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
#include "StdAfx.h"
#include ".\savapsettingsadapter.h"

SavApSettingsAdapter::SavApSettingsAdapter(void)
{
}

SavApSettingsAdapter::~SavApSettingsAdapter(void)
{
}

DWORD SavApSettingsAdapter::GetDword( LPCTSTR pszName, DWORD* pdwValue )
{
	DWORD ret = ERROR_SUCCESS;
	*pdwValue = 0;
	return ret;
}

DWORD SavApSettingsAdapter::GetString( LPCTSTR pszName, LPCTSTR pszValue )
{
	DWORD ret = ERROR_SUCCESS;
	pszValue = "";
	return ret;
}

SYMTESTRESULT SavApSettingsAdapter::Set(LPCTSTR pszName, DWORD dwValue)
{
	SYMTESTRESULT ret = SYMTEST_SUCCESS;
	return ret;
}

SYMTESTRESULT SavApSettingsAdapter::Set(LPCTSTR pszName, LPCTSTR pszValue)
{
	SYMTESTRESULT ret = SYMTEST_SUCCESS;
	return ret;
}