// WProtect32IIDs.cpp
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2004, 2005 Symantec Corporation. All rights reserved.
//

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>

#define INITIIDS
#include "defutilsinterface.h"
#include "RoleVector.h"
#include "IUser.h"
#include "ICert.h"
#include "ICertSigningRequest.h"
#include "ccSettingsManagerHelper.h"
#include "SrtControlInterface.h"
// Declares any objects available from this process. Really the below is to
// get a g_DLLObjectCount instance created.
SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()

#undef INITIIDS
