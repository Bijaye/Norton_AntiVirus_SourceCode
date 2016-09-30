/*
 * ZipArchive.cpp
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
 * Implementation of the ZipArchive object,
 * which is used to unpack .VDB packages.  See
 * ZipArchive.cpp for the interface definition.
 *
 * Changelog
 * ---------
 * Please keep this change log up to date!
 *
 * mm   1999-??-?? : First implementation, using InfoZIP.
 * inw  2000-02-10 : Reimplementated for Decomposer.
 * inw  2000-02-11 : Folded into VDBUnpacker.
 */

/* ----- Important things to remember about the Decomposer ----- */

/*
 * I have established the following things on the subject
 * of the Decomposer interface:
 *
 *  + It's entirely thread-safe.  You can use the
 *    same IDecomposer interface on multiple threads
 *    in a concurrent fashion without any difficulties.
 *    This is not tested (by me), but is stated (by SYMC).
 *
 *  + It's necessary for the calling program to keep
 *    the sub-engine interfaces (for example, the
 *    Zip engine interface returned from DecNewZipEngine())
 *    around until it finishes using the IDecomposer
 *    interface.  The sequence of operations is:
 *      DecNewDecomposer(&decomp);
 *      DecNewZipEngine(&zipEng);
 *      decomp->AddEngine(zipEng);
 *      decomp->SetTempDir(NULL);  // Or whatever.
 *      // Use IDecomposer interface.
 *      decomp->RemoveEngines();
 *      DecDeleteZipEngine(zipEng);
 *      DecDeleteDecomposer(decomp);
 *
 *  + IDecomposer::Process() takes an IDecObject interface,
 *    which describes the object upon which to operate.
 *    Use the following sequence of operations, under normal
 *    circumstances:
 *      DecNewFileObject(fnam, fnam, &fobj);
 *      decomp->Process(fobj, ...); // See later for more.
 *      DecDeleteFileObject(fobj);
 *
 *  + IDecomposer::Process() also takes a CDecEventSink
 *    object, or an object derived from CDecEventSink.
 *    See evntsink.cpp for an example, or just do this:
 *      CDecEventSink evntsink;
 *      decomp->Process(fobj, &evntsink, ...);
 *    For our purposes, it may be that writing a derived
 *    object isn't necessary.  But it's safer, and I feel
 *    more confident using it.  For example, the behaviour
 *    of the default CDecEventSink could so easily change
 *    in the future...
 *
 *  + When calling IDecomposer::Process(), it's necessary to
 *    pass it an event sink object, a 16-bit 'word' to put
 *    a result code in, and a filename.  Whilst, in our case,
 *    we're not interested in the filename (it's used for the
 *    name of a replacement file), it can't be null.  Just
 *    create a buffer of size MAX_PATH, and pass it in.
 *    Try something like the following (IDecObject has already
 *    been created):
 *      char procFil[MAX_PATH];
 *      WORD procRes;
 *      procFil[0] = 0;
 *      procRes    = 0;
 *      decomp->Process(fobj, &evntsink, procRes, procFil);
 *
 *  + IDecomposer::Process() will return DEC_OK (no error)
 *    if it doesn't thing the file you pointed it at is an
 *    archive.  Within the context of the interface, this is
 *    correct behaviour, it's just a shame from our point
 *    of view.
 *
 *  + Decomposer does not throw any exceptions.
 *
 * [inw 2000-02-10]
 * [inw 2000-02-11]
 */

/* ----- Windows headers ----- */

#include <windows.h>

/* ----- STL headers ----- */

#pragma warning(disable:4786)
#include <string>

/* ----- Our headers ----- */

#include "symcsink.h"
#include "ZipArchive.h"
#include "MyTraceClient.h"
#include "Util.h"

/* ----- START HERE ----- */

/*
 * Constructor.
 *
 * For future expansion.
 */
ZipArchive::ZipArchive() : _errorCode(NoError),
                           _symcEventSink(NULL),
                           _symcDecomposer(NULL),
                           _symcZipEngine(NULL),
                           _tc(NULL)
{
}

/* ----- */

/*
 * Destructor.  This has to take care of disconnecting from
 * the Decomposer engines, and that sort of stuff.
 */
ZipArchive::~ZipArchive()
{
  /*
   * Where are we?
   */
  if (_tc)
    _tc->debug("In ZipArchive destructor");

  /*
   * Are we connected to a Decomposer?
   */
  if (_symcDecomposer)
  {
    /*
     * If so, ask it to drop the engines.
     * It's okay to call this if it doesn't
     * have any connected.  In addition,
     * we don't care if it fails,
     * because there's nothing we can do
     * about it, and the engines should
     * be destroyed when the delete the
     * Decomposer anyway.
     */
    if (_tc)
      _tc->debug("Calling RemoveEngines() on Decomposer");
    _symcDecomposer->RemoveEngines();
  }

  /*
   * Now drop my copy of the engine...
   * [you do need to do this, apparently,
   * just like you need to keep 'your'
   * copy of the engine hanging around
   * for the lifetime of the Deecomposer].
   */
  if (_symcZipEngine)
  {
    if (_tc)
      _tc->debug("Calling DecDeleteZipEngine()");
    DecDeleteZipEngine(_symcZipEngine);
  }

  /*
   * Check again for the Decomposer...
   */
  if (_symcDecomposer)
  {
    /*
     * And delete the Decomposer itself.
     * Again, we don't care if this fails.
     */
    if (_tc)
      _tc->debug("Calling DecDeleteDecomposer()");
    DecDeleteDecomposer(_symcDecomposer);
  }

  /*
   * Need to arrange for this fella to go away,
   * if it's here.
   */
  if (_symcEventSink)
    delete _symcEventSink;

  /*
   * [Retentive] Clear all these for safety.
   */
  _symcZipEngine  = NULL;
  _symcDecomposer = NULL;
  _symcEventSink  = NULL;
  _tc             = NULL;

  /*
   * JD.
   */
}

/* ----- */

/*
 * Allows the caller to retrieve the error
 * code of the last Extract() operation.
 */
ZipArchive::ErrorCode ZipArchive::Error()
{
  /*
   * Taxing!
   */
  return _errorCode;
}

/* ----- */

/*
 * This one does the work -- the caller passes the
 * pathname to the archive [VDB] to unpack, the
 * pathname of an already-created directory into
 * which to put the output, and a trace interface.
 *
 * Be aware that the final two parameters, 'options'
 * and 'files', are not used at the present time.
 * They are left over from the previous version of
 * this interface.
 */
bool ZipArchive::Extract(const std::string &archive,
                         const std::string &directory,
                         void              *traceClient,
                         Options            options, 
                         const std::string &files)
{
  char          *dir;
  char          *arc;
  HRESULT        hr;
  IDecObject    *symcDecObj;
  char           procFil[MAX_PATH];
  WORD           procRes;
  MyTraceClient *tc  = static_cast<MyTraceClient *>(traceClient);

  /*
   * Copy the trace client over.  It would have been nice to
   * do this in the constructor, but I can't change the
   * interface.
   */
  _tc = tc;

  /*
   * If there's no archive name, then give up now.
   */
  if (archive.size() == 0)
  {
    _errorCode = ArchiveNotFoundError;
    return false;
  }

  /*
   * Initialise the Decomposer, if it's not ready to go yet.
   */
  if (! this->_ensureOnline(tc))
  {
    if (_tc)
      _tc->msg("Failed to start Decomposer");
    _errorCode = ParameterError;
    return false;
  }

  /*
   * Get the C-style version of the archive.  I know const_cast
   * is evil, but there you go.  The interface doesn't expect
   * the string to be const, which is a shame, but what can you do?
   */
  arc = const_cast<char *>(archive.c_str());

  /*
   * Record the name of the root file in the event object.
   * This is used to identify callbacks occuring on the root
   * file, as opposed to callbacks on child files.
   */
  _symcEventSink->rootFile(archive);

  /*
   * Record the output directory into the event object as well.
   */
  _symcEventSink->outputDir(directory);

  /*
   * If there's no directory specified, extract to the current
   * directory (this is weird, but it preserves the functionality
   * of the old interface).  On the other hand, get the C-string
   * version of the directory.
   * Before that, however, if a directory was specified, check
   * that it is, in fact, a directory.
   *
   * [Directory == directory into which to put the files.]
   */
  if (directory.size() != 0)
  {
    if (! DirUtil::isDir(directory))
    {
      if (_tc)
        _tc->msg("Error: Directory into which to extract does not appear to exist");
      _errorCode = ArchiveNotFoundError;
      return false;
    }
    /*
     * Due to the way that the Decomposer interface is
     * declared, this grotesqueness is required.
     */
    dir = const_cast<char *>(directory.c_str());
  }
  else
    dir = ".";
  
  /*
   * Attempt to stamp the extraction directory into the engine.
   * If it fails, it's safe to return false at this point, after
   * setting the error code.
   */
  hr = _symcDecomposer->SetTempDir(dir);
  if (SUCCEEDED(hr))
  {
    /*
     * Create a file object for the Decomposer library.
     */
    hr = DecNewFileObject(arc, arc, &symcDecObj);
    if (SUCCEEDED(hr))
    {
      /*
       * See comments at head of module.
       */
      procFil[0] = 0;

      /*
       * And call the Decomposer.
       */
      hr = _symcDecomposer->Process(symcDecObj, _symcEventSink, &procRes, procFil);

      /*
       * Destroy the file object.
       */
      DecDeleteFileObject(symcDecObj);
    }
  }
  else
  {
    /*
     * Log the error.
     */
    if (_tc)
      _tc->msg("SetTempDir() on Decomposer failed");
  }

  /*
   * However we got here, the contents of 'hr' are
   * the last error that occurred.  I now have to
   * to translate the error code into the old set of
   * error codes used in the original InfoZIP-based
   * version of VDBunpacker.
   *
   * In some cases, the translation is a little forced.
   * This is a shame, but there it is.
   */
  switch(hr)
  {
    /*
     * No error occurred.
     */
    case DEC_OK :
      _errorCode = NoError;
      break;

    /*
     * These are Decomposer/Engine init errors.
     */
    case DECERR_ENGINE_NOT_SUPPORTED :
    case DECERR_ENGINE_FILE_MISSING :
    case DECERR_INVALID_PARAMS :
      _errorCode = ParameterError;      
      break;

    /*
     * These ones means that the start file doesn't exist.
     */
    case DECERR_OBJECT_FILE_INVALID :
    case DECERR_CONTAINER_ACCESS :
      _errorCode = ArchiveNotFoundError;
      break;

    /*
     * These either come from some problem in the
     * temporary directory, or some problem with
     * the disk.  I'm forced to call them all
     * 'disk full'.
     */
    case DECERR_TEMPDIR_INVALID :
    case DECERR_TEMPFILE_CREATE :
    case DECERR_TEMPFILE_OPEN :
    case DECERR_TEMPFILE_READ :
    case DECERR_TEMPFILE_WRITE :
    case DECERR_TEMPFILE_CLOSE :
    case DECERR_TEMPFILE_DELETE :
    case DECERR_DISK_NOT_READY :
    case DECERR_DISK_FULL :
      _errorCode = DiskFullError;
      break;

    /*
     * This one's obvious.
     */
    case DECERR_OUT_OF_MEMORY :
      _errorCode = MemoryError;
      break;

    /*
     * These are the weird errors.
     *
     * In our world, the user should never cancel,
     * or attempt to update anything, so fold those
     * in here.  There should be no child access
     * problems, so that one's not a worry either.
     * However, the engine exception case is also
     * in here.
     */
    case DECERR_USER_CANCEL :
    case DECERR_CONTAINER_UPDATE :
    case DECERR_INVALID_CHANGE :
    case DECERR_INVALID_REPLACE :
    case DECERR_INVALID_DELETE :
    case DECERR_ENGINE_EXCEPTION :
    case DECERR_CHILD_ACCESS :
      _errorCode = UnknownError;
      break;

    /*
     * These probably come from corrupt archives.
     *
     * Note that I have put DECERR_GENERIC and
     * default in here.  This is intentional --
     * the error callbacks implemented in symcsink.cpp
     * return DECERR_GENERIC.
     */
    case DECERR_CONTAINER_OPEN :
    case DECERR_CONTAINER_CLOSE :
    case DECERR_CHILD_EXTRACT :
    case DECERR_CHILD_DECRYPT :
    case DECERR_CHILD_SPLIT :
    case DECERR_CHILD_SPLIT_PART :
    case DECERR_CHILD_SIZE_UNKNOWN :
    case DECERR_GENERIC :
    default :
      _errorCode = ZipArchiveError;
      break;
  }

  /*
   * This is a teensy bit of a kludge [ahem].
   * We need to return whether or not the file was
   * actually a Zip archive or not.  Because <see
   * comment at head of this file on the subject
   * of return codes from IDecomposer::Process()>,
   * the event sink object has graciously agreed
   * to make a note if a callback indicating that
   * processing began occurs.  Here, we check that,
   * and if (a) we are currently error-free [which means
   * the file existed!]; and (b) no relevant callback
   * took place; then we change the error code.
   */
  if ((_errorCode == NoError) && (! _symcEventSink->callbackHappened()))
    _errorCode = ArchiveNotFoundError;

  /*
   * JD.
   */
  return ((_errorCode == NoError) ? true : false);
}

/* ----- */

/*
 * _ensureOnline() handles actually constructing and preparing
 * the Decomposer engines.
 */
bool ZipArchive::_ensureOnline(MyTraceClient *tc)
{
  HRESULT        hr;

  /*
   * If we don't already have an event sink object,
   * make one now.  If we already have one, then
   * just use it again.
   */
  if (! _symcEventSink)
    _symcEventSink = new CinwEventSink(tc);

  if (! _symcDecomposer)
  {
    /*
     * Get the main decomposer online.
     */
    hr = DecNewDecomposer(&_symcDecomposer);
    if ((FAILED(hr)) || (NULL == _symcDecomposer))
      return false;

    /*
     * Get the Zip understander online.
     */
    hr = DecNewZipEngine(&_symcZipEngine);
    if ((FAILED(hr)) || (NULL == _symcZipEngine))
      return false;

    /*
     * Hook the Zip engine into the main Decomposer engine.
     */
    hr = _symcDecomposer->AddEngine(_symcZipEngine);
    if (FAILED(hr))
      return false;
  }

  /*
   * JD.
   */
  return true;
}

/* ----- Transmission Ends ----- */
