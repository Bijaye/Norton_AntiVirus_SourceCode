// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004, 2005 Symantec Corporation.. All rights reserved.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
//////////////////////////////////////////////////////////////////////

#pragma once

#include "CliproxyATL_h.h"

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>

class CCliProxyModule : public CAtlDllModuleT< CCliProxyModule >
{
public :
	DECLARE_LIBID(LIBID_CliProxyLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_CLIPROXY, "{4E9AEDCC-5516-41cc-AF40-2740C2310662}")
};
