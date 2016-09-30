// Scan.cpp: implementation of the Scan class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <iostream>

#include "NAVScan.h"

#include "NavEngine.h"

#include "Scan.h"
#include "ScanService.h"
#include "WaitToScan.h"
#include "scanserviceglobals.h"
//#include "ScanMonitor.h"

#include <MappedMemory.h>

#include <Logger.h>
#include <ExceptionTyper.h>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Scan::~Scan()
{

}

NAVScan::ScanRC	ConvertAndLogScanRC(NAVSTATUS status, HNAVVIRUS virusInfo, const char* fileToScan)
{
  NAVScan::ScanRC	rc	= NAVScan::BadScan;
  string			msg("NAVScanFile(");
  msg	+= fileToScan;
  msg += ") returning ";

  switch (status)
    {
    case NAV_OK:
      if (NULL == virusInfo)
        {
          rc = NAVScan::NotInfected;
          msg += "not infected";
          Logger::Log(Logger::LogInfo, Logger::LogAvisScanner, msg.c_str());
        }
      else
        {
          rc = NAVScan::Infected;
          msg	+= "infected";
          Logger::Log(Logger::LogInfo, Logger::LogAvisScanner, msg.c_str());
        }

      break;

    case NAV_NO_ACCESS:
      msg += "error \"NAV_NO_ACCESS\"";
      Logger::Log(Logger::LogError, Logger::LogAvisScanner, msg.c_str());
      break;

    case NAV_ERROR:
      msg += "error \"NAV_ERROR\"";
      Logger::Log(Logger::LogError, Logger::LogAvisScanner, msg.c_str());
      break;

    case NAV_MEMORY_ERROR:
      msg += "error \"NAV_MEMORY_ERROR\"";
      Logger::Log(Logger::LogError, Logger::LogAvisScanner, msg.c_str());
      break;

    case NAV_INVALID_ARG:
      msg += "error \"NAV_INVALID_ARG\"";
      Logger::Log(Logger::LogError, Logger::LogAvisScanner, msg.c_str());
      break;

    default:
      msg += "unknown error code";
      Logger::Log(Logger::LogError, Logger::LogAvisScanner, msg.c_str());
    }

  return rc;
}

bool Scan::IsInfectable(NavEngineHandle& navHandle, std::string& fileName, std::string& extension)
{
  EntryExit	entryExit(Logger::LogAvisScanner, "Scan::IsInfectable");

  char	buffer[512];
  BOOL	isInfectable	= true;

  fileToScan = fileName;
  //	NavEngineHandle	navHandle;
  NAVSTATUS	status;
	
  try
    {
      status = NAVIsInfectableFile(navHandle, this, (char *) extension.c_str(),
                                   false, &isInfectable);
    }
  catch (...)
    {
      ExceptionHandler("call to NAVIsInfectableFile");
      Logger::Log(Logger::LogCriticalError,Logger::LogAvisScanner,
		  "exiting after catching NAVIsInfectableFile exception");
      ScanServiceGlobals::stopService = true;
      exit(1);
    }

  switch (status)
    {
    case NAV_OK:
      sprintf(buffer, "NAVIsInfectableFile returned NAV_OK, infectable = %s",
              (isInfectable ? "True" : "False"));
      Logger::Log(Logger::LogDebug, Logger::LogAvisScanner, buffer);
      break;

    case NAV_MEMORY_ERROR:
      Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                  "Scan::IsInfectable, NAVIsInfectableFile error \"NAV_MEMORY_ERROR\"");
      isInfectable = true;
      break;

    case NAV_INVALID_ARG:
      Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                  "Scan::IsInfectable, NAVsInfectableFile error \"NAV_INVLAID_ARG\"");
      isInfectable = true;
      break;

    case NAV_NO_ACCESS:
      Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                  "Scan::IsInfectable, NAVsInfectableFile error \"NAV_NO_ACCESS\"");
      isInfectable = true;
      break;

    case NAV_ERROR:
      Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                  "Scan::IsInfectable, NAVsInfectableFile error \"NAV_CANT_ERROR\"");
      isInfectable = true;
      break;
    }

  return 0 != isInfectable;
}

static string	GetLastErrorAsStr()
{
  char	msg[512];

  if (0 >= FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL,
                         GetLastError(), 0, msg, 512, NULL))
    strcpy(msg, "No message from system");

  return string(msg);
}


inline	float	CalcSeconds(ulong deltaClockTicks)
{
  return ((float)deltaClockTicks)/((float) CLOCKS_PER_SEC);
}

NAVScan::ScanRC Scan::RepairFile(NavEngineHandle& navHandle, std::string& fileName,
                                 std::string& extension)
{
  EntryExit	entryExit(Logger::LogAvisScanner, "Scan::RepairFile");

  NAVScan::ScanRC	rc	= NAVScan::RepairTriedAndError;

  int				dot = fileName.find_last_of(".");
  int				slash=fileName.find_last_of("\\");
  std::string		copyName	= fileName.substr(slash+1, dot - slash - 1);
  std::string		mappedPrefix("\\\\.\\mapped\\");

#ifdef LOG_TIME
  clock_t			tmpClock		= clock();
  ulong			dupFileTime;
  ulong			repairFileTime;
#endif

  overflowed	= false;
  copyName	+= "-cpy";
  copyName	+= extension;

  padding		= InitPaddingSize;
  MappedMemory	fileCopy(copyName, fileSize + sizeof(ulong) + padding);
  if (mappedFile)
    {
      string	mappedName = fileToScan.substr(mappedPrefix.size(), fileToScan.size());
      fileHandle = OpenFileMapping(FILE_MAP_READ, FALSE, mappedName.c_str());
      if (0 == fileHandle)
        {
          string	errMsg("OpenFileMappingFailed [");
          errMsg	+= GetLastErrorAsStr();
          errMsg	+= "]";
          Logger::Log(Logger::LogCriticalError, Logger::LogAvisScanner, errMsg.c_str());

          return NAVScan::RepairTriedAndError;
        }
      else
        {
          mappedPointer	= (uchar *) MapViewOfFile(fileHandle, FILE_MAP_READ, 0, 0, 0);
          if (0 == mappedPointer)
            {
              string	errMsg("MapView of File failed [");
              errMsg	+= GetLastErrorAsStr();
              errMsg	+= "]";
              Logger::Log(Logger::LogCriticalError, Logger::LogAvisScanner, errMsg.c_str());

              return NAVScan::RepairTriedAndError;
            }
          else
            {
              memset(fileCopy.Begining(), 0, fileSize+padding);
              memcpy(fileCopy.Begining(), &mappedPointer[sizeof(ulong)], fileSize);
              ulong*	ptr	= (ulong *) fileCopy.Begining();
              --ptr;
              *ptr	=	fileSize;

              UnmapViewOfFile(mappedPointer);
            }
          ::CloseHandle(fileHandle);
        }
    }
  else								
    {
      fileHandle	= CreateFile((LPCTSTR) fileToScan.c_str(),
                                     GENERIC_READ, FILE_SHARE_READ, NULL,
                                     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

      if (INVALID_HANDLE_VALUE != fileHandle)
        {
          memset(fileCopy.Begining(), 0, fileSize+padding);

          uchar	*tmp = fileCopy.Begining();
          tmp	-= sizeof(ulong);
          *((ulong *) tmp) = fileSize;
          DWORD	bytesRead	= fileSize;
          if (!ReadFile(fileHandle, fileCopy.Begining(), fileSize, &bytesRead, NULL) ||
              bytesRead != fileSize)
            {
              CloseHandle(fileHandle);

              string	errMsg("ReadFile failed [");
              errMsg	+= GetLastErrorAsStr();
              errMsg	+= "]";
              Logger::Log(Logger::LogCriticalError, Logger::LogAvisScanner, errMsg.c_str());

              return NAVScan::RepairTriedAndError;
            }

          CloseHandle(fileHandle);
        }
    }
  fileToScan = mappedPrefix;
  fileToScan += copyName;
  writeAccess	= true;

#ifdef LOG_TIME
  dupFileTime	= clock() - tmpClock;
  tmpClock	= clock();
#endif

  try
    {
      repairInitialized	= false;
      NAVSTATUS	status = NAVRepairFile(navHandle, this, (char *) extension.c_str());

#ifdef LOG_TIME
      repairFileTime = clock() - tmpClock;
#endif

      writeAccess			= false;
      repairInitialized	= false;
      ioClose();

      switch (status)
        {
        case NAV_OK:
          rc = NAVScan::Repaired;
          Logger::Log(Logger::LogInfo, Logger::LogAvisScanner,
                      "Scan::RepairFile, file was repaired by NAV");
          break;

        case NAV_CANT_REPAIR:
          rc = NAVScan::RepairTriedAndFailed;
          Logger::Log(Logger::LogInfo, Logger::LogAvisScanner,
                      "Scan::RepairFile, NAV tried to repair the file and failed");
          break;

        case NAV_MEMORY_ERROR:
          Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                      "Scan::RepairFile, NAVRepairFile error \"NAV_MEMORY_ERROR\"");
          break;

        case NAV_INVALID_ARG:
          Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                      "Scan::RepairFile, NAVRepairFile error \"NAV_INVLAID_ARG\"");
          break;

        case NAV_NO_ACCESS:
          Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                      "Scan::RepairFile, NAVRepairFile error \"NAV_NO_ACCESS\"");
          break;

        case NAV_ERROR:
          Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                      "Scan::RepairFile, NAVRepairFile error \"NAV_CANT_ERROR\"");
          break;

        default :
          Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                      "Scan::RepairFile, NAVRepairFile unknown error return code");
        }
    }
  catch (...)
    {
      ExceptionHandler("call to NAVRepairFile");
      Logger::Log(Logger::LogCriticalError,Logger::LogAvisScanner,
		  "exiting after catching NAVRepairFile exception");
      ScanServiceGlobals::stopService = true;
      exit(1);
    }

#ifdef LOG_TIME
  if (CalcSeconds(repairFileTime + dupFileTime) > 30.0)
    {
      char	tmpBuf[512];
      sprintf(tmpBuf, "dupFileTime = %f, repairFileTime = %f",
              CalcSeconds(dupFileTime), CalcSeconds(repairFileTime));
      Logger::Log(Logger::LogError, Logger::LogAvisScanner, tmpBuf);
    }
#endif

  return rc;
}

DWORD Scan::GetVirusInfo(HNAVVIRUS virusInfo, NAVVIRINFO flag, char* data, DWORD* length)
{
  DWORD	rc = -1;

  try
    {
      rc = NAVGetVirusInfo(virusInfo, flag, data, length);
    }
  catch (...)
    {
      ExceptionHandler("call to NAVGetVirusInfo");
      Logger::Log(Logger::LogCriticalError,Logger::LogAvisScanner,
		  "exiting after catching NAVGetVirusInfo exception");
      ScanServiceGlobals::stopService = true;
      exit(1);
    }

  return rc;
}

NAVSTATUS Scan::NavFreeVirusHandle(HNAVVIRUS& hVirus)
{
  try
    {
      NAVSTATUS	rc = NAVFreeVirusHandle(hVirus);
      hVirus	= 0;
      return rc;
    }
  catch (...)
    {
      ExceptionHandler("call to NAVFreeVirusHandle");
      Logger::Log(Logger::LogCriticalError,Logger::LogAvisScanner,
		  "exiting after catching NAVFreeVirusHandle exception");
      ScanServiceGlobals::stopService = true;
      exit(1);
    }
}

string TaggedMsg(uint clockTicks, const char* tag)
{
  string	msg("");
  float	secs = CalcSeconds(clockTicks);
  if (secs >= .001)
    {
      char	toChar[20];
      msg	+= tag;
      msg += " = ";
      sprintf(toChar, "%.3f, ", secs);
      msg += toChar;
    }

  return msg;
}


#define maxVirusNameLength	512

bool Scan::ScanFile(char* readBuffer, DWORD bytesRead)
{
  EntryExit	entryExit(Logger::LogAvisScanner, "Scan::ScanFile");

  Logger::Log(Logger::LogInfo, Logger::LogAvisScanner, readBuffer);

  //	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

  clock_t	scanOnlyStart		= clock();
  clock_t tmpClock;

  ulong	waitToScanTime		= 0;
  ulong	isInfectableTime	= 0;
  ulong	scanFileTime		= 0;
  ulong	virusInfoTime1		= 0;
  ulong	virusInfoTime2		= 0;
  ulong	virusInfoTime3		= 0;
  ulong	virusInfoTime4		= 0;
  ulong	repairTime			= 0;
  ulong	writeToPipeTime		= 0;


  char	virusName[maxVirusNameLength];
  char	trueFalse;
  char	fileName[ScanService::maxMessageSize];
  bool	wasRepaired			= false;
  bool	infectable			= true;
  BOOL	foundByHueristic	= false;	
  //DWORD	virusID				= 0;
  char	virusID[100];

  sscanf(readBuffer, "scan, %u, %u, %u, %c, %u, %s",
         &messageID, &returnID, &localCookie, &trueFalse,
         &sigSeqNum, fileName);
  fileToScan		= std::string(fileName);
  writeAccess		= false;
  
  // Log the complete pathname of the file we are about to scan, and
  // the sequence number of the definions we will be scanning it with.

  {
    char buffer[1000];
    sprintf(buffer, "Scanning file %s with defs %08d", fileName, sigSeqNum);
    Logger::Log(Logger::LogDebug, Logger::LogAvisScanner, buffer);
  }

  int				dot			= fileToScan.find_last_of(".");
  std::string		extension	= fileToScan.substr(dot, fileToScan.size());
  NAVScan::ScanRC	rc			= NAVScan::UnknownError;
  HNAVVIRUS		virusInfo	= 0;

  std::string		defVersion("00000000.000");

  try
    {
      tmpClock		= clock();
      WaitToScan		wToScan(sigSeqNum);		// throws ScanException
      waitToScanTime	= clock() - tmpClock;

      defVersion = NAVEngine::DefVersion();

      NavEngineHandle	navHandle;
      NAVSTATUS		status;

      tmpClock	= clock();
      infectable	= IsInfectable(navHandle, std::string(fileName), extension);
      isInfectableTime=clock() - tmpClock;

      if (infectable)
        {
          Logger::Log(Logger::LogInfo, Logger::LogAvisScanner,
                      "Sample is infectable");

          try
            {
              tmpClock	= clock();			
              status = NAVScanFile(navHandle, this, (char*) extension.c_str(),
                                   false, &virusInfo);
              scanFileTime= clock() - tmpClock;

              rc = ConvertAndLogScanRC(status, virusInfo, fileToScan.c_str());
            }
          catch (...)
            {
              ExceptionHandler("call to NAVScanFile");
              Logger::Log(Logger::LogCriticalError,Logger::LogAvisScanner,
                          "exiting after catching NAVScanFile exception");
              ScanServiceGlobals::stopService = true;
              exit(1);
            }

          if (NAVScan::Infected == rc)		// try to repair the file
            {
              tmpClock = clock();
               if (GetVirusInfo(virusInfo, NAV_VI_BOOL_SUBMITABLE, NULL, 0))
                 {
                  virusInfoTime1 = tmpClock - clock();

                  tmpClock		= clock();
                  char	trueFalse[16];
                  DWORD	bufferSize = sizeof(virusID);
                  DWORD	vInfoRC	= GetVirusInfo(virusInfo, NAV_VI_VIRUS_ID,
                                               (char *)&virusID, &bufferSize);
                  virusInfoTime2	= tmpClock - clock();
                  if (NAV_OK == vInfoRC)
                    {
                      tmpClock	= clock();
                      foundByHueristic = GetVirusInfo(virusInfo, NAV_VI_BOOL_HEURISTIC,
                                                      NULL, 0);

                      bufferSize = maxVirusNameLength;
                      vInfoRC = GetVirusInfo(virusInfo, NAV_VI_VIRUS_NAME,
                                             virusName, &bufferSize);
                      virusInfoTime3	= clock() - tmpClock;
				
                      if (!foundByHueristic)
                        {
                          bufferSize	= sizeof(trueFalse);
                          tmpClock	= clock();
                          BOOL repairable = GetVirusInfo(virusInfo, NAV_VI_BOOL_REPAIRABLE,
                                                         trueFalse, &bufferSize);

                          NavFreeVirusHandle(virusInfo);
                          virusInfoTime4 = clock() - tmpClock;
                          if (repairable)
                            {
                              tmpClock	= clock();
                              rc	= RepairFile(navHandle, std::string(fileName), extension);
                              repairTime	= clock() - tmpClock;
                            }
                          else
                            rc	= NAVScan::NoRepairInstructions;
                        }
                      else
                        {
                          NavFreeVirusHandle(virusInfo);
                          rc = NAVScan::Heuristic;
                        }
                     }
                   else
                     {
                       NavFreeVirusHandle(virusInfo);
                       rc = NAVScan::UnSubmittable;
                     }
                }
              else
                {
                  NavFreeVirusHandle(virusInfo);
                  rc = NAVScan::UnSubmittable;
                }
            }
          else
            {
              NavFreeVirusHandle(virusInfo);
            }
        }
      else
        rc = NAVScan::NotInfectable;
    }
  catch (ScanException& se)
    {
      ExceptionHandler("Scan::ScanFile by catch(scanException)");

      if (ScanException::WaitToScan_DefLoad == se.Type())
        rc = NAVScan::NavLoadDefFileError;
      else if (ScanException::WaitToScan_TimedOut == se.Type())
        rc = NAVScan::WaitForScanTimedOut;
      else
        rc = NAVScan::ScannerServiceError;
    }
  catch (...)
    {
      ExceptionHandler("Scan::ScanFile by catch(...)");
      Logger::Log(Logger::LogCriticalError,Logger::LogAvisScanner,
		  "exiting after catching unknown exception in Scan::ScanFile");
      ScanServiceGlobals::stopService = true;
      exit(1);
    }

  if (0 != virusInfo)
    NavFreeVirusHandle(virusInfo);


  char	tmpBuffer[ScanService::maxMessageSize];
  sprintf(tmpBuffer, "%d, %d, %d, %d, ", messageID, returnID, sigSeqNum, rc);


  string	returnMsg = tmpBuffer;
  returnMsg	+= defVersion;

  if (NAVScan::InfectedState(rc))
    {
      sprintf(tmpBuffer, ", %s, ", virusID);
      returnMsg += tmpBuffer;
      returnMsg += virusName;
    }

  string	msg("Scan::ScanFile, return buffer from scanner [");
  msg	+= returnMsg;
  msg	+= "]";
  Logger::Log(Logger::LogInfo, Logger::LogAvisScanner, msg.c_str());
  msg	= "Scan::ScanFile, return code (in English) = ";
  msg	+= NAVScan::RCtoChar(rc);
  Logger::Log(Logger::LogInfo, Logger::LogAvisScanner, msg.c_str());

  tmpClock	= clock();
  DWORD	bytesWritten = 0;
  if (WriteFile((HANDLE)pipeHandle, returnMsg.c_str(), returnMsg.size()+1,
                &bytesWritten, &overlapped) == FALSE)
    {
      // If the pipe broke, we will disconnect
      if (ERROR_BROKEN_PIPE == GetLastError())
        {
          Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                      "Scan::ScanFile, Pipe broken before answer could be returned");
        }
      else if (ERROR_IO_PENDING == GetLastError())
        {
          // waiting for write to complete
          if (FALSE == GetOverlappedResult(pipeHandle, &overlapped,
                                           &bytesWritten, TRUE))	// block
            {
              Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                          "Scan::ScanFile, Write never completed");
            }
        }
    }

  writeToPipeTime	= clock() - tmpClock;

  ulong	scanOnlyTicks	= clock() - scanOnlyStart;
  static LONG	scanCount	= 0;
  InterlockedIncrement(&scanCount);

  //	ScanMonitor::TrackIntermediateTimes(scanOnlyTicks, waitToScanTime, isInfectableTime, scanFileTime,
  //										virusInfoTime1, virusInfoTime2, virusInfoTime3, virusInfoTime4,
  //										repairTime, writeToPipeTime);


  if (Logger::WillLog(Logger::LogDebug, Logger::LogAvisScanner))
    {
      char	asChar[20];
      string	ticksMsg("Scanning ");
      ticksMsg	+= fileName;
      ticksMsg	+= " took ";

      sprintf(asChar, "%.3f", CalcSeconds(scanOnlyTicks));
      ticksMsg	+= asChar;
      ticksMsg	+= " seconds [";
      ticksMsg	+= TaggedMsg(waitToScanTime, "change def");
      ticksMsg	+= TaggedMsg(isInfectableTime, "check infectable");
      ticksMsg	+= TaggedMsg(scanFileTime, "scanning");
      ticksMsg	+= TaggedMsg(virusInfoTime1, "check submitable");
      ticksMsg	+= TaggedMsg(virusInfoTime2, "get virus ID");
      ticksMsg	+= TaggedMsg(virusInfoTime3, "found by heuristic & get virus name");
      ticksMsg	+= TaggedMsg(virusInfoTime4, "check if repairable");
      ticksMsg	+= TaggedMsg(repairTime, "repairing");
      ticksMsg	+= TaggedMsg(writeToPipeTime, "write response to pipe");
      // remove trailing ,
      if (',' == ticksMsg[ticksMsg.length()-2])
        {
          ticksMsg.erase(ticksMsg.length()-2, 2);
        }
      ticksMsg	+= " ], def = ";
      sprintf(asChar, "%d", sigSeqNum);
      ticksMsg	+= asChar;

      Logger::Log(Logger::LogDebug, Logger::LogAvisScanner, ticksMsg.c_str());
    }

  return true;
}


bool Scan::QueryScan(char* readBuffer, DWORD bytesRead)
{
  EntryExit	entryExit(Logger::LogAvisScanner, "Scan::QueryScan");

  bool	rc	= false;
  char	returnBuffer[ScanService::maxMessageSize];

  if (!strncmp("query, seqNum", readBuffer, 13))
    {
      Logger::Log(Logger::LogInfo, Logger::LogAvisScanner,
                  "Scan::QueryScan, asking for the current sequence number");

      uint	seqNum	= NAVEngine::SeqNum();

      sprintf(returnBuffer, "current seqNum = %u", seqNum);
      rc	= true;
    }
  else if (!strncmp("query, sigPath", readBuffer, 14))
    {
      Logger::Log(Logger::LogInfo, Logger::LogAvisScanner,
                  "Scan::QueryScan, asking for the current definitions path");

      sprintf(returnBuffer, "current sigPath = [%s]",
              NAVEngine::SigPath().c_str());
    }
  else if (!strncmp("query, defVersion, sigSeqNum =", readBuffer, 30))
    {
      Logger::Log(Logger::LogInfo, Logger::LogAvisScanner,
                  "Scan::QueryScan, asking for defVersion of current or explict defs");

      uint	seqNum;
      if (ScanService::ScanExplicit == ScanService::GetServiceType())
        {
          sscanf(readBuffer, "query, defVersion, sigSeqNum = %d", &seqNum);
          try
            {
              WaitToScan	wToScan(seqNum);
              sprintf(returnBuffer, "current defVersion = [%s]",
                      NAVEngine::DefVersion().c_str());
            }
          catch (...)
            {
              strcpy(returnBuffer, "Invalid Sequence Number");
            }
        }
			
    }
  else
    {
      strcpy(returnBuffer, "Error, unknown request");

      Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                  "Scan::QueryScan, unknown request made");
    }

  std::string	returnMsg = returnBuffer;

  DWORD	bytesWritten = 0;
  if (WriteFile((HANDLE)pipeHandle, returnMsg.c_str(), returnMsg.size()+1,
                &bytesWritten, &overlapped) == FALSE)
    {
      // If the pipe broke, we will disconnect
      if (ERROR_BROKEN_PIPE == GetLastError())
        {
          Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                      "Scan::QueryScan, Pipe broken before answer could be returned");
        }
      else if (ERROR_IO_PENDING == GetLastError())
        {
          // waiting for write to complete
          if (FALSE == GetOverlappedResult(pipeHandle, &overlapped,
                                           &bytesWritten, TRUE))	// block
            {
              Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                          "Scan::ScanFile, Write never completed");
            }
        }
    }

  return rc;
}




unsigned Scan::ThreadHandlerProc()
{
  EntryExit	entryExit(Logger::LogAvisScanner, "Scan::ThreadHandlerProc");

  ticksAtStart	= clock();
  active			= true;

  ResetEvent(overlapEvent);
  memset(&overlapped, 0, sizeof(overlapped));
  overlapped.hEvent = overlapEvent;

  DWORD	bytesRead	= 0;
  char	readBuffer[ScanService::maxMessageSize];


  if (FALSE == ReadFile((HANDLE)pipeHandle, &readBuffer, ScanService::maxMessageSize,
                        &bytesRead, &overlapped))
    {
      DWORD	lastError = GetLastError();
      if (ERROR_BROKEN_PIPE == lastError)
        {
          bytesRead = 0;
        }
      else if (ERROR_IO_PENDING == lastError)	// this is what we expect
        {
          if (FALSE == GetOverlappedResult((HANDLE)pipeHandle, &overlapped, &bytesRead, TRUE))
            {
              bytesRead = 0;
            }
        }
      else									// unexpected error
        {
          bytesRead = 0;

          char	msg[512];

          if (0 >= FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL,
                                 lastError, 0, msg, 512, NULL))
            strcpy(msg, "No message from system");

          string	errMsg("Scan::ThreadHandlerProc, error reading pipe! (");
          errMsg	+= msg;
          errMsg	+= ") EXITING!";

          Logger::Log(Logger::LogError, Logger::LogAvisScanner, errMsg.c_str());
          exit(1);
        }
    }

  if (bytesRead)
    {
      if (!strncmp("scan", readBuffer, 4))
        {
          activeScanning	= true;
          ScanFile(readBuffer, bytesRead);
        }
      else if (!strncmp("query", readBuffer, 5))
        QueryScan(readBuffer, bytesRead);
      else
        {
          Logger::Log(Logger::LogError, Logger::LogAvisScanner,
                      "Scan::ThreadHandlerProc, unknown request received");
        }
    }

  DisconnectPipe();

#ifdef LOG_TIME
  //	active			= false;
  //	if (activeScanning)
  //	{
  //		activeScanning	= false;
  //		ScanMonitor::LogFinished(clock() - ticksAtStart);
  //	}
#endif

  return true;
}


void Scan::DisconnectPipe()
{
  EntryExit	entryExit(Logger::LogAvisScanner, "Scan::DisconnectPipe");

  DisconnectNamedPipe(pipeHandle);			// disconnect the client
  ConnectNamedPipe(pipeHandle, serviceOLap);	// set up so service is notified on
  // next connection.
}


DWORD Scan::ioOpen(DWORD dwOpenMode)
{
  try
    {
      //
      // Pointer for message buffer from FormatMessage
      //
      LPVOID lpMsgBuf;
      
      //
      // Log the name of the memory mapped file
      //
      std::string _message("Scan:ioOpen[");
      _message.append(fileToScan);
      _message.append("]");
      Logger::Log(Logger::LogDebug, Logger::LogAvisScanner, _message.c_str());

      if (!(writeAccess && repairInitialized))
        {
          std::string	mappedPrefix("\\\\.\\mapped\\");
          int	begining	= fileToScan.find(mappedPrefix);
          if (std::string::npos == begining)
            {
              mappedFile	= false;
              DWORD _creation_flags = 0;
              if (writeAccess)
                {
                  _creation_flags = (GENERIC_READ | GENERIC_WRITE);
                }
              else
                {
                  _creation_flags = (GENERIC_READ);
                }
              
              // 
              // Perform the call to Create File
              //

              fileHandle = CreateFile((LPCTSTR) fileToScan.c_str(),
                                      _creation_flags, NULL, NULL,
                                      OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
              if (fileHandle != NULL)
                {
                  fileSize	= GetFileSize(fileHandle, NULL);
                  Logger::Log(Logger::LogDebug,Logger::LogAvisScanner,"CreateFile succeeded!");
                }
              else
                {
                  Logger::Log(Logger::LogError,Logger::LogAvisScanner,"CreateFile failed");
                  FormatMessage( 
                                FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                                FORMAT_MESSAGE_FROM_SYSTEM | 
                                FORMAT_MESSAGE_IGNORE_INSERTS,
                                NULL,
                                GetLastError(),
                                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                                (LPTSTR) &lpMsgBuf,
                                0,
                                NULL 
                                );
                  
                  Logger::Log(Logger::LogError,Logger::LogAvisScanner,(LPCSTR)lpMsgBuf);
                  throw ScanException(ScanException::ioOpen,ExceptionHandler("ioOpen callback at CreateFile").c_str(),GetLastError());

                }
            }
          else
            {
              mappedFile	= true;
              std::string	mappedName = fileToScan.substr(mappedPrefix.size(), fileToScan.size());
              DWORD _open_flags = 0;
              if (writeAccess == true)
                {
                  _open_flags = FILE_MAP_WRITE;
                }
              else
                {
                  _open_flags = FILE_MAP_READ;
                }
                
              fileHandle = OpenFileMapping(_open_flags,FALSE,mappedName.c_str());
              if (fileHandle != NULL)
                {
                  // We've succeeded.
                  mappedPointer = (uchar *)MapViewOfFile(fileHandle,_open_flags,0,0,0);
                  Logger::Log(Logger::LogDebug,Logger::LogAvisScanner,"File opened and mapped");
                  fileSize		= *((ulong*)mappedPointer);
                  mappedPointer	+= sizeof(ulong);
                  currLoc			= 0;

                }
              else
                {
                  Logger::Log(Logger::LogError,Logger::LogAvisScanner,"MapViewOfFile failed");
                  FormatMessage( 
                                FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                                FORMAT_MESSAGE_FROM_SYSTEM | 
                                FORMAT_MESSAGE_IGNORE_INSERTS,
                                NULL,
                                GetLastError(),
                                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                                (LPTSTR) &lpMsgBuf,
                                0,
                                NULL 
                                );
                      
                  Logger::Log(Logger::LogError,Logger::LogAvisScanner,(LPCSTR)lpMsgBuf);
                  throw ScanException(ScanException::ioOpen,ExceptionHandler("ioOpen callback at OpenfileMapping").c_str(),GetLastError());
                }

            }
        }
      else
        {
          currLoc	= 0;
        }

      if (writeAccess)
        repairInitialized = true;

      return (DWORD) this;
    }
  catch (...)
    {
      ExceptionHandler("ioOpen callback");
      Logger::Log(Logger::LogCriticalError,Logger::LogAvisScanner,
		  "exiting after catching ioOpen callback exception");
      ScanServiceGlobals::stopService = true;
      exit(1);
    }

  return 0;
}

BOOL Scan::ioClose()
{
  try
    {
      if (!(writeAccess && repairInitialized))
        {
          if (mappedFile && 0 != mappedPointer)
            {
              mappedPointer	-= sizeof(ulong);
              UnmapViewOfFile(mappedPointer);
              mappedPointer	= 0;
            }
          return (INVALID_HANDLE_VALUE == fileHandle) ? false :
            CloseHandle(fileHandle);
        }
      else
        return true;
    }
  catch (...)
    {
      ExceptionHandler("ioClose callback");
      Logger::Log(Logger::LogCriticalError,Logger::LogAvisScanner,
		  "exiting after catching ioClose callback exception");
      ScanServiceGlobals::stopService = true;
      exit(1);
    }

  return false;
}

char * From[3] = { "front", "current", "end" };


DWORD Scan::ioSeek(LONG lOffset, int nFrom)
{
  try
    {
      if (mappedFile)
        {
          //char buffer[64];
          //sprintf(buffer, "callback bufferSeek(, %ld, %s)", lOffset, From[nFrom]);
          //Logger::Log(Logger::LogDebug, Logger::LogAvisScanner, buffer);
          //ulong oldLoc = currLoc;

          if (1 == nFrom)
            currLoc	+= lOffset;
          else if (2 == nFrom)
            currLoc	= fileSize + lOffset;  // jhill changed from  minus to plus to fix bug Jean-Michel found April 2001
          else
            currLoc = lOffset;

          if (currLoc < 0)
            currLoc = -1;

          //sprintf(buffer, "   returned %ld, moved from %ld to %ld", currLoc, oldLoc, currLoc);
          //Logger::Log(Logger::LogDebug, Logger::LogAvisScanner, buffer);

          return currLoc;
        }
      else
        {
          DWORD	moveMethod = FILE_BEGIN;
          if (1 == nFrom)
            moveMethod	= FILE_CURRENT;
          else if (2 == nFrom)
            moveMethod	= FILE_END;

          return SetFilePointer(fileHandle, lOffset, NULL, moveMethod);
        }
    }
  catch (...)
    {
      ExceptionHandler("ioSeek callback");
      Logger::Log(Logger::LogCriticalError,Logger::LogAvisScanner,
		  "exiting after catching ioSeek callback exception");
      ScanServiceGlobals::stopService = true;
      exit(1);
    }

  return 0;
}

UINT Scan::ioRead(LPVOID lpvBuffer, UINT nBytesToRead)
{
  try
    {
      DWORD	bytesRead	= 0;

      if (mappedFile)
        {
          //char buffer[64];
          //sprintf(buffer, "callback bufferRead(,, %d)", nBytesToRead);
          //Logger::Log(Logger::LogDebug, Logger::LogAvisScanner, buffer);
          //ulong oldLoc = currLoc;

          if (currLoc > fileSize)
            nBytesToRead = 0;
          else if (nBytesToRead + currLoc > fileSize)
            nBytesToRead = fileSize - currLoc;
          if (nBytesToRead < 0)
            nBytesToRead = 0;
          if (nBytesToRead > 0)
            {
              memcpy(lpvBuffer, &mappedPointer[currLoc], nBytesToRead);
              currLoc += nBytesToRead;
            }

          //sprintf(buffer, "   returned %d, moved from %ld to %ld", nBytesToRead, oldLoc, currLoc);
          //Logger::Log(Logger::LogDebug, Logger::LogAvisScanner, buffer);

          return nBytesToRead;
        }
      else
        {
          ReadFile(fileHandle, lpvBuffer, nBytesToRead, &bytesRead, NULL);

          return bytesRead;
        }
    }
  catch (...)
    {
      ExceptionHandler("ioRead callback");
      Logger::Log(Logger::LogCriticalError,Logger::LogAvisScanner,
		  "exiting after catching ioRead callback exception");
      ScanServiceGlobals::stopService = true;
      exit(1);
    }

  return 0;
}

UINT Scan::ioWrite(LPVOID lpvBuffer, UINT nBytesToWrite)
{
  try
    {
      DWORD	bytesWritten	= 0;
      //ulong oldLoc = currLoc;
      //char buffer[64];
      //sprintf(buffer, "callback bufferWrite(,, %d)", nBytesToWrite);
      //Logger::Log(Logger::LogDebug, Logger::LogAvisScanner, buffer);

      if (mappedFile)
        {
          if (0 == nBytesToWrite)
            {
              padding	-= currLoc - fileSize;
              fileSize = currLoc;
            }
          else if (nBytesToWrite + currLoc < fileSize + padding)
            {
              memcpy(&mappedPointer[currLoc], lpvBuffer, nBytesToWrite);
              bytesWritten = nBytesToWrite;
              currLoc += bytesWritten;
              if (fileSize < currLoc)
                {
                  padding -= currLoc - fileSize;
                  fileSize = currLoc;
                }
            }
          else
            {
              overflowed = true;
              return -1;
            }
        }
      else
        {
          if (!WriteFile(fileHandle, lpvBuffer, nBytesToWrite, &bytesWritten, NULL))
            bytesWritten	= -1;
        }
      //if (0 == nBytesToWrite)
      //	sprintf(buffer, "   returned 0, truncated from %ld to %ld", currLoc, currLoc);
      //else
      //	sprintf(buffer, "   returned %d, moved from %ld to %ld", bytesWritten, oldLoc, currLoc);
      //Logger::Log(Logger::LogDebug, Logger::LogAvisScanner, buffer);


      return bytesWritten;
    }
  catch (...)
    {
      ExceptionHandler("ioWrite callback");
      Logger::Log(Logger::LogCriticalError,Logger::LogAvisScanner,
		  "exiting after catching ioWrite callback exception");
      ScanServiceGlobals::stopService = true;
      exit(1);
    }

  return 0;
}



string Scan::ExceptionHandler(const char* where)
{
  string	errMsg("Exception of type ");
  string	errType;
  char	buff[64];

  ExceptionTyper(errType);
  errMsg	+= errType;
  errMsg	+= " caught in ";
  errMsg	+= where;
  errMsg	+= " (fileName = ";
  errMsg	+= fileToScan;
  sprintf(buff, ", with seq %d)", sigSeqNum);
  errMsg	+= buff;

  Logger::Log(Logger::LogError, Logger::LogAvisScanner, errMsg.c_str());

  return errMsg;
}
