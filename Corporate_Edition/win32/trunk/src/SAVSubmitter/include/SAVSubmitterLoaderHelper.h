
#pragma once

#include "SAVSubmitterInterface.h"
#include "ccSymInterfaceLoader.h"
#include "SymInterfaceLoader.h"
#include "SAVSubmitterModuleNames.h"

namespace SAVSubmission {

	typedef CSymInterfaceDLLHelper<&sz_SAVSubmitter_dll, 
									ccSym::CProcessPathProvider, 
									CSymInterfaceLoader,	//TODO:JJM: change this to use the trusted loader commented out below
									//cc::CSymInterfaceTrustedCacheMgdLoader, 
									ISAVSubmitter, 
									&CLSID_SAVSubmitter,
									&IID_SAVSubmitter> savSubmission_ISAVSubmitterMgd;

	typedef CSymInterfaceDLLHelper<&sz_SAVSubmitter_dll, 
								ccSym::CProcessPathProvider, 
								CSymInterfaceLoader,	//TODO:JJM: change this to use the trusted loader commented out below
								//cc::CSymInterfaceTrustedLoader, 
								ISAVSubmitter, 
								&CLSID_SAVSubmitter,
								&IID_SAVSubmitter> savSubmission_ISAVSubmitter;

} // namespace SAVSubmission
