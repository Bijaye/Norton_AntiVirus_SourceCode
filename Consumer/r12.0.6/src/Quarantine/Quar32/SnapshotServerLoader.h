//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// SnapshotServerLoader.h
//

#pragma once


#include "ccSymInterfaceLoader.h" // Has trusted loader
#include "SnapServerSX_i.h"

extern const LPCTSTR sz_SnapServer_dll;
                  

typedef CSymInterfaceDLLHelper<&sz_SnapServer_dll,
                                ccSym::CNAVPathProvider, 
                                cc::CSymInterfaceTrustedCacheLoader,
                                SnapServerSX::ISnapServer, 
                                &SnapServerSX::IID_ISnapServer, 
                                &SnapServerSX::IID_ISnapServer> SnapServer_Loader;
