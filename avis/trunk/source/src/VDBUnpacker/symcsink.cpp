/*
 * symcsink.cpp
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
 * The implementation of the CinwEventSink object,
 * an overridden IDecEventSink, and is used by the
 * Symantec Decomposer API.  See symcsink.h for
 * the interface definition.
 *
 * Notes
 * -----
 * Must be compiled in single-byte mode (that is to say, no Unicode
 * or wide-char support).  In addition, the Symantec Decomposer
 * modules must also be compiled in single-byte mode.
 *
 * Changelog
 * ---------
 * Please keep this change log up to date!
 *
 * inw  2000-02-10 : Initial implementation.
 * inw  2000-02-11 : Folded into VDBUnpacker.
 */

/* ----- Standard headers ----- */

#include <stdio.h>

/* ----- Windows headers ----- */

#include <windows.h>

/* ----- STL headers ----- */

#include <string>

/* ----- My headers ----- */

#include "symcsink.h"
#include "MyTraceClient.h"
#include "Util.h"

/* ----- START HERE ----- */

/*
 * Constructor.
 *
 * The caller should really pass a VDBUnpacker-style
 * trace object, but he doesn't have to.
 */
CinwEventSink::CinwEventSink(MyTraceClient *tc) : _rootFile(""),
                                                  _outDir(""),
                                                  _callbackHappened(false),
                                                  CDecEventSink()
{
  _tc = tc;

  if (_tc)
    _tc->debug("In CinwEventSink() constructor");
}

/* ----- */

/*
 * Destructor.
 *
 * For future expansion.
 */
CinwEventSink::~CinwEventSink()
{
  if (_tc)
    _tc->debug("In CinwEventSink() destructor");

  _tc = NULL;
}

/* ----- */

/*
 * Allows the caller to set the root file -- that is to
 * say, the archive on the disk that we're unpacking.
 */
void CinwEventSink::rootFile(std::string rootFile)
{
  _rootFile = rootFile;
}

/* ----- */

/*
 * Allows the caller to retrieve the root file.
 */
std::string CinwEventSink::rootFile()
{
  return _rootFile;
}

/* ----- */

/*
 * Allows the caller to set the output directory -- the
 * directory into which decompressed files will be placed.
 */
void CinwEventSink::outputDir(std::string outDir)
{
  _outDir = outDir;
}

/* ----- */

/*
 * Allows the caller to retrieve the output directory.
 */
std::string CinwEventSink::outputDir()
{
  return _outDir;
}

/* ----- */

/*
 * Allows the caller to find out whether or any callbacks
 * indicating that the file is actually an archive
 * occurred.
 */
boolean CinwEventSink::callbackHappened()
{
  return _callbackHappened;
}

/* ----- Symantec callbacks ----- */

/*
 * OnNew() is called by the Decomposer engine (I quote) 'just
 * before it uses the engines to identify a data object'.  What
 * this means is that OnNew() will be called right at the start
 * of processing an archive (that is to say, it will be called
 * with an IDecObject for the on-disk object we're processing),
 * and then will be called again for each file within the
 * archive.
 * 
 * When OnNew() is called, the file is available on the host's
 * disk for examination.  Clearly this is of questionable use in
 * the case of the initial call, but it makes for a nice
 * symmetrical interface (consider recursive archives, for
 * example).  In our case, it's the subsequent calls which are
 * useful.  
 *
 * So, I first get the filename, and compare it with the filename
 * which we started with.  If it's the same, then I just say 'go
 * on'.  If it's different, then I rename (aka move) the
 * temporary copy of the file to over to the correct name.
 *
 * In the event of errors calling methods on the IDecObject
 * interface, I return the handy DECERR_GENERIC.
 */
HRESULT CinwEventSink::OnNew(IDecObject *pObject,
                             WORD       *pwResult,
                             LPTSTR      pszNewDataFile,
                             BOOL       *pbContinue)
{
  char        fileName[MAX_PATH];
  char        dataFileName[MAX_PATH];
  HRESULT     hr;
  char       *newFileName  = NULL;

  if (_tc)
    _tc->debug("In CinwEventSink::OnNew()");

  /*
   * Carefully get the filename.  This is the name that the file
   * _should_ have.  The Decomposer engine doesn't extract it
   * with that name.
   */
  fileName[0] = 0;
  hr = pObject->GetName(fileName);
  if (FAILED(hr))
  {
    if (_tc)
      _tc->msg("GetName() failed in OnNew(), error %lx", hr);
    return DECERR_GENERIC;
  }

  /*
   * If it's _not_ the file with which we started...
   */
  if (fileName != _rootFile)
  {
    /*
     * Get the on-disk filename [cf GetName].
     */
    dataFileName[0] = 0;
    hr = pObject->GetDataFile(dataFileName);
    if (FAILED(hr))
    {
      if (_tc)
        _tc->msg("GetDataFile() failed in OnNew(), error %lx", hr);
      return DECERR_GENERIC;
    }

    if (_tc)
    {
      _tc->debug("GetName()     returned '%s'", fileName);
      _tc->debug("GetDataFile() returned '%s'", dataFileName);
    }

    /*
     * Get enough memory to put the full name in.
     * If it fails, either an exception will be thrown and caught
     * outside, or we'll get NULL back and spot it.
     */
    newFileName = new char[_outDir.size() + strlen(fileName) + 5];
    if (! newFileName)
      return DECERR_GENERIC;
    _makepath(newFileName, NULL, _outDir.c_str(), fileName, NULL);
    DirUtil::renameFileMakingDirs(dataFileName, newFileName, _tc);
    delete [] newFileName;
  }
  else
  {
    /*
     * If it _is_ the file with which we started, then clear the
     * callback indicator, just in case someone is reusing
     * the object.
     */
    _callbackHappened = false;
  }

  /*
   * We're not changing this file.
   */
  *pwResult      = DECRESULT_UNCHANGED;
  pszNewDataFile = NULL;

  /*
   * Go on.
   */
  *pbContinue    = TRUE;
  return DEC_OK;
}

/* ----- */

/*
 * OnIdentified() is called when the Decomposer engines think decide
 * that a file is of a type it can deal with (that is to say,
 * is itself a container).
 *
 * In the case of VDB files, this should only happen once --
 * OnIdentified() will be called with a container object
 * constructed for the file that was originally passed
 * to Process() -- that is to say, the .vdb file we're unpacking.
 * None of the files within the VDB should be identified as
 * of archive format.
 *
 * However, just in case, I retrieve the name of the object
 * upon which we are being called.  If it's the same as the
 * object that Process() was originally called upon, then
 * I say 'continue down into this object'.  Otherwise, I say
 * 'don't continue down into this object'.
 *
 * The next callback to occur will be OnChildFound().
 */
HRESULT CinwEventSink::OnIdentified(IDecContainerObject *pObject,
                                    BOOL                *pbContinue)
{
  char    fileName[MAX_PATH];
  HRESULT hr;

  if (_tc)
    _tc->debug("In CinwEventSink::OnIdentified()");

  /*
   * Make a note that a relevant callback occurred.  See
   * comments in ZipArchive.cpp for why.
   */
  _callbackHappened = true;

  /*
   * Get the name of the object upon which we are being called.
   */
  fileName[0] = 0;
  hr = pObject->GetName(fileName);

  /*
   * Set the continue value to 'go' if (a) the call
   * succeeded, and (b) the filename matches the root filename.
   * Otherwise, we set 'don't.
   *
   * This means that if the GetName() call failed, we ignore
   * the failure.  This is okay, because (a) subsequent calls
   * will also fail, and (b) we don't care anyway, as 'don't'
   * is the correct default answer.
   */
  *pbContinue = (((SUCCEEDED(hr)) && (fileName == _rootFile)) ? true : false);

  /*
   * JD.
   */
  return DEC_OK;
}

/* ----- */

/*
 * OnChildFound() is called when the Decomposer engine (I quote)
 * 'first knows the name of one of its contained files'.  The
 * file is not yet available on the disk for processing, but we
 * know the name.  This is presumably designed for primative
 * extension-based anti-virus measures.
 *
 * In our case, we just tell it to proceed.  The next callback
 * will be OnNew().
 */
HRESULT CinwEventSink::OnChildFound(IDecContainerObject *pObject,
                                    LPCTSTR              pszChildName,
                                    BOOL                *pbContinue)
{
  if (_tc)
  {
    _tc->debug("In CinwEventSink::OnChildFound()");
    _tc->debug("  pszChildName is '%s'", pszChildName);
  }
  
  /*
   * Go on.
   */
  *pbContinue = TRUE;
  return DEC_OK;
}

/* ----- */

/*
 * OnBadChild() is called by the Decomposer engine when (I quote)
 * 'it is unable to process a contained file'.
 *
 * This callback will only occur in the case of an error.  In
 * that case, we tell the engine to stop -- there's no point
 * going on.
 */
HRESULT CinwEventSink::OnBadChild(IDecContainerObject *pObject,
                                  HRESULT              hr,
                                  LPCTSTR              pszChildName,
                                  BOOL                *pbContinue)
{
  if (_tc)
  {
    _tc->debug("In CinwEventSink::OnBadChild()");
    _tc->debug("  pszChildName is '%s'", pszChildName);
    _tc->debug("  hr [error]   is %lx", hr);
  }
  
  /*
   * Stop now.
   */
  *pbContinue = FALSE;

  /*
   * Choose an error code.  We're really only interested in
   * 'disk full' type codes.
   */
  HRESULT retMe;
  switch (hr)
  {
    case DECERR_CHILD_EXTRACT :
    case DECERR_TEMPFILE_CREATE :
    case DECERR_TEMPFILE_OPEN :
    case DECERR_DISK_FULL :
    case DECERR_DISK_NOT_READY :
      retMe = DECERR_DISK_FULL;
      _tc->debug("  [translating error to DECERR_DISK_FULL]");
      break;
    default :
      retMe = DECERR_GENERIC;
      break;
  }

  return retMe;
}

/* ----- */

/*
 * OnNotIdentified() is called if the Decomposer engine
 * fails to identify a file as an archive type (that is
 * to say, it's my belief that on each and every file,
 * one [and only one] of OnNotIdentified() and OnIdentified()
 * will be called).
 *
 * In our case, we just say 'go on' -- continue to process
 * this file.  The next callback to occur will be
 * OnChildFound().
 */
HRESULT CinwEventSink::OnNotIdentified(IDecObject  *pObject,
                                       WORD       *pwResult,
                                       LPTSTR      pszNewDataFile)
{
  if (_tc)
    _tc->debug("In CinwEventSink::OnNotIdentified()");

  /*
   * We're not changing this file.
   */
  *pwResult       = DECRESULT_UNCHANGED;
  pszNewDataFile  = NULL;

  /*
   * Go on.
   */
  return DEC_OK;
}

/* ----- */

/*
 * OnBusy() is called by the Decomposer engine to indicate
 * that it hasn't gone to sleep -- it's still working away
 * on something.  Although the documentation is silent on
 * the point, I suspect that returning any code other
 * that DEC_OK will cause it to stop and return from the
 * Process() call.
 *
 * In our case, we always say DEC_OK.
 */
HRESULT CinwEventSink::OnBusy()
{
  if (_tc)
    _tc->debug("In CinwEventSink::OnBusy()");

  /*
   * Go on.
   */
  return DEC_OK;
}

/* ----- */

/*
 * OnEngineError() is called when the engine encounters an
 * error [well duh!].  Possible return values are (a) DEC_OK,
 * which tells the engine to ignore the error and blunder on
 * hopelessly through the files; or (b) anything else, which
 * will cause the original call to Process() to return as
 * soon as possible, and it will return with the same code
 * that this method returned with (grammar failure detected).
 *
 * In our case, we tell it to stop, and return the handy-
 * sounding code DECERR_GENERIC.
 */
HRESULT CinwEventSink::OnEngineError(IDecContainerObject *pObject,
                                     HRESULT              hr,
                                     WORD                *pwResult,
                                     LPTSTR               pszNewDataFile)
{
  if (_tc)
    _tc->debug("In CinwEventSink::OnEngineError()");
  
  /*
   * We're not changing this file.
   */
  *pwResult      = DECRESULT_UNCHANGED;
  pszNewDataFile = NULL;

  /*
   * Stop now.
   */
  return DECERR_GENERIC;
}

/* ----- Transmission Ends ----- */
