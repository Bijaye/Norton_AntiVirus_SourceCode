#include <string>
#include <list>

#include <AVIS.h>
#include <DefCast.h>
#include <SampleStatus.h>

#include "Sample.h"

// new types
typedef std::list<Sample>::iterator SampleListIterator;

// the class CDefTarget represents an object maintained on a list. Each element on
// this list contains information about the target to which to deliver definitions
// elements are always removed from the front, and added to the back.
// This class abstracts the sending of definitions to a target machine, including the 
// use of the DefCast API.
class CDefTarget
{
private:
	std::string m_strComputerName;		// name of target, or NULL
	std::string m_strHostname;			// TCP/IP fully qualified hostname of target, or NULL
	std::string m_strAddress;			// IP and/or IPX network address of target, or NULL
	std::string m_strParent;			// NAVCE distribution server for target, or NULL	
	std::string m_strGuid;				// NAVCE client identifier of target, or NULL
	bool m_boNeedBlessedDefsOnly;		// flag indicating whether the needed definitions must be blessed
	ulong m_ulKeySample;				// id
	bool m_boDelivered;					// flag indicating whether the definitions have been delivered, but not yet installed
	ulong m_ulSeqNumDelivered;			// sequence number of definitions delivered to the target
	std::string m_strFilename;			// filename associated with sample
	bool m_boStatusChanged;

public:
	typedef enum 
	{
		pointCast,
		narrowCast,
		unblessedBroadcast,
		blessedBroadcast

	} TargetType;

	// default constructor
	CDefTarget();

	// constructor to build a target based on a samples attributes (pointcast or
	// narrowcast targets)
	CDefTarget(TargetType targetType, SampleListIterator iterSample, bool boDistributed = false);

	// constructor to build a broadcast target.
	CDefTarget(TargetType targetType, char* pszTargetMachineName);

	// "testing" constructor
	CDefTarget(
		const std::string& strComputerName,
		const std::string& strHostname,
		const std::string& strAddress,
		const std::string& strParent,
		const std::string& strGuid,
		bool boNeedBlessedDefsOnly,
		ulong ulKeySample,
		bool boDelivered,
		ulong ulSeqNumDelivered,
		const std::string& strFilename);

	~CDefTarget();
	bool IsClear();
	void Clear();

public:
	enum ResultCode
	{
		Success,
		TransientError,
		PermanentError,
		ExceptionOccurred,
		UnknownResult
	};

public:
	bool IsDuplicate(CDefTarget& dt);

public:
	// "set" methods
	bool UpdateSampleStatus(SampleStatus::Status status);
	bool MarkAsDistributed(ulong ulSeqNumDelivered);
	bool MarkAsInstalled();
	bool MarkAsNotInstalled();
	void ResetStatusChanged();

	// "get" methods
	bool NeedsBlessedDefsOnly();
	bool DefsWereDelivered();
	ulong GetSeqNumDelivered();
	bool IsPointCastTarget();
	std::string GetComputerName();
	std::string GetHostname();
	std::string GetAddress();
	std::string GetParent();
	std::string GetGuid();
	ulong GetSampleKey();
	SampleStatus::Status GetSampleStatus();
	std::string GetFilename();
	bool SampleStatusHasChanged();

	// other methods
	bool SampleExistsInQuarantine();
	void LogContents(LPCSTR pszHeader = NULL);

public:
	ResultCode QueryInstalledDefs(int iTimeout, unsigned long int* pulSequence);
	ResultCode DeliverDefs(int iTimeout, const char* pszPackage);

public:
	static ResultCode MapResultCode(defcaststatus dcs);
	static ResultCode InitApi();
	static ResultCode TermApi();
};
