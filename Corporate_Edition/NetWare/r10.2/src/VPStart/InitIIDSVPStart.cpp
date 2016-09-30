// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2004, 2005 Symantec Corporation. All rights reserved.
//*************************************************************************
// InitIIDSVPStart.cpp - This file provides the initialization for shared
// SymInterface style interfaces for the Netware VPStart project.
//
// This CPP file should be included in any PScan project wishing to access
// these interfaces.
//
// NOTE: stdafx.h is not included here. If this file is included in a project
// the pre-compiler headers must be turn off for this file.

#define INITIIDS

#ifdef NLM
#include <stdio.h>
#include "nlm.h"
#include "winThreads.h"
#endif

#include "SymInterface.h"

#include "IAuth.h"
#include "IUser.h"
#include "IMessageManager.h"
#include "ICert.h"
#include "ICertSigningRequest.h"
#include "ICertIssuer.h"
#include "IByteStreamer.h"

SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END()
