////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined THREATLEVELLOADER_H_INCLUDED_
#define THREATLEVELLOADER_H_INCLUDED_

#pragma once

#include "ThreatLevelInterface.h"
#include "ccSymPathProvider.h"
#include "SymInterfaceManagedLoader.h"
#include "ccModuleNames.h"

// This loader assumes that the calling module is in the same directory as TLevel.dll, if not this will fail
typedef CSymInterfaceDLLHelper<&cc::sz_ThreatLevel_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader,
                               IThreatLevel, 
                               &IID_ThreatLevel, 
                               &IID_ThreatLevel> ThreatLevel_Loader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ThreatLevel_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader,
                                      IThreatLevel, 
                                      &IID_ThreatLevel, 
                                      &IID_ThreatLevel> MgdThreatLevel_Loader;

// This loader assumes that the calling module is in the same directory as TLevel.dll, if not this will fail
typedef CSymInterfaceDLLHelper<&cc::sz_ThreatLevel_dll,
                               ccSym::CModulePathProvider, 
                               CSymInterfaceLoader,
                               IThreatLevelSettings, 
                               &IID_ThreatLevelSettings, 
                               &IID_ThreatLevelSettings> ThreatLevelSettings_Loader;

typedef CSymInterfaceManagedDLLHelper<&cc::sz_ThreatLevel_dll,
                                      ccSym::CModulePathProvider, 
                                      CSymInterfaceManagedLoader,
                                      IThreatLevelSettings, 
                                      &IID_ThreatLevelSettings, 
                                      &IID_ThreatLevelSettings> MgdThreatLevelSettings_Loader;

#endif  //#if !defined THREATLEVELLOADER_H_INCLUDED_