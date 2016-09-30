// NAVEngine.h: interface for the NAVEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVENGINE_H__2D9CC65F_7E60_11D2_ACEA_00A0C9C71BBC__INCLUDED_)
#define AFX_NAVENGINE_H__2D9CC65F_7E60_11D2_ACEA_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AVIS.h>
#include <Logger.h>


#define	SYM_WIN32	1
#include <NAVapi.h>

class NAVEngine  
{
private:
  NAVEngine();
  virtual ~NAVEngine();

public:
  enum Status {	Okay,					// everything is okay and ready
                NotInitialized,			// the engine is not initialized
                CannotClose,			// Unable to clean up from last run
                UnableToFindDefsPath,	// Unable to find the path to the
                // definition files
                DefsPathInvalid,		// the path in the registry is
                // not a valid path
                ParticularDefPathInvalid,//the directory for the requested
                // sequence number does not exist
                NAVHandleInvalid,		// this should not be possible
                //					UnSpecifiedNAVerror,	// nav returned "error"
                //					NAVMemoryError,			// nav returned memory allocation error
                NAVEngineInUse,			// nav returned engine in use
                NAVLoadDefFailure,		// nav failed trying to load definition set
                //					NAVInvalidArg,			// nav returned invalid argument
                NAVVXDInitFailure,		// nav returned navapi.vxd failed to initialize
                NAVDefinitionsError,	// nav returned the definitions set in incomplete or failed to load
                NAVDefLoadException,	// nav threw an exception while trying to open a definition set
                //					UnknownNAVerror,		// nav returned an unknown code
                SystemError,			// system ipc/file call failed, should never happen
                BadScan,				// something happened to prevent the scanning of the file
                BadRepair,				// something happened to prevent the repairing of the file
                CouldntChangeDef,		// unable to change definition file (probably because
                // an in progress scan did not complete in time).
                UnknownError			// could be a NAV error, could be a system error, don't know
  };

  static const char* StatusToChar();

  // Open a new signature definition file.
  //	If the engine is already opened will call close first
  static bool		Open(uint sigSeqNum);

  //	Closes the current signature definition file.
  //	Returns false if all of the users have not released
  //	their copy of the the NAVEngine handle.
  static bool		Close(void);

  //	Returns the current status, could be an error, could
  //	be okay.
  static Status	GetStatus(void) { return status; };
  static void 	SetStatus(Status s) { status = s; };

  //
  //	Returns the current signature sequence number this
  //	process is using.
  static uint		SeqNum(void) { return seqNum; };


  //
  //	Returns the current signature version
  static std::string	DefVersion(void);


  //
  //  Returns the current signature path this process is
  //  using.

  static std::string	SigPath(void) { return defsDir; }

  static	void		GetTraceParameters(Logger::LogSeverity& severity,
                                                   Logger::LogSource& filter,
                                                   std::string& traceFileName);

  static	const char*	registryRoot;		// where to look for registry values
  static	const char*	registryRootUpOne;	// where to look for registry values if not in CURRENT. CKF

private:
  static	Status		status;				// status of the NAV engine
  static	uint		seqNum;				// the current sequence number
  static	HNAVENGINE	navEngineHandle;	// the current nav engine handle
  static	LONG		useCount;			// how many people using handle
  static	std::string	defsDir;			// root of the definition file tree
  static	WORD		heuristicLevel;		// how agressively to scan


  static	std::string	GetDefsDir(void);
  static	WORD		GetHeuristicLevel(void);


  static	HNAVENGINE	GetNavEngHandle(void)		// increment counter by one
  {	InterlockedIncrement(&useCount);
  return navEngineHandle;		}
  static	void		ReturnNavEngHandle(void)	// decrement counter by one
  {	InterlockedDecrement(&useCount); }

  friend class NavEngineHandle;
};


class NavEngineHandle
{
public:
  NavEngineHandle() :	handle(NAVEngine::GetNavEngHandle()) {};
  ~NavEngineHandle()	{ NAVEngine::ReturnNavEngHandle(); };

  operator HNAVENGINE(void) { return handle; };

private:
  HNAVENGINE	handle;
};

#endif // !defined(AFX_NAVENGINE_H__2D9CC65F_7E60_11D2_ACEA_00A0C9C71BBC__INCLUDED_)
