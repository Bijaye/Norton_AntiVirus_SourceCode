// IcePackException.h: interface for the IcePack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICEPACKEXCEPTION_H__753BF539_F667_11D2_ADBE_00A0C9C71BBC__INCLUDED_)
#define AFX_ICEPACKEXCEPTION_H__753BF539_F667_11D2_ADBE_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AVISException.h>

class IcePackException : public AVISException
{
public:

	enum TypeOfException {
		SampleID,
		SamplePriority,
		SampleSigPriority,
		SampleForwardTime,
		SetSampleStatus,
		SampleGatewayCookie,
		SampleStatus,
		SampleFinalStatus,
		SampleInfected,
		SampleSigSeqNum,
		SampleNeededSeqNum,
		SampleCompare,
		SampleSetID,
		SampleValueParse,
		SampleScan,
		SampleChanges,
		ScannerSubmittor_ScanPendingSamples,
		QuarantineMonitorPause,
		VQuarantineServer,
		VQSEnum,
		SampleBase,
		SampleStream,
		MappedMemCallback,
		RegistryMonitor
	};

	IcePackException(TypeOfException t, std::string& di);
	IcePackException(TypeOfException t, const char* di);

	explicit IcePackException(TypeOfException t) :
					type(t), detailedInfo(typeAsChar[t])
					{};

	~IcePackException() {};

	TypeOfException	Type(void)			{ return type; };
	std::string		DetailedInfo(void)	{ return detailedInfo; };
	std::string		TypeAsString(void)	{ return std::string(typeAsChar[type]); };
	std::string		ClassAsString(void)	{ return "IcePackException"; }


private:
	TypeOfException	type;
	std::string		detailedInfo;

	static char		*typeAsChar[];
};

#endif // !defined(AFX_ICEPACKEXCEPTION_H__753BF539_F667_11D2_ADBE_00A0C9C71BBC__INCLUDED_)
