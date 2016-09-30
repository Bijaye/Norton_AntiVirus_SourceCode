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
#pragma once

#include <windows.h>

#include "SymInterface.h"

namespace cc
{
	class IKeyValueCollection;
}

namespace ccEraser
{
	class IAnomaly;
}

namespace ccScanw
{
	class IScannerw;
}

class IQuarantineItem2;

namespace SAVSubmission {

class ISAVSubmitter : public ISymBase
{
public:
	typedef enum { eManual, eSample, eDetection } SubmissionType;
	
	// Logically this should be "const IQuarantineItem2* pQuarantineItem" but IQuarantineItem2 doesn't use const appropriately
	virtual HRESULT SubmitQuarantinedSample(IQuarantineItem2* pQuarantineItem, ccScanw::IScannerw* pScanner) throw() = 0;
	virtual HRESULT SubmitAVDetection(const ccEraser::IAnomaly* pAnomaly, ccScanw::IScannerw* pScanner) throw() = 0;
	virtual HRESULT SubmitCOHSample(const ccEraser::IAnomaly* pAnomaly, ccScanw::IScannerw* pScanner) throw() = 0;
};

// {BD31B35D-DDFC-4ba7-91DD-FDE1229BEFCF}
SYM_DEFINE_INTERFACE_ID(IID_SAVSubmitter, 0xbd31b35d, 0xddfc, 0x4ba7, 0x91, 0xdd, 0xfd, 0xe1, 0x22, 0x9b, 0xef, 0xcf);
TYPEDEF_SYM_POINTERS(ISAVSubmitter, IID_SAVSubmitter);


// Create a client to the SAV submitter
SYM_DEFINE_OBJECT_ID(CLSID_SAVSubmitter, 0xa36d3226, 0xe07f, 0x4d72, 0x97, 0x3b, 0x6a, 0x8f, 0x8c, 0xb5, 0x5d, 0x88);


//
// These are the specific classes of SAV submissions that are supported
//

// {71072142-8CEC-4182-8510-10C0D7DBD41F}
SYM_DEFINE_OBJECT_ID(CLSID_SAVQuarantineSubmission, 0x71072142, 0x8cec, 0x4182, 0x85, 0x10, 0x10, 0xc0, 0xd7, 0xdb, 0xd4, 0x1f);

// {4CEDAC38-B2A8-4d13-83ED-4D35F94338BD}
SYM_DEFINE_OBJECT_ID(CLSID_SAVAVDetection, 0x4cedac38, 0xb2a8, 0x4d13, 0x83, 0xed, 0x4d, 0x35, 0xf9, 0x43, 0x38, 0xbd);

// {83838BBE-28B7-4433-A652-E908EC98B6EB}
SYM_DEFINE_OBJECT_ID(CLSID_SAVCOHSample, 0x83838bbe, 0x28b7, 0x4433, 0xa6, 0x52, 0xe9, 0x8, 0xec, 0x98, 0xb6, 0xeb);

} //namespace SAVSubmission
