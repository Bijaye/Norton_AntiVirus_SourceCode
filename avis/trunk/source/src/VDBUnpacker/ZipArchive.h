/*
 * ZipArchive.h
 *
 * ----- COPYRIGHT STATEMENT -----
 *
 * Licensed Materials - Property of IBM
 *
 * (c) Copyright IBM Corp. 2000
 *
 * U.S. Government Users Restricted Rights - use,
 * duplication or disclosure restricted by GSA ADP
 * Schedule Contract with IBM Corp.
 *
 * ----- COPYRIGHT STATEMENT ----- 
 *
 * What
 * ----
 * Interface definition for the ZipArchive object,
 * which is used to unpack .VDB packages.  See
 * ZipArchive.cpp for the implementation.
 *
 * Changelog
 * ---------
 * Please keep this change log up to date!
 *
 * mm   1999-??-?? : First implementation, using InfoZIP.
 * inw  2000-02-10 : Reimplementated for Decomposer.
 * inw  2000-02-11 : Folded into VDBUnpacker.
 */

/* ----- Include guard ----- */

#ifndef __ZIPARCHIVE_DOT_H_
#define __ZIPARCHIVE_DOT_H_

/* ----- Our headers ----- */

#include "symcsink.h"
#include "MyTraceClient.h"

/* ----- START HERE ----- */

class ZipArchive
{
public:
  enum ErrorCode
  { 
    NoError              = 0,
    UnknownError,
    ZipArchiveError,
    ArchiveNotFoundError, 
    MemoryError,
    NoFilesFoundError,
    ParameterError,
    DiskFullError, 
    UnexpectedEOFError,
  };

  /*
   * At the present time, there are no options available on
   * this version of the interface.  This may change, so
   * I have left this code here
   */
  enum Options
  {
    DefaultOptions         = 0x0000,
//  OnlyNewerOrNewFiles    = 0x0800,   // Copying into archive.
//  RecurseIntoDirectories = 0x1000,   // Copying into archive.
//  OnlyChangedFiles       = 0x2000,   // Copying into archive.
//  DoNotMakeDirectories   = 0x4000,   // Copying out of archive.
//  DoNotOverwriteFiles    = 0x8000,   // Copying out of archive.
  };


            ZipArchive   ();
            ~ZipArchive  ();
  ErrorCode Error        ();
  bool      Extract      (const std::string &archive,
                          const std::string &directory,
                          void              *traceClient = NULL,
/* Not used */            Options            options     = DefaultOptions,
/* Not used */            const std::string &files       = "");

protected:
  ErrorCode _errorCode;

private:
  bool           _ensureOnline(MyTraceClient *tc);
  IDecomposer    *_symcDecomposer;
  IDecEngine     *_symcZipEngine;
  CinwEventSink  *_symcEventSink;
  MyTraceClient  *_tc;
};

#endif /* __ZIPARCHIVE_DOT_H__ */
