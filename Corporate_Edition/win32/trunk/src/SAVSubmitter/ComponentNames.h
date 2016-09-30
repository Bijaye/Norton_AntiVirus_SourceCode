// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
#pragma once

namespace SubSDK
{
	namespace Components
	{
		extern const WCHAR* const g_szOEH;
		extern const WCHAR* const g_szVXMS;
		extern const WCHAR* const g_szAV;
		extern const WCHAR* const g_szCOHTrojan;
		extern const WCHAR* const g_szCOHKeylogger;
		extern const WCHAR* const g_szCOHSpyware;
		extern const WCHAR* const g_szCOHPWSteal;
		extern const WCHAR* const g_szCOHGeneral;
		extern const WCHAR* const g_szCOH;
		extern const WCHAR* const g_szCOL;
		extern const WCHAR* const g_szManual;

		//MSL specific
		extern const TCHAR* const g_szMSLKey;
		extern const TCHAR* const g_szMSLDLL;

		//COH specific
		extern const TCHAR* const g_szCOHKey;
		extern const TCHAR* const g_szCOHDLL;
	} // Components

	enum SpecialVIDs
	{
		//these are the VIDs eraser is using...
		eVID_COH_Positive = 0x4ad6,
		eVID_COH_Suspicious = 0x4ad7,

		//these are reserved by response, but not used by eraser
		eVID_COH_PWSteal = 0x4adb,
		eVID_COH_Trojan = 0x4adc,
		eVID_COH_Keylogger = 0x4add,
		eVID_COH_Spyware = 0x4ade,
		eVID_VXMS = 0x4adf,
		
		eVID_COL = 0xffffbaad, //?: not defined yet
	};
} // SubSDK
