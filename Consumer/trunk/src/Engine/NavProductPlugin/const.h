////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef _CONST_H_
#define _CONST_H_


const TCHAR cszNAVUpgradeCode[] =		_T("{68C33091-72FA-4DB8-A40E-8CEAE4552D73}");
const TCHAR cszNAVPROUpgradeCode[] =	_T("{CD05B6DD-65EC-43AC-BE00-73394F19B3AA}");
const TCHAR cszNISUpgradeCode[]	=		_T("{E363D102-3D7F-11D3-AF49-00600811C705}");

const char* szNAVUnRemovedFiles[] =
{
	 "ccIMScan.dll",
	 "ccIMScan.exe",
	 "NavEmail.dll",
	 "NavError.dll",
	 "NAVlogv.dll",
	 "AVRES.dll",
	 "OEHeur.dll",
	 "Qspak32.dll",
	 "savRT32.dll",
	 NULL
};

const char* szNAVRemovedFiles[] =
{
	 "AboutPlg.dll",
     "Apwutil.dll",
	 "Cfgwiz.dll",
	 "Cfgwiz.exe",
	 "DefAlert.exe",	 
	 "Navapi32.dll",
	 "NAVLnch.dll",
	 "NAVOpts.dll",
	 "NAVShExt.dll",
	 "NAVSTATS.dll",
	 "NAVTskWz.dll",
	 "NAVUI.dll", 
	 "qconres.dll",
	 "qconsole.exe",
	 "Scandlvr.dll",
	 "Scandres.dll",
	 "ScanMgr.dll",
	 "ScriptUI.dll",
	 "VERSION.DAT",	 
	  NULL	
};

enum MODULE_IDs
{
	NAVPRODUCTPLUGIN_ERR_START = 0
};

#endif