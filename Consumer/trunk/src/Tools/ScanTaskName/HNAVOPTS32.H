////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "navopt32.h"

class CHNAVOPTS32  // Wrapper for the file primitive
{
	HNAVOPTS32 h;
public:
	CHNAVOPTS32(void) : h(NULL) {}
	~CHNAVOPTS32(void) { if (h) ::NavOpts32_Free(h); }
	NAVOPTS32_STATUS Allocate(void) { return NavOpts32_Allocate(&h); }
	NAVOPTS32_STATUS Load(const wchar_t* szFileName) { return NavOpts32_LoadU(szFileName, h, FALSE); }
	NAVOPTS32_STATUS Save(const wchar_t* szFileName) { return NavOpts32_SaveU(szFileName, h); }
	NAVOPTS32_STATUS GetStringValue(const char* szOption, char szBuffer[], int iSize) { return NavOpts32_GetStringValue(h, szOption, szBuffer, iSize, "*"); }
	NAVOPTS32_STATUS GetDwordValue(const char* szOption, DWORD& dwValue, DWORD dwDefault) { return NavOpts32_GetDwordValue(h, szOption, &dwValue, dwDefault); }
	NAVOPTS32_STATUS GetBinaryValue(const char* szOption, LPBYTE pBuffer, DWORD dwBufferSize) { return NavOpts32_GetBinaryValue(h, szOption, pBuffer, dwBufferSize); }
	NAVOPTS32_STATUS SetStringValue(const char* szOption, const char szBuffer[]) { return NavOpts32_SetStringValue(h, szOption, szBuffer); }
	NAVOPTS32_STATUS SetDwordValue(const char* szOption, DWORD dwValue) { return NavOpts32_SetDwordValue(h, szOption, dwValue); }
	NAVOPTS32_STATUS SetBinaryValue(const char* szOption, LPBYTE pBuffer, DWORD dwDataLength) { return NavOpts32_SetBinaryValue(h, szOption, pBuffer, dwDataLength); }
};
