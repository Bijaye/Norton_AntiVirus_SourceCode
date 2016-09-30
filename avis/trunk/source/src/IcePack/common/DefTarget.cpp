#include "stdafx.h"
#include "DefTarget.h"

#include <GlobalData.h>
#include <AttributeKeys.h>
#include <Logger.h>

using namespace std;

/*****/
CDefTarget::CDefTarget()
{
	EntryExit enterExit(Logger::LogIcePackDDS, "CDefTarget default constructor");
	Clear();
}
/*****/
void CDefTarget::LogContents(LPCSTR pszHeader)
{
	char szMsg[1000];
	char* pszDefaultHeader = "Target Contents";

	if (pszHeader == NULL)
		pszHeader = pszDefaultHeader;

	::sprintf(szMsg, 
		"%s :\n"
		"computer name = \"%s\",\n"
		"hostname = \"%s\",\n"
		"address = \"%s\",\n"
		"parent = \"%s\",\n"
		"guid = \"%s\",\n"
		"needs blessed only = %u,\n"
		"sample key = %u,\n"
		"defs delivered = %u,\n"
		"seq num delivered = %u,\n"
		"filename = \"%s\"",
		pszHeader,
		m_strComputerName.c_str(),
		m_strHostname.c_str(),
		m_strAddress.c_str(),
		m_strParent.c_str(),
		m_strGuid.c_str(),
		m_boNeedBlessedDefsOnly,
		m_ulKeySample,
		m_boDelivered,
		m_ulSeqNumDelivered,
		m_strFilename.c_str());

	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, szMsg);
}
/*****/
/* Use the following constructor to build a target based on a sample's attributes,
   and whether it has already been distributed. */
CDefTarget::CDefTarget(TargetType targetType, SampleListIterator iterSample, bool boDistributed)
  :	m_strComputerName(""),
	m_strHostname(""),
	m_strAddress(""),
	m_strParent(""),
	m_strGuid(""),
	m_boNeedBlessedDefsOnly(false),
	m_boDelivered(boDistributed),
	m_ulSeqNumDelivered(0),
	m_strFilename(""),
	m_boStatusChanged(false)
{
	char szMsg[1000];

	EntryExit enterExit(Logger::LogIcePackDDS, "CDefTarget PointCast/NarrowCast constructor.");

	if (narrowCast == targetType)
	{
		// try to retrieve the "distributor" (parent) attribute;
		// however, must set it into the "computer name" field.
		iterSample->GetAttribute(AttributeKeys::PlatformDistributor(), m_strComputerName);

		::sprintf(szMsg, "Creating NarrowCast target; computer name = %s.", m_strComputerName.c_str());
		Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, szMsg);

		LogContents("NarrowCast target");

		return;
	}

	// try to retrieve the "computer name" attribute;
	iterSample->GetAttribute(AttributeKeys::PlatformComputer(), m_strComputerName);

	// try to retrieve the "host name" attribute;
	iterSample->GetAttribute(AttributeKeys::PlatformHost(), m_strHostname);

	// try to retrieve the "address" attribute;
	iterSample->GetAttribute(AttributeKeys::PlatformAddress(), m_strAddress);

	// try to retrieve the "distributor" (parent) attribute;
	iterSample->GetAttribute(AttributeKeys::PlatformDistributor(), m_strParent);

	// try to retrieve the "GUID" attribute;
	iterSample->GetAttribute(AttributeKeys::PlatformGUID(), m_strGuid);

	// remember the sample from which the target was formed
	m_ulKeySample = iterSample->SampleKey();

	// try to retrieve the "filename" attribute;
	iterSample->GetAttribute(AttributeKeys::SampleFile(), m_strFilename);

	if (boDistributed)
	{
		// remember the minimum sequence number of definitions we delivered
		m_ulSeqNumDelivered = iterSample->NeededSeqNum();

		Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Recovering a 'distributed' target...");
	}

	LogContents("PointCast target");
}
/*****/
CDefTarget::CDefTarget(TargetType targetType, char* pszTargetMachineName)
  :	m_strComputerName(pszTargetMachineName),
    m_strHostname(""),
	m_strAddress(""),
	m_strParent(""),
	m_strGuid(""),
	m_ulKeySample(0),
	m_boDelivered(false),
	m_ulSeqNumDelivered(0),
	m_strFilename(""),
	m_boStatusChanged(false)
{
	EntryExit enterExit(Logger::LogIcePackDDS, "CDefTarget BroadCast constructor.");

	// specify the minimum sequence number needed, and whether it must be blessed
	m_boNeedBlessedDefsOnly = (blessedBroadcast == targetType);

	LogContents("BroadCast target");
}
/*****/
CDefTarget::CDefTarget(
	const std::string& strComputerName,
	const std::string& strHostname,
	const std::string& strAddress,
	const std::string& strParent,
	const std::string& strGuid,
	bool boNeedBlessedDefsOnly,
	ulong ulKeySample,
	bool boDelivered,
	ulong ulSeqNumDelivered,
	const std::string& strFilename)
 :	m_strComputerName(strComputerName),
	m_strHostname(strHostname),
	m_strAddress(strAddress),
	m_strParent(strParent),
	m_strGuid(strGuid),
	m_boNeedBlessedDefsOnly(boNeedBlessedDefsOnly),
	m_ulKeySample(ulKeySample),
	m_boDelivered(boDelivered),
	m_ulSeqNumDelivered(ulSeqNumDelivered),
	m_strFilename(strFilename),
	m_boStatusChanged(false)
{
	LogContents("Debug target");
}
/*****/
// is the target argument, dt, a duplicate of the current target?
bool CDefTarget::IsDuplicate(CDefTarget& dt)
{
	if (m_strComputerName != dt.GetComputerName())
		return false;

	if (m_strHostname != dt.GetHostname())
		return false;

	if (m_strAddress != dt.GetAddress())
		return false;

	if (m_strParent != dt.GetParent())
		return false;

	if (m_strGuid != dt.GetGuid())
		return false;

	// if target machines are the same, but one needs blessed defs and the other
	// doesn't, then the targets are NOT duplicates.
	if (m_boNeedBlessedDefsOnly != dt.NeedsBlessedDefsOnly())
		return false;

	// the elements either both require blessed defs, or both don't require blessed defs
	return true;
}
/*****/
CDefTarget::~CDefTarget()
{
}
/*****/
bool CDefTarget::MarkAsDistributed(ulong ulSeqNumDelivered)
{
	m_boDelivered = true;
	m_ulSeqNumDelivered = ulSeqNumDelivered;

	char szMsg[1000];

	::sprintf(szMsg, "Marking target as having been distributed defs with sequence number %u.", ulSeqNumDelivered);
	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, szMsg);

	return UpdateSampleStatus(SampleStatus::distributed);
}
/*****/
bool CDefTarget::MarkAsInstalled()
{
	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Marking target as installed.");

	return UpdateSampleStatus(SampleStatus::installed);
}
/*****/
bool CDefTarget::MarkAsNotInstalled()
{
	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Marking target as NOT installed.");

	return UpdateSampleStatus(SampleStatus::notInstalled);
}
/*****/
void CDefTarget::ResetStatusChanged()
{
	m_boStatusChanged = false;
}
/*****/
bool CDefTarget::NeedsBlessedDefsOnly()
{
	return m_boNeedBlessedDefsOnly;
}
/*****/
bool CDefTarget::DefsWereDelivered()
{
	return m_boDelivered;
}
/*****/
ulong CDefTarget::GetSeqNumDelivered()
{
	return m_ulSeqNumDelivered;
}
/*****/
std::string CDefTarget::GetComputerName()
{
	return m_strComputerName;
}
/*****/
std::string CDefTarget::GetHostname()
{
	return m_strHostname;
}
/*****/
std::string CDefTarget::GetAddress()
{
	return m_strAddress;
}
/*****/
std::string CDefTarget::GetParent()
{
	return m_strParent;
}
/*****/
std::string CDefTarget::GetGuid()
{
	return m_strGuid;
}
/*****/
ulong CDefTarget::GetSampleKey()
{
	return m_ulKeySample;
}
/*****/
std::string CDefTarget::GetFilename()
{
	return m_strFilename;
}
/*****/
bool CDefTarget::IsPointCastTarget()
{
	return (m_ulKeySample > 0);
}
/*****/
CDefTarget::ResultCode CDefTarget::MapResultCode(defcaststatus dcs)
{
	switch(dcs)
	{
		case SUCCESS:
			return CDefTarget::Success;

		// transient errors:
		case TOOMANY:			// [transient] too many concurrent function calls
			Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Defcast returned status of TOOMANY.");
			return CDefTarget::TransientError;

		case NOTCONNECTED:		// [transient] target is not connected to network
			Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Defcast returned status of NOTCONNECTED.");
			return CDefTarget::TransientError;

		case INCOMPLETE:		// [transient] function did not complete successfully
			Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Defcast returned status of INCOMPLETE.");
			return CDefTarget::TransientError;

		case TIMEOUT:			// [transient] function did not complete within timeout limit
			Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Defcast returned status of TIMEOUT.");
			return CDefTarget::TransientError;

		// permanent errors:
		case INVALID:			// [permanent] target name and/or address values are invalid
			Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Defcast returned status of INVALID.");
			return CDefTarget::PermanentError;

		case UNMANAGED:			// [permanent] target is not managed by NAVCE
			Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Defcast returned status of UNMANAGED.");
			return CDefTarget::PermanentError;

		case NOTINITIALIZED:	// the DefCast component is not initialized
			Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Defcast returned status of NOTINITIALIZED.");
			return CDefTarget::PermanentError;

		case TERMINATING:		// the DefCast component is terminating
			Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Defcast returned status of TERMINATING.");
			return CDefTarget::PermanentError;

		case OUTOFMEMORY:
			Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Defcast returned status of OUTOFMEMORY.");
			return CDefTarget::PermanentError;

		// unknown status codes
		default:
			Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, "Defcast returned an unknown result.");
			return CDefTarget::UnknownResult;
	}
}
/*****/
CDefTarget::ResultCode CDefTarget::InitApi()
{
	defcaststatus dcs;

	try
	{
		dcs = ::DefCastInit();
	}
	catch(...)
	{
		Logger::Log(Logger::LogError, Logger::LogIcePackDDS, "Exception occurred while calling DefCastInit().");
		return CDefTarget::ExceptionOccurred;
	}

	return CDefTarget::MapResultCode(dcs);
}
/*****/
CDefTarget::ResultCode CDefTarget::TermApi()
{
	defcaststatus dcs;

	try
	{
		dcs = ::DefCastTerm();
	}
	catch(...)
	{
		Logger::Log(Logger::LogError, Logger::LogIcePackDDS, "Exception occurred while calling DefCastTerm().");
		return CDefTarget::ExceptionOccurred;
	}

	return CDefTarget::MapResultCode(dcs);
}
/*****/
CDefTarget::ResultCode CDefTarget::QueryInstalledDefs(int iTimeout, unsigned long int* pulSequence)
{
	defcaststatus dcs;

	try
	{
		char szMsg[1000];

		::sprintf(szMsg, "About to call DefCastQuery();\n"
			"timeout = %d,\n"
			"computer name = \"%s\",\n"
			"hostname = \"%s\",\n"
			"address = \"%s\",\n"
			"parent = \"%s\",\n"
			"guid = \"%s\"",
			iTimeout,
			m_strComputerName.c_str(),
			m_strHostname.c_str(),
			m_strAddress.c_str(),
			m_strParent.c_str(),
			m_strGuid.c_str());
		Logger::Log(Logger::LogDebug, Logger::LogIcePackDDS, szMsg);

		dcs = ::DefCastQuery(
				iTimeout,
				(char* )m_strComputerName.c_str(),
				(char* )m_strHostname.c_str(),
				(char* )m_strAddress.c_str(),
				(char* )m_strParent.c_str(),
				(char* )m_strGuid.c_str(),
				pulSequence);

		if (dcs == SUCCESS)
		{
			::sprintf(szMsg, "DefCastQuery() returned %u as the installed sequence number.", *pulSequence);
			Logger::Log(Logger::LogDebug, Logger::LogIcePackDDS, szMsg);
		}
	}
	catch(...)
	{
		Logger::Log(Logger::LogError, Logger::LogIcePackDDS, "Exception occurred while calling DefCastQuery().");
		return CDefTarget::ExceptionOccurred;
	}

	return CDefTarget::MapResultCode(dcs);
}
/*****/
CDefTarget::ResultCode CDefTarget::DeliverDefs(int iTimeout, const char* pszPackage)
{
	defcaststatus dcs;

	try
	{
		char szMsg[1000];

		::sprintf(szMsg, "About to call DefCastInstall();\n"
			"timeout = %d,\n"
			"computer name = \"%s\",\n"
			"hostname = \"%s\",\n"
			"address = \"%s\",\n"
			"parent = \"%s\",\n"
			"guid = \"%s\",\n"
			"package path = \"%s\"",
			iTimeout,
			m_strComputerName.c_str(),
			m_strHostname.c_str(),
			m_strAddress.c_str(),
			m_strParent.c_str(),
			m_strGuid.c_str(),
			pszPackage);
		Logger::Log(Logger::LogDebug, Logger::LogIcePackDDS, szMsg);

		dcs = ::DefCastInstall(
				iTimeout,
				(char* )m_strComputerName.c_str(),
				(char* )m_strHostname.c_str(),
				(char* )m_strAddress.c_str(),
				(char* )m_strParent.c_str(),
				(char* )m_strGuid.c_str(),
				(char* )pszPackage);
	}
	catch(...)
	{
		return CDefTarget::ExceptionOccurred;
	}

	return CDefTarget::MapResultCode(dcs);
}
/*****/
bool CDefTarget::UpdateSampleStatus(SampleStatus::Status status)
{
	if (m_ulKeySample == 0)
		return false;

	// lookup the sample
	Sample samp;

	// set the id of the sample to be the key we stored 
	// for the sample. *** This will retrieve the
	// sample data ***
	samp.Key(m_ulKeySample);

	// if we are unable to find the sample in quarantine, this means that someone
	// deleted it. In this case, we cannot set the sample status.
	if (samp.IsNull())
		return false;

	char szMsg[100];

	::sprintf(szMsg, "Updating sample status to %s...", SampleStatus::ToConstChar(status));
	Logger::Log(Logger::LogInfo, Logger::LogIcePackDDS, szMsg);

	// update the status
	samp.Status(status);

	// mark the sample as "changed"
	samp.Mark(Sample::sChanged);

	// update it.
	GlobalData::UpdateMasterSampleList(samp);

	// detach the sample from the quarantine server
	samp.DeActivate();

	// remember that we changed the status
	m_boStatusChanged = true;

	return true;
}
/*****/
SampleStatus::Status CDefTarget::GetSampleStatus()
{
	if (m_ulKeySample == 0)
		return SampleStatus::unknown;

	// lookup the sample
	Sample samp;

	// set the id of the sample to be the key we stored 
	// for the sample. *** This will retrieve the
	// sample data ***
	samp.Key(m_ulKeySample);

	// if we are unable to find the sample in quarantine, this means that someone
	// deleted it, leaving us having an unknown sample status.
	if (samp.IsNull())
		return SampleStatus::unknown;

	// we have a valid sample status
	return samp.Status();
}
/*****/
bool CDefTarget::SampleExistsInQuarantine()
{
	if (m_ulKeySample == 0)
		return false;

	// lookup the sample
	Sample samp;

	// check for a valid sample
	return samp.Key(m_ulKeySample);
}
/*****/
bool CDefTarget::SampleStatusHasChanged()
{
	return m_boStatusChanged;
}
/*****/
void CDefTarget::Clear()
{
    m_strComputerName="";
	m_strHostname="";
	m_strAddress="";
	m_strParent="";
	m_strGuid="";
	m_boNeedBlessedDefsOnly=false;
	m_ulKeySample=0;
	m_boDelivered=false;
	m_ulSeqNumDelivered=0;
	m_strFilename="";
	m_boStatusChanged=false;
}
/*****/
bool CDefTarget::IsClear()
{
    if ((!m_strComputerName.size())&&
	(!m_strHostname.size())&&
	(!m_strAddress.size())&&
	(!m_strParent.size())&&
	(!m_strGuid.size()))
	{
		return true;
	}
	else
	{
		return false;
	}
}
