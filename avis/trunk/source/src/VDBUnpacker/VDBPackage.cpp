//
//  IBM AntiVirus Immune System
//
//  File Name:  VDBPackage.cpp
//  Author:   Milosz Muszynski
//
//  VDB Package Extraction Utility Class
//
//  $Log:  $
//


//------ pragmas ------------------------------------------------------
#pragma warning(disable : 4786) // identifier truncated warning

//------ STL includes -------------------------------------------------
#include <string>

//------ Run Time Library includes ------------------------------------
#include <process.h>

//------ local includes -----------------------------------------------
#include "VDBUnpacker.h"
#include "ZipArchive.h"
#include "Util.h"
#include "ZExcept.h"
#include "MyTraceClient.h"

//------ static data --------------------------------------------------
// the following list of files is used for checking if the
// VDB file extraction was successful
// these files get deleted before the VDBUnpacker is run
const char *  targetFileList[]  =
{
  "NAVENG32.DLL",
  "NAVEX32A.DLL",
  "VIRSCAN1.DAT",
  "VIRSCAN2.DAT",
  "VIRSCAN3.DAT",
  "VIRSCAN4.DAT",
  "VIRSCAN5.DAT",
  "VIRSCAN6.DAT",
  "VIRSCAN7.DAT",
  "VIRSCAN8.DAT",
  "VIRSCAN9.DAT",
  "ZDONE.DAT",
  NULL
};

//------ local definitions --------------------------------------------
#define COMMAND_LINE_MAX_LENGTH 2048

//------ static function prototypes -----------------------------------
static void run(const std::string&        package,
                const std::string&        targetDirectory,
                VDBPackage::ErrorCode&    _errorCode,
                void                     *traceClient = NULL);

//------ methods of class VDBPackage ----------------------------------


//
// VDBPackage constructor
//

VDBPackage::VDBPackage() : _errorCode(NoError),
                           _systemExceptionCode(0)
{
}


//
// VDBPackage::Unpack
//

bool VDBPackage::Unpack(const std::string&  package,
                        const std::string&  targetDirectory,
                        HANDLE              mutex,
                        void               *traceClient)
{
  if (mutex != NULL)
    WaitForSingleObject(mutex, INFINITE);

  MyTraceClient * _tc = static_cast<MyTraceClient*>(traceClient);

  ZExceptionTranslator exceptionTranslator;
  exceptionTranslator.init();

  _errorCode = NoError;

  try
  {
    if (FileUtil::exists(targetDirectory))
      DirUtil::prune(targetDirectory, _tc);

    DirUtil::make(targetDirectory, _tc);

    if (! FileUtil::exists(targetDirectory, _tc))
      _errorCode = TargetDirError;

    if (! FileUtil::exists(package, _tc))
      _errorCode = PackageNotFoundError;

    if (_errorCode == NoError)
    {
      run(package, targetDirectory, _errorCode, _tc);// run the unzipper
    }

    if (_errorCode == NoError)
    {
      if (! FileUtil::allExist(targetDirectory, targetFileList, _tc))      // check if OK
      {
        _errorCode = NoFilesFoundError;                     // test files not found
        DirUtil::prune(targetDirectory, _tc);
      }
    }
  }
  catch(ZException& exc)
  {
    _errorCode            = SystemException;
    _systemExceptionCode  = exc.code();
  }

  if (mutex != NULL)
    ReleaseMutex(mutex);

  std::string errorStr;
  switch(_errorCode)
  {
    case NoError :
      errorStr = "NoError";
      break;
    case PackageNotFoundError :
      errorStr = "PackageNotFoundError";
      break;
    case TargetDirError :
      errorStr = "TargetDirError";
      break;
    case NoFilesFoundError :
      errorStr = "NoFilesFoundError";
      break;
    case DiskFullError :
      errorStr = "DiskFullError";
      break;
    case VersionError :
      errorStr = "VersionError";
      break;
    case SystemException :
      errorStr = "SystemException";
      break;
    default :
      errorStr = "UnknownError";
      break;
  }

  if (_tc)
    _tc->msg("VDBUnpacker: exits with rc = \"%s\"", errorStr.c_str());

  if (_errorCode == SystemException)
  {
    if (_tc)
      _tc->msg("VDBUnpacker: system exception code = %8.8x", _systemExceptionCode);
  }

  return _errorCode == NoError;
}


//
// VDBPackage::Prune
//

/*
 * This function now only deletes the package file (presumably
 * the .VDB) if it successfully removes the directory.  This
 * behaviour is important, as it means that the .VDB file will
 * still be there next time, unless all the removes worked out.
 * This, in turn, means that the caller will notice that there is
 * more pruning to be done.
 */
bool VDBPackage::Prune(const std::string&  package,
                       const std::string&  targetDirectory,
                       HANDLE              mutex,
                       void               *traceClient)
{
  if (mutex != NULL)
    WaitForSingleObject(mutex, INFINITE);

  MyTraceClient * _tc = static_cast<MyTraceClient*>(traceClient);

  ZExceptionTranslator exceptionTranslator;
  exceptionTranslator.init();

  _errorCode = NoError;

  try
  {
    /*
     * First, find out if the directory exists.  The directory
     * contains the unpacked definition files.
     */
    if (FileUtil::exists(targetDirectory, _tc))
    {
      /*
       * If the directory is there, attempt to delete everything
       * in it, and it.  DirUtil::prune() appears to return true
       * only if it successfully removed everything.
       */
      if (! DirUtil::prune(targetDirectory, _tc))
        _errorCode = DirPruningError;
    }
    else
      _errorCode = PackageNotFoundError;

    /*
     * If we get here, and we have had no error, then look for,
     * and attempt to remove, the .VDB file (the 'package' file).
     */
    if (NoError == _errorCode)
    {
      if (! FileUtil::exists(package, _tc))
       _errorCode = PackageNotFoundError;
      else
        if (! FileUtil::remove(package, _tc))
         _errorCode = FileRemovingError;
    }
  }
  catch(ZException& exc)
  {
    _errorCode        = SystemException;
    _systemExceptionCode  = exc.code();
  }

  if (mutex != NULL)
    ReleaseMutex(mutex);

  std::string errorStr;
  switch(_errorCode)
  {
    case NoError :
      errorStr = "NoError";
      break;
    case PackageNotFoundError :
      errorStr = "PackageNotFoundError";
      break;
    case TargetDirError :
      errorStr = "TargetDirError";
      break;
    case NoFilesFoundError :
      errorStr = "NoFilesFoundError";
      break;
    case DiskFullError :
      errorStr = "DiskFullError";
      break;
    case VersionError :
      errorStr = "VersionError";
      break;
    case FileRemovingError :
      errorStr = "FileRemovingError";
      break;
    case DirPruningError :
      errorStr = "DirPruningError";
      break;
    case SystemException :
      errorStr = "SystemException";
      break;
    default :
      errorStr = "UnknownError";
      break;
  }

  if (_tc)
    _tc->msg("VDBUnpacker: exits with rc = \"%s\"", errorStr.c_str());

  if (_errorCode == SystemException)
    if (_tc)
      _tc->msg("VDBUnpacker: system exception code = %8.8x", _systemExceptionCode);

  return _errorCode == NoError;
}


//
// Error
//
VDBPackage::ErrorCode VDBPackage::Error()
{
  return _errorCode;
}


//
// ExceptionCode
//
unsigned long VDBPackage::ExceptionCode()
{
  return _systemExceptionCode;
}


//------ static functios ----------------------------------------------

//
// run
//

void run(const std::string&        package,
         const std::string&        targetDirectory,
         VDBPackage::ErrorCode&    _errorCode,
         void                     *traceClient)
{
  MyTraceClient * _tc = static_cast<MyTraceClient*>(traceClient);

  ZipArchive zipArchive;

  zipArchive.Extract(package, targetDirectory, _tc);

  ZipArchive::ErrorCode zipError = zipArchive.Error();

  switch (zipError)
  {
    case ZipArchive::NoError :
      _errorCode = VDBPackage::NoError;
      break;
    case ZipArchive::ZipArchiveError :
      _errorCode = VDBPackage::ZipArchiveError;
      break;
    case ZipArchive::MemoryError :
      _errorCode = VDBPackage::MemoryError;
      break;
    case ZipArchive::ArchiveNotFoundError :
      _errorCode = VDBPackage::PackageNotFoundError;
      break;
    case ZipArchive::ParameterError :
      _errorCode = VDBPackage::ParameterError;
      break;
    case ZipArchive::NoFilesFoundError :
      _errorCode = VDBPackage::ZipNoFilesFoundError;
      break;
    case ZipArchive::DiskFullError :
      _errorCode = VDBPackage::DiskFullError;
      break;
    case ZipArchive::UnexpectedEOFError :
      _errorCode = VDBPackage::UnexpectedEOFError;
      break;
    default :
      _errorCode = VDBPackage::ZipUnknownError;
      break;
  }
}











