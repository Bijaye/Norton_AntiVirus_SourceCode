// NAVScan.h: interface for the NAVScan class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(NAVSCAN_H)
#define NAVSCAN_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include <AVIS.h>
#include "../../icepack/common/registrydefaults.h"
class NAVScan  
{
public:
	enum ScanRC {
				  Infected,				// only used internially in the scanner to
										// indicate that a virus was found, before
										// leaving the scanner this should be converted
										// to a different code that indicates how
										// the repair operation went.

				  NotInfectable,		// The file cannot be infected
										// return to sender.

				  NotInfected,			// no virus was detected,
										// generally this means forward this request
										// up the chain.  (On the client the sample
										// gets up on hold until the admin decides it's
										// important if the sample was submitted manually.

				  UnSubmittable,		// Scan found something bad, but not a
										// virus, could be a Trogan horse, whatever
										// it is we are done with the file

				  Heuristic,			// Found a virus using a heuristic.  Repair
										// was not attempted.
										// Forward file on for futher analysis.

				  NoRepairInstructions,	// Scan found a virus, but there are no
										// repair instructions for the virus so
										// forward it on for further analysis.

				  RepairTriedAndError,	// Found a virus, should be able to repair it,
										// but an error occurred that prevented repair,
										// forward it on for further analysis.

				  RepairTriedAndFailed,	// Found a virus, should be able to repair it,
										// NAV returned "CANT REPAIR", so
										// forward it on for further analysis.

				  Repaired,				// A virus was found and repaired (in a copy of the
										// file), return to the
										// gateway/user that the problem has been solved and
										// what signatures to use.  (Used to be called "Infected")
				  
				  BadScan,				// Nav returned an error or threw an exception while attempting
										// to scan the file.
										// forward it on for further analysis.


//				  NAVrepairOverrun,		// Nav tried to write beyond the padding at the end of 
										// a file while doing a repair operation.
		
//				  InvalidFile, NavHung,
				  NavCrashed,
				  NavVcdInitError, NavDefinitionFileError,
//				  NavUnknownError, NavUnknownScanError,
				  NavLoadDefFileError,
//				  NavMemoryError, NavInvalidArg,
				  SigDefMissing,

				  ScannerServiceError,

				  WaitForScanTimedOut,  CommWithScannerOffline,
				  BadResponceFromScanner,

				  UnknownError
	};


	//	fileName, localCookie, and updateDB are input variables
	//	signatureSeq is output only and specifies the sig used to scan
	//	virusName is output only and, if the virus is found, contains the name
	//				of the virus found.
	static ScanRC	ScanBlessed(const std::string& fileName,
								const uint localCookie, const bool updateDB,
								std::string& virusName,
								uint& signatureSeq, std::string& navVersion,
								uint& virusID);
	static ScanRC	ScanNewest(const std::string& fileName,
								const uint localCookie, const bool updateDB,
								std::string& virusName,
								uint& signatureSeq, std::string& navVersion,
								uint& virusID);

	//	In this case the signatureSeq number is input only and specifies
	//	which signature to use when scanning this file.
	static ScanRC	ScanExplicit(const std::string& fileName,
								const uint localCookie, const bool updateDB,
								std::string& virusName,
								const uint signatureSeq, 
								std::string& navVersion,
								uint& virusID);

	static const char* RCtoChar(ScanRC rc)
	{
		switch (rc)
		{
		case Infected:					return "Infected";
		case NotInfectable:				return "NotInfectable";
		case NotInfected:				return "NotInfected";
		case UnSubmittable:				return "UnSubmittable";
		case Heuristic:					return "Heuristic";
		case NoRepairInstructions:		return "NoRepairInstructions";
		case RepairTriedAndError:		return "RepairTriedAndError";
		case RepairTriedAndFailed:		return "RepairTriedAndFailed";
		case Repaired:					return "Repaired";
		case BadScan:					return "BadScan";
//		case NAVrepairOverrun:			return "NAVrepairOverrun";
//		case InvalidFile:				return "InvalidFile";
//		case NavHung:					return "NavHung";
		case NavCrashed:				return "NavCrashed";
		case NavVcdInitError:			return "NavVcdInitError";
		case NavDefinitionFileError:	return "NavDefinitionFileError";
//		case NavUnknownError:			return "NavUnknownError";
//		case NavUnknownScanError:		return "NavUnknownScanError";
		case NavLoadDefFileError:		return "NavLoadDefFileError";
//		case NavMemoryError:			return "NavMemoryError";
//		case NavInvalidArg:				return "NavInvalidArg";
		case SigDefMissing:				return "SigDefMissing";
		case WaitForScanTimedOut:		return "WaitForScanTimedOut";
		case CommWithScannerOffline:	return "CommWithScannerOffline";
		case BadResponceFromScanner:	return "BadResponceFromScanner";
		case UnknownError:				return "UnknownError";
		default:						return "Uknown NAVScan error";
		}
	}

	static bool		InfectedState(ScanRC rc)
					{
						if (Heuristic			== rc ||
							NoRepairInstructions== rc ||
							RepairTriedAndError	== rc ||
							RepairTriedAndFailed== rc ||
							Repaired			== rc )
							return true;
						else
							return false;
					}

					//
					//	Note:  These methods return the signature sequence number
					//			in use at the time you called them.  They may not
					//			be the latest or blessed version (see the database
					//			if that is what you want).  The sequence number
					//			could have changed by the time these methods return.
					//
					//	These methods return true if they suceeded, false if they
					//  failed.  Reasons for failure could be, but are not limited
					//  to, named pipe to service is not up or is full, the required
					//  scan service is not running, hung, or unable to return an
					//  answer in a timely fashion.

	static bool		BlessedSeqNum(uint& sigSeqNum);
	static bool		LatestSeqNum(uint& sigSeqNum);
	static bool		ExplicitSeqNum(uint& sigSeqNum);


					//
					//	Load the requested signatures (if not already in use) and
					//	return the definition version string in YYMMDD.VVV where VVV
					//	is the daily version number.

	static bool		ExplicitDefVersion(std::string& defVersion, uint sigSeqNum);

					//
					//	See above note about value only good at time you called
					//
					//	These methods return true if they suceeded, false if they
					//	failed.  (see above for notes about failure reasons.

	static bool		BlessedSigPath(std::string& sigPath);
	static bool		LatestSigPath(std::string& sigPath);
	static bool		ExplicitSigPath(std::string& sigPath);


private:
	NAVScan();
	virtual ~NAVScan();

	static ScanRC	ScanFile(const std::string& fileName, const uint localCookie,
							 const bool updateDB, std::string& virusName,
							 uint& signatureSeq,  std::string& navVersion,
							 std::string& pipeName, uint& virusID);

	static bool		GetSeqNum(uint& sigSeqNum, std::string& pipeName);
	static bool		GetSigPath(std::string& sigPath, std::string& pipeName);
	static bool		GetDefVersion(std::string& defVersion, uint seqNum, std::string& pipeName);


	static std::string	blessedPipe;
	static std::string	latestPipe;
	static std::string	explicitPipe;
	static bool			initialized;
	static uint			messageID;

	static bool			Initialize();

	//
	//	Wait up to 60 seconds for the scanner to respond to a request
	//	(NOTE:  this is after a pipe is successfully openned to the scanner
	//	service).
	static uint maxWaitForScannerResponse;
	static uint	scanRetryInterval;
	static uint scanRetryLimit;

	static uint SetValuesFromRegistry(void);

  enum { maxWaitMin        = ICEP_MIN_SCANCOMPLETIONTIMEOUT, 
         maxWaitMax        = ICEP_MAX_SCANCOMPLETIONTIMEOUT, 
         maxWaitDefault    = ICEP_DEF_SCANCOMPLETIONTIMEOUT,
         retryIntMin       = ICEP_MIN_SCANRETRYINTERVAL, 
         retryIntMax       = ICEP_MAX_SCANRETRYINTERVAL, 
         retryIntDefault   = ICEP_DEF_SCANRETRYINTERVAL,
         retryLimitMin     = ICEP_MIN_SCANRETRYLIMIT,
         retryLimitMax     = ICEP_MAX_SCANRETRYLIMIT, 
         retryLimitDefault = ICEP_DEF_SCANRETRYLIMIT
 	};

	static bool NAVScan::SendMessageToScanService(std::string pipeName, std::string& message,
					   char* readBuffer, const ulong readBuffSize, ulong& bytesReturned,
					   const char* whereFrom);

};

#endif // !defined(NAVSCAN_H)
