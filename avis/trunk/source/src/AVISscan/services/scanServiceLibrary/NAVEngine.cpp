// NAVEngine.cpp: implementation of the NAVEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <log\cfgprof.h>

#include "NAVEngine.h"
#include "WinRegistry.h"
#include "Scan.h"

#include "scanserviceglobals.h"
#include "ScanService.h"
#include "ScanException.h"
#include "Logger.h"
#include "TraceSinkFile.h"

using namespace std;


class TraceLife
{
public:
  TraceLife() : traceSink(GetLogName().c_str()),
                mTrace(traceSink, MODE_MAX, MTrace::tv_exhaustive, ",") 
  {
    Logger::SendTo(&mTrace);
    Logger::ApplicationName("ScanService");
    Logger::SeverityFilter(severity);
    Logger::SourceFilter(sourceFilter);
  };

  ~TraceLife() { Logger::SendTo(NULL);/* delete traceSink; delete mTrace; */ }

private:
  Logger::LogSeverity	severity;
  Logger::LogSource	sourceFilter;
  TraceSinkFile		traceSink;
  MTrace				mTrace;

  string	GetLogName() 
  {
    string	traceFileName("c:\\temp\\ScanService");
    NAVEngine::GetTraceParameters(severity, sourceFilter, traceFileName);

    return traceFileName;
  }
};

const char*		NAVEngine::registryRoot			= "Software\\Symantec\\Quarantine\\Server\\Avis\\current";
const char*		NAVEngine::registryRootUpOne	= "Software\\Symantec\\Quarantine\\Server\\Avis";


static TraceLife	traceLife;

NAVEngine::Status NAVEngine::status		= NAVEngine::NotInitialized;
uint		NAVEngine::seqNum			= 0;
HNAVENGINE	NAVEngine::navEngineHandle	= NULL;
LONG		NAVEngine::useCount			= 0L;
string		NAVEngine::defsDir			= GetDefsDir();
WORD		NAVEngine::heuristicLevel	= GetHeuristicLevel();




	//
	//	NAV File I/O Callbacks

BOOL	NAVCALLBACK ioOpen(LPVOID lpvFileInfo, DWORD dwOpenMode, LPVOID FAR *lplpvHandle);
BOOL	NAVCALLBACK ioClose(LPVOID dwHandle);
DWORD	NAVCALLBACK ioSeek(LPVOID dwHandle, LONG lOffset, int nFrom);
UINT	NAVCALLBACK ioRead(LPVOID dwHandle, LPVOID lpvBuffer, UINT nBytesToRead);
UINT	NAVCALLBACK ioWrite(LPVOID dwHandle, LPVOID lpvBuffer, UINT nBytesToWrite);
BOOL	NAVCALLBACK ioDelete(LPVOID lpvFileInfo);
BOOL	NAVCALLBACK ioGetDateTime(LPVOID dwHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime);
BOOL	NAVCALLBACK ioSetDateTime(LPVOID dwHandle, UINT uType, WORD wDate, WORD wTime);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NAVEngine::NAVEngine()
{
  // you shouldn't be able to create one of these!

  throw ScanException(ScanException::NavEngineConstructor,
                      "Trying to construct a NAVEngine object.  NAVEngine is a singleton class so this is not allowed");
}

NAVEngine::~NAVEngine()
{

}


bool NAVEngine::Open(uint sigSeqNum)
{
  EntryExit	entryExit(Logger::LogAvisScanner, "NAVEngine::Open");

  if (seqNum == sigSeqNum)
    return true;
  else if (0 != seqNum)
    {
      if (!Close())
        {
          status = CannotClose;
          return false;
        }
      seqNum = 0;
    }

  std::string	signatures(defsDir);
  char		buffer[64];
  sprintf(buffer, "\\%08d", sigSeqNum);
  signatures	+= buffer;

  // Log the complete pathname of the directory we are about to load
  // definitions from.

  {
    string	msg("Loading definitions from directory ");
    msg	+= signatures;
    Logger::Log(Logger::LogDebug, Logger::LogAvisScanner, msg.c_str());
  }

  DWORD	attrs	= GetFileAttributes(signatures.c_str());
  if (0xFFFFFFFF == attrs ||					// directory does not exist
      !(FILE_ATTRIBUTE_DIRECTORY & attrs))	// not a directory
    {
      char	buffer[1024];
      sprintf(buffer, "NAVEngine::Open, Def path (%s) invalid, attrs = %x", signatures.c_str(), attrs);

      Logger::Log(Logger::LogError, Logger::LogAvisScanner, buffer);

      status = ParticularDefPathInvalid;
      return false;
    }

  NAVFILEIO	callbacks = {	&ioOpen, &ioClose, &ioSeek, &ioRead, &ioWrite, &ioDelete,
                                &ioGetDateTime, &ioSetDateTime };

  NAVSTATUS	navStatus;

  try
    {
      navEngineHandle = NAVEngineInit((char *) signatures.c_str(), NULL, &callbacks,
                                      heuristicLevel, 0, &navStatus);
    }
  catch (...)
    {
      // Terminate the process immediately if NAVEngineInit throws an exception.
      navEngineHandle	= NULL;
      Logger::Log(Logger::LogInfo, Logger::LogAvisScanner,
                  "NAVEngineInit threw an exception of unknown type");
      Logger::Log(Logger::LogCriticalError,Logger::LogAvisScanner,
                  "exiting after catching NAVEngineInit exception");
      ScanServiceGlobals::stopService = true;
      exit(1);
    }

  switch (navStatus)
    {
    case NAV_OK:
      status = Okay;
      seqNum = sigSeqNum;

      char	buffer[128];
      sprintf(buffer, "Successfully called NAVEngineInit with seqNum = %d", sigSeqNum);
      Logger::Log(Logger::LogInfo, Logger::LogAvisScanner, buffer);
      break;

    case NAV_MEMORY_ERROR:
      status = NAVLoadDefFailure;
      Logger::Log(Logger::LogError, Logger::LogAvisScanner, 
                  "NAVEngine::Open, NAVEngineInit failed due to a memory error");

      break;

    case NAV_ENGINE_IN_USE:
      status = NAVEngineInUse;
      Logger::Log(Logger::LogError, Logger::LogAvisScanner, 
                  "NAVEngine::Open NAVEngineInit failed because it is already in use (should never happen!)");
      break;

    case NAV_INVALID_ARG:
      status = NAVLoadDefFailure;
      Logger::Log(Logger::LogError, Logger::LogAvisScanner, 
                  "NAVEngine::Open, NAVEngineInit failed because of an invalid argument");
      break;

    case NAV_VXD_INIT_FAILURE:
      status = NAVVXDInitFailure;
      Logger::Log(Logger::LogError, Logger::LogAvisScanner, 
                  "NAVEngine::Open, NAVEngineInit failed because of a VXD init failure");
      break;

    case NAV_DEFINITIONS_ERROR:
      status = NAVDefinitionsError;
      Logger::Log(Logger::LogError, Logger::LogAvisScanner, 
                  "NAVEngine::Open, NAVEngineInit failed because the definitions set is incomplete or failed to load");
      break;

    default:
      status = UnknownError;
      Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                  "NAVEngine::Open, NAVEngineInit failed with an unknown return code");
    }

  return NULL != navEngineHandle;
}

bool NAVEngine::Close()
{
  EntryExit	entryExit(Logger::LogAvisScanner, "NAVEngine::Close");

  bool	rc	= false;

  if (0 == useCount)
    {
      if (0 != seqNum)
        {
          NAVSTATUS navRC;
			
          try
            {
              navRC = NAVEngineClose(navEngineHandle);
            }
          catch (...)
            {
	      // Terminate the process immediately if NAVEngineClose throws an exception.
              Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                          "NAVEngineClose threw an exception of unknown type");
	      Logger::Log(Logger::LogCriticalError,Logger::LogAvisScanner,
			  "exiting after catching NAVEngineClose exception");
	      ScanServiceGlobals::stopService = true;
	      exit(1);
            }

          if (NAV_OK == navRC)
            {
              rc				= true;
              seqNum			= 0;
              navEngineHandle	= NULL;

              Logger::Log(Logger::LogDebug, Logger::LogAvisScanner,
                          "NAVEngine::Close, NAVEngineClose succeeded");
            }
          else if (NAV_INVALID_ARG == navRC)
            {
              status	= NAVHandleInvalid;

              Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                          "NAVEngine::Close, NAVEngineClose failed because the handle was invalid");
            }
          else if (NAV_ERROR == navRC)
            {
              status	= NAVLoadDefFailure;

              Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                          "NAVEngine::Close, NAVEngineClose failed because of an unspecified NAV Error");
            }
          else
            {
              status	= NAVLoadDefFailure;

              Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                          "NAVEngine::Close, NAVEngineClose failed with an unknown return code");
            }
        }
      else
        rc = true;
    }

  return rc;
}

std::string	NAVEngine::DefVersion()
{
  EntryExit	entryExit(Logger::LogAvisScanner, "NAVEngine::DefVersion");

  std::string			defVersion("00000000.000");
  NAVVIRUSDBINFOEX	info	= { 0 };
  info.dwSize	= sizeof(NAVVIRUSDBINFOEX);

  NAVSTATUS			status	= NAV_ERROR;

  try
    {
      status = NAVGetVirusDBInfoEx(navEngineHandle, &info);
    }
  catch (...)
    {
      // Terminate the process immediately if NAVGetVirusDBInfoEx throws an exception.
      Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                  "NAVGetVirusDBInfoEx threw an exception of unknown type");
      Logger::Log(Logger::LogCriticalError,Logger::LogAvisScanner,
                  "exiting after catching NAVGetVirusDBInfoEx exception");
      ScanServiceGlobals::stopService = true;
      exit(1);
    }

  if (NAV_OK == status)
    {
      char	buffer[20];

      sprintf(buffer, "%04d%02d%02d.%03d", info.wYear, info.wMonth,
              info.wDay, info.dwVersion);
      defVersion = buffer;

      Logger::Log(Logger::LogInfo, Logger::LogAvisScanner,
                  "NAVEngine::DefVersion, NAVGetVirusDBInfoEx suceeded");
    }
  else
    Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                "NAVEngine::DefVersion, NAVGetVirusDBInfoEx failed");

  return defVersion;
}

std::string	NAVEngine::GetDefsDir()
{
  EntryExit	entryExit(Logger::LogAvisScanner, "NAVEngine::GetDefsDir");

  std::string	dir("");

  WinRegistry	root(HKEY_LOCAL_MACHINE, registryRoot);

  // check if Registry key is valid
  if (root.ValidKey())
    { // if, key valid

      char	buffer[256];
      ulong	size	= 256;

      // get the value for the Definitions Directory
      if (root.QueryValue("definitionLibraryDirectory", (uchar*)buffer, size))
        { // if, yes got value for Definition value

          dir = buffer;

          Logger::Log(Logger::LogDebug, Logger::LogAvisScanner,
                      "NAVEngine::GetDefsDir, found definitions directory in registry");
        } // end if

    } // end if, key valid
  else
    { // else, look up one level to AVIS

      WinRegistry	rootUpOne(HKEY_LOCAL_MACHINE, registryRootUpOne);
		
      if (rootUpOne.ValidKey())
        { // if, yes valid key

          char	buffer[256];
          ulong	size	= 256;

          // get the value for the Definitions Directory
          if (rootUpOne.QueryValue("definitionLibraryDirectory", (uchar*)buffer, size))
            { // if, yes got value

              dir = buffer;

              Logger::Log(Logger::LogDebug, Logger::LogAvisScanner,
                          "NAVEngine::GetDefsDir, found definitions directory in registry");

            } // end if	

        } // end if, yes valid key
      else
        { // else, not in Registry, look for "Profile" File

          try
            { // try

              ConfigProfile	profile("AVISscan.prf");

              if (profile.validObject())
                { // if, yes found profile file

                  dir = profile["definitionLibraryDirectory"];

                  Logger::Log(Logger::LogDebug, Logger::LogAvisScanner,
                              "NAVEngine::GetDefsDir, found definitions directory in profile");

                } // end if

            } // end try
          catch (...)
            { // catch
              ;
            } // end catch

        } // end else, not in Registry, look for "Profile" File

    } // end else, look up one level to AVIS

  // see if found Definition directory in Registry or profile file
  if ("" == dir)
    { // if, not found,last chance look off current directory

      // ckf
      char	cBuffer[256];
      DWORD   dwBufferLength = 256;

      //
      // get the current working directory
      //
      GetCurrentDirectory( dwBufferLength, cBuffer );

      strcat( cBuffer, "\\signatures" );

      dir = cBuffer;

      //	CKF 	dir = "C:\\Program Files\\Symantec\\Quarantine\\Server\\signatures";

      Logger::Log(Logger::LogWarning, Logger::LogAvisScanner,
                  "definition file base directory not found in either the registry or profile file, setting to default of current dir\\signatures");
    }

  DWORD	attrs = GetFileAttributes(dir.c_str());
		
  if (0xFFFFFFFF == attrs ||					// directory does not exist
      !(FILE_ATTRIBUTE_DIRECTORY & attrs))// not a directory
    {
      Logger::Log(Logger::LogError, Logger::LogAvisScanner, "defs directory invalid");
      status = DefsPathInvalid;
      dir = "";
    }
  else
    {
      string	msg("Definitions directory found to be ");
      msg	+= dir;

      Logger::Log(Logger::LogInfo, Logger::LogAvisScanner, msg.c_str());
    }

  return dir;
}
WORD NAVEngine::GetHeuristicLevel()
{
  EntryExit	entryExit(Logger::LogAvisScanner, "NAVEngine::GetHeuristicLevel");

  WORD		hL = 3;		// set to high if not defined;

  WinRegistry	root(HKEY_LOCAL_MACHINE, registryRoot);
		
  if (root.ValidKey())
    {
      DWORD	value	= 2;
      char	asChar[20];
      ulong	size	= 20;

      if (root.QueryValue("definitionHeuristicLevel", (uchar *) asChar, size))
        {
          sscanf(asChar, "%d", &value);
          hL = value;

          Logger::Log(Logger::LogDebug, Logger::LogAvisScanner,
                      "Heuristic level found in registry");
        }
    }
  else
    {
      try
        {
          ConfigProfile	profile("AVISscan.prf");

          if (profile.validObject())
            {
              string	hl = profile["definitionHeuristicLevel"];
              hL	= atoi(hl.c_str());

              Logger::Log(Logger::LogDebug, Logger::LogAvisScanner,
                          "Heuristic level found in profile file");
            }
        }
      catch (...)
        {
          hL = 2;
        }
    }

  char	buffer[128];
  sprintf(buffer, "Heuristic level found to be %d", hL);

  Logger::Log(Logger::LogInfo, Logger::LogAvisScanner, buffer);

  return hL;
}

void NAVEngine::GetTraceParameters( Logger::LogSeverity& severity, 
                                    Logger::LogSource& filter,
                                    string& traceFileName)
{
  severity = Logger::LogWarning;
  filter	 = Logger::LogNone;

  char	cBuffer[256];
  DWORD   dwBufferLength =  256;

  // get the current directory
  GetCurrentDirectory( dwBufferLength, cBuffer );

  strcat( cBuffer, "\\avistrace.txt" );

  traceFileName = cBuffer;

  // CKF	traceFileName = "C:\\Program Files\\Symantec\\Quarantine\\Server\\avistrace.txt";

  // use the  CURRENT key
  WinRegistry	root(HKEY_LOCAL_MACHINE, registryRoot);
	
  // check if that key is in the Registry
  if (root.ValidKey())
    { // if, use this key 

      uchar	buffer[512];
      DWORD	length(512);

      if (root.QueryValue("traceFilename", buffer, length))
        { 
          traceFileName = (char*) buffer;
        }

      length = 512;
      if (root.QueryValue("traceSeverity", buffer, length))
        {
          severity = Logger::SeverityFromChar((const char*) buffer);
        }

      length = 512;
      if (root.QueryValue("traceFilter", buffer, length))
        {
          filter = (Logger::LogSource) atoi((const char*) buffer);
        }

    } // end if, use this key
  else
    { 

      // use the AVIS key, i.e. up one
      WinRegistry	rootUpOne(HKEY_LOCAL_MACHINE, registryRootUpOne);
	
      // check if that key is in the Registry
      if (rootUpOne.ValidKey())
        { // if, use this key 

          uchar	buffer[512];
          DWORD	length(512);

          if (rootUpOne.QueryValue("traceFilename", buffer, length))
            { 
              traceFileName = (char*) buffer;
            }

          length = 512;
          if (rootUpOne.QueryValue("traceSeverity", buffer, length))
            {
              severity = Logger::SeverityFromChar((const char*) buffer);
            }

          length = 512;
          if (rootUpOne.QueryValue("traceFilter", buffer, length))
            {	
              filter = (Logger::LogSource) atoi((const char*) buffer);
            }

        }
      else
        { // else, try to find it in the profile file
          try
            { // try

              ConfigProfile	profile("AVISscan.prf");

              if (profile.validObject())
                { // if

                  try 
                    {  // try

                      traceFileName	= profile["traceFileName"]; 
                    } // end try
                  catch (...) 
                    { // catch
					
                      char	cBuffer[256];
                      DWORD   dwBufferLength =  256;

                      GetCurrentDirectory( dwBufferLength, cBuffer );

                      strcat( cBuffer, "\\avistrace.txt" );

                      traceFileName =  cBuffer;

                      //					traceFileName = "C:\\Program Files\\Symantec\\Quarantine\\Server\\avistrace.txt"; 

                    } // end catch
				
                  try 
                    { 
                      severity	= Logger::SeverityFromChar(profile["traceSeverity"].c_str());
                    }
                  catch (...) 
                    { 
                      severity = Logger::LogWarning; 
                    }
				
                  try 
                    { 
                      filter = (Logger::LogSource) atoi(profile["traceFilter"].c_str()); 
                    }
                  catch (...) 
                    { 
                      filter = Logger::LogNone; 
                    }

                  try 
                    { 
                      if (0 == atoi(profile["traceEnable"].c_str())) 
                        filter = Logger::LogNone; 
                    }
                  catch (...) 
                    {
                    }

                } // end if

            } // end try
          catch (...)
            { // catch 
				// ckf
              char	cBuffer[256];
              DWORD   dwBufferLength =  256;

              GetCurrentDirectory( dwBufferLength, cBuffer );

              strcat( cBuffer, "\\avistrace.txt" );

              traceFileName	= cBuffer;
              severity		= Logger::LogWarning;
              filter			= Logger::LogNone;

            } // end catch

        } // end else, try to find it in the profile file

    } // end else
}

//
//	NAV File I/O Callbacks

BOOL	NAVCALLBACK ioOpen(LPVOID lpvFileInfo, DWORD dwOpenMode, LPVOID FAR *lplpvHandle)
{
  *lplpvHandle =  (void **) static_cast<Scan*>(lpvFileInfo)->ioOpen(dwOpenMode);
  return 1;
}


BOOL	NAVCALLBACK ioClose(LPVOID dwHandle)
{
  return reinterpret_cast<Scan*>(dwHandle)->ioClose();
}

DWORD	NAVCALLBACK ioSeek(LPVOID dwHandle, LONG lOffset, int nFrom)
{
  return reinterpret_cast<Scan*>(dwHandle)->ioSeek(lOffset, nFrom);
}

UINT	NAVCALLBACK ioRead(LPVOID dwHandle, LPVOID lpvBuffer, UINT nBytesToRead)
{
  return reinterpret_cast<Scan*>(dwHandle)->ioRead(lpvBuffer, nBytesToRead);
}

UINT	NAVCALLBACK ioWrite(LPVOID dwHandle, LPVOID lpvBuffer, UINT nBytesToWrite)
{
  return reinterpret_cast<Scan*>(dwHandle)->ioWrite(lpvBuffer, nBytesToWrite);
}

BOOL	NAVCALLBACK ioDelete(LPVOID lpvFileInfo)
{		
  return 0;
}

BOOL	NAVCALLBACK ioGetDateTime(LPVOID dwHandle, UINT uType, LPWORD lpwDate, LPWORD lpwTime)
{
  return 0;
}

BOOL	NAVCALLBACK ioSetDateTime(LPVOID dwHandle, UINT uType, WORD wDate, WORD wTime)
{
  return 0;
}


const char* NAVEngine::StatusToChar()
{
  switch (status)
    {
    case Okay:					return "everything is okay and ready";
    case NotInitialized:		return "the engine is not initialized";
    case CannotClose:			return "Unable to clean up from last run";
    case UnableToFindDefsPath:	return "Unable to find the path to the definition files";
    case DefsPathInvalid:		return "the path in the registry is not a valid path";
    case ParticularDefPathInvalid:	return "the directory for the requested sequence number does not exist";
    case NAVHandleInvalid:		return "this should not be possible";
      //	case UnSpecifiedNAVerror:	return "nav returned \"error\"";
      //	case NAVMemoryError:		return "nav returned memory allocation error";
    case NAVEngineInUse:		return "nav returned engine in use";
    case NAVLoadDefFailure:		return "nav failed trying to load definition set";
      //	case NAVInvalidArg:			return "nav returned invalid argument";
    case NAVVXDInitFailure:		return "nav returned navapi.vxd failed to initialize";
    case NAVDefinitionsError:	return "nav returned the definitions set in incomplete or failed to load";
    case NAVDefLoadException:	return "nav threw an exception while trying to open a definition set";
      //	case UnknownNAVerror:		return "nav returned an unknown code";
    case SystemError:			return "system ipc/file call failed, should never happen";
    case BadScan:				return "something happended to prevent the scanning of the file";
    case BadRepair:				return "something happended to prevent the repair of the file";
    case CouldntChangeDef:		return "couldnt change active definition set";
    case UnknownError:			return "could be a NAV error, could be a system error, something threw an exception";
    }

  return "Unknow status state";
}
