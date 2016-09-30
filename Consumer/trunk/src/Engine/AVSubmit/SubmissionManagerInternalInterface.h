////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccEraserInterface.h"
#include "ccScanwInterface.h"
#include "AVInterfaces.h"
#include "SubmissionEngineInterface.h"
#include "QBackupInterfaces.h"

namespace AVSubmit {
namespace mgr {

class IManager : public ISymBase
{
public:
	virtual HRESULT GetScanner(ccScanw::IScannerw*& pScanner) throw() = 0;
	virtual HRESULT GetEraser(ccEraser::IEraser4*& pEraser) throw() = 0;
	virtual HRESULT GetAVDefInfo(AVModule::IAVDefInfo*& pDefInfo) throw() = 0;
	virtual HRESULT GetSubmissionEngine(clfs::ISubmissionEngine*& pSubmissionEngine) throw() = 0;
	virtual HRESULT GetAVThreatInfo(AVModule::IAVThreatInfo*& pThreatInfo) throw() = 0;
	virtual HRESULT GetQBackup(QBackup::IQBackup*& pBackup) throw() = 0;
};

// {6443500A-634D-4edb-9149-9C11E063A5AF}
SYM_DEFINE_INTERFACE_ID(IID_IManager, 0x6443500a, 0x634d, 0x4edb, 0x91, 0x49, 0x9c, 0x11, 0xe0, 0x63, 0xa5, 0xaf);
TYPEDEF_SYM_POINTERS(IManager, IID_IManager);

}
}