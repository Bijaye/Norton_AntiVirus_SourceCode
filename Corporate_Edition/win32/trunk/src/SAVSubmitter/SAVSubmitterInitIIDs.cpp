// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
// AVSubmit.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#define INITIIDS
#define INITGUID

#include  <guiddef.h>

#include "SAVSubmitterInterface.h"
#include "SAVSubmitterImpl.h"
#include "SAVQuarantineSubmission.h"
#include "SAVAVDetection.h"

#include "ccSymDebugOutput.h"
#include "ccTrace.h"
#include "ccSettings/ccSettingsLoader.h"
#include "ccVerifyTrustInterface.h"
#include "ccCloneInterface.h"
