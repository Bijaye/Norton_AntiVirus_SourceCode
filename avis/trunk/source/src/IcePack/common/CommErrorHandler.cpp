#include "stdafx.h"

#include <AVIS.h>
#include "EventLog.h"
#include "CommErrorHandler.h"
#include "GlobalData.h"

/* ----- */

using namespace IcePackAgentComm;
using namespace std;

/* ----- */

/*
 * This file has been commented in an attempt to work out what it
 * does and why it wasn't working.  None of the comments in the
 * file were written design or code-authoring time, and reflect
 * the understanding of the code that I have, rather than the
 * understanding of the code that the author had.
 *
 * You may decide which of the above is greater.
 *
 * [inw 2000-06-04]
 */

/* ----- */

void CommErrorHandler::HandleCommError(IcePackAgentComm::ErrorCode errCode,
                                       ulong                       systemErrorCode,
                                       ErrorInfo&                  errorInfo,
                                       bool&                       retryWithInterval,
                                       bool&                       retryWithoutInterval,
                                       Logger::LogSource           caller,
                                       const char                 *errMsgPrefix)
{
  EntryExit entryExit(caller, "HandleCommError");
  string    why;
  char      err[512];

  /*
   * Start out by assuming these are false.
   */
  retryWithInterval     = false;
  retryWithoutInterval  = false;

  /*
   * May not be valid, but get it in case it's needed.
   */
  err[0] = '\0';
  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, systemErrorCode, 0, err, 512, NULL);

  /*
   * We should only be here if the error is not from the gateway.
   */
  if (GatewayError == errCode)
    return;

  errorInfo.CommError(errCode);

  switch (errCode)
  {
    case PackageNotFoundError :
      if (errorInfo.tries < 2)
        retryWithoutInterval  = true;

      why = " PackageNotFoundError ";
      break;
    case TempDirError :
      if (0 == errorInfo.tries)
      {
        if (!(FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(GlobalData::TempDirectory().c_str())))
          CreateDirectory(GlobalData::TempDirectory().c_str(), NULL);

        GlobalData::SetCommParameters();
        retryWithoutInterval  = true;
      }
      else
      {
        EventLog::Error(FACILITY_COMMUNICATIONS,
                        IPREG_COMM_UNPACK_FAILURE,
                        "Temporary directory could not be created (TempDirError)");
        GlobalData::Stop();
      }
      why = " TempDirError ";
      break;
    case TargetDirError :
      if (0 == errorInfo.tries)
      {
        if (!(FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(GlobalData::DefLibraryDirectory().c_str())))
          CreateDirectory(GlobalData::DefLibraryDirectory().c_str(), NULL);
        retryWithoutInterval  = true;
      }
      else
      {
        EventLog::Error(FACILITY_COMMUNICATIONS,
                        IPREG_COMM_UNPACK_FAILURE,
                        "Temporary directory could not be created (TargetDirError)");
        GlobalData::Stop();
      }
      why = " TargetDirError ";
      break;
    case TargetDOSDirError :
      EventLog::Error(FACILITY_COMMUNICATIONS,
                      IPREG_COMM_UNPACK_FAILURE,
                      "Temporary directory could not be created (TargetDOSDirError)");
      GlobalData::Stop();

      why  = " TargetDOSDirError, system error [";
      why += err;
      why += "] ";
      break;
    case FileCopyError :
      if (0 == errorInfo.tries)
      {
        GlobalData::RecoverDiskSpace();
        why  = " FileCopyError, system error [";
        why += err;
        why += "] ";
      }
      else
      {
        EventLog::Error(FACILITY_COMMUNICATIONS,
                        IPREG_COMM_UNPACK_FAILURE,
                        "files could not be copied/moved from the temporary directory to the target directory");
        GlobalData::Stop();
      }
      break;
    case NoFilesFoundError :
      if (0 == errorInfo.tries)
        retryWithoutInterval  = true;
      why = " NoFilesFoundError ";
      break;
    case DiskFullError:
      if (0 == errorInfo.tries)
        GlobalData::RecoverDiskSpace();
      else
      {
        EventLog::Error(FACILITY_COMMUNICATIONS,
                        IPREG_COMM_UNPACK_FAILURE,
                        "Disk is full");
        GlobalData::Stop();
      }
      why = " DiskFullError ";
      break;
    case VersionError :
      why = " VersionError ";
      break;
    case SystemException :
      why  = " SystemException, system error [";
      why += err;
      why += "] ";
      EventLog::Error(FACILITY_COMMUNICATIONS,
                      IPREG_COMM_SYS_EXCEPTION,
                      err);
      GlobalData::Stop();
      break;
    case LibraryException :
      why  = " LibraryException, system error [";
      why += err;
      why += "] ";
      EventLog::Error(FACILITY_COMMUNICATIONS,
                      IPREG_COMM_LIB_EXCEPTION,
                      err);
      GlobalData::Stop();
      break;
    case ProcessCreationError :
      EventLog::Error(FACILITY_COMMUNICATIONS,
                      IPREG_COMM_UNPACK_FAILURE,
                      "ProcessCreationError");
      GlobalData::Stop();
      why  = " ProcessCreationError, system error [";
      why += err;
      why += "] ";
      break;
    case ProcessTimedOutError :
      if (errorInfo.tries < GlobalData::RetryLimit())
        retryWithInterval = true;
      why = " ProcessTimedOutError ";
      break;
    case ProcessTerminationError :
      if (errorInfo.tries < GlobalData::RetryLimit())
        retryWithInterval = true;
      why  = " ProcessTerminationError, system error [";
      why += err;
      why += "] ";
      break;
    case ProcessSyncError :
      if (errorInfo.tries < GlobalData::RetryLimit())
        retryWithInterval = true;       
      why  = " ProcessSyncError, system error [";
      why += err;
      why += "] ";
      break;
    case NoSignaturesAvailable :  // try forever
      retryWithInterval = true;
      why = " NoSignaturesAvailable ";
      break;
    case FileWriteError :
      why  = " FileWriteError, system error [";
      why += err;
      why += "] ";
      EventLog::Error(FACILITY_COMMUNICATIONS,
                      IPREG_COMM_FILE_IO,
                      "Write");
      GlobalData::Stop();
      break;
    case FileReadError :
      why  = " FileReadError, system error [";
      why += err;
      why += "] ";
      EventLog::Error(FACILITY_COMMUNICATIONS,
                      IPREG_COMM_FILE_IO,
                      "Read");
      GlobalData::Stop();
      break;
    case FileOpenError :
      why  = " FileOpenError, system error [";
      why += err;
      why += "] ";
      EventLog::Error(FACILITY_COMMUNICATIONS,
                      IPREG_COMM_FILE_IO,
                      "Open");
      GlobalData::Stop();
      break;
    case UnknownSignatureTypeError :
      EventLog::Error(FACILITY_COMMUNICATIONS,
                      IPREG_COMM_UNKWN_SIG_TYPE);
      GlobalData::Stop();
      why = " UnknownSignatureTypeError ";
      break;
    case MissingSignatureSequenceError :
      if (errorInfo.tries < GlobalData::RetryLimit())
        retryWithInterval = true;
      else
      {
        EventLog::Error(FACILITY_COMMUNICATIONS,
                        IPREG_COMM_MISSING_SEQ_NUM);
        GlobalData::Stop();
      }
      why = " MissingSignatureSequenceError ";
      break;
    case NetworkError :       // try forever
      retryWithInterval = true;
      why  = " NetworkError, system error [";
      why += err;
      why += "] ";
      break;
#if 0
    case NoHttpCodeError :
      if (errorInfo.tries < GlobalData::RetryLimit())
        retryWithInterval = true;
      why = " NoHttpCodeError ";
      break;
#endif /* 0 */
    case NetworkCriticalError :
      if (errorInfo.tries < GlobalData::RetryLimit())
        retryWithInterval = true;
      why  = " NetworkCriticalError, system error [";
      why += err;
      why += "] ";
      break;
    case NoContentError :
      if (errorInfo.tries < GlobalData::RetryLimit())
        retryWithInterval = true;
      why = " NoContentError ";
      break;
    case FatAlbertSystemException :
    case VDBSystemException :
    case VDBZipArchiveError :
    case VDBMemoryError :
    case VDBParameterError :
    case VDBUnexpectedEOFError :
    case VDBUnknownError :
      if (errorInfo.tries < GlobalData::RetryLimit())
        retryWithInterval = true;
      why  = " FatAlbertSystemException, system error [";
      why += err;
      why += "] ";
      break;
    case ThreadCouldNotStartError :
      why  = " ThreadCouldNotStartError, system error [";
      why += err;
      why += "] ";
      EventLog::Error(FACILITY_COMMUNICATIONS,
                      IPREG_COMM_THREAD_NO_START);
      GlobalData::Stop();
      break;
    case InvalidRedirectError :
      why = " InvalidRedirectError (there is a misconfiguration at the gateway)";
      break;
    case UnknownError :
      why  = " UnknownError, system error [";
      why += err;
      why += "] ";
      EventLog::Error(FACILITY_COMMUNICATIONS,
                      IPREG_COMM_UNKNOWN);
      GlobalData::Stop();
      break;
    case OutOfMemoryError :
      why = " Out of memory ";
      EventLog::Error(FACILITY_COMMUNICATIONS,
                      IPREG_COMM_OUT_OF_MEM);
      GlobalData::Stop();
      break;
    case TerminatedByCaller :
      why = " IcePack shutdown initiated by another thread ";
      break;
    default:
      if (errorInfo.tries < GlobalData::RetryLimit())
        retryWithInterval = true;
      why  = " Even more unknown error, system error [";
      why += err;
      why += "] ";
      break;
  }

  if (!GlobalData::stop)
  {
    string msg("Error ");

    msg += errMsgPrefix;
    msg += why;
    Logger::Log(Logger::LogWarning, caller, msg.c_str());
  }
  else
  {
    string msg("Critical error ");

    msg += errMsgPrefix;
    msg += why;
    Logger::Log(Logger::LogCriticalError, caller, msg.c_str());
    EventLog::Error(FACILITY_GENERAL, IPREG_COMM_ERROR_STOP, msg.c_str());
  }
}
/* ----- Transmission Ends ----- */
