#pragma once

#include "ccSymPathProvider.h"
#include "ccSymInterfaceLoader.h" // Has trusted loader
#include "ccStreamInterface.h"
#include "SDPack_i.h"
using namespace SDPACK;

extern const LPCTSTR sz_SDPack_dll;

typedef CSymInterfaceManagedDLLHelper<&sz_SDPack_dll,
                                ccSym::CNAVPathProvider, 
                                cc::CSymInterfaceTrustedCacheMgdLoader,
                                ISNDSubmission, 
                                &IID_ISNDSubmission, 
                                &IID_ISNDSubmission> SNDSubmissionDLL_Loader;

typedef CSymInterfaceManagedDLLHelper< &sz_SDPack_dll,
                                ccSym::CNAVPathProvider, 
                                cc::CSymInterfaceTrustedCacheMgdLoader,
                                ISNDSample, 
                                &IID_ISNDSample, 
                                &IID_ISNDSample> SNDSampleDLL_Loader;
