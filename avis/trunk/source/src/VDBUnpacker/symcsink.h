/*
 * symcsink.h
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
 * Interface definition for the CinwEventSink object,
 * an overridden IDecEventSink, and is used by the
 * Symantec Decomposer API.  See symcsink.cpp for
 * the implementation.
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

/* ----- Include guard ----- */

#ifndef __SYMCSINK_DOT_H__
#define __SYMCSINK_DOT_H__

/* ----- STL headers ----- */

#include <string>

/* ----- Symantec headers ---- */

/*
 * It is necesary to arrange for _WINDOWS to be defined
 * before including the Symantec headers, or they don't
 * work -- critical entry point functions are not
 * defined.
 */
#ifndef _WINDOWS
# define _WINDOWS
# define _INW_DEFINED_WINDOWS
#endif /* _WINDOWS */

#include "dec2.h"
#include "dec2zip.h"

/*
 * Repair the compiler directive gyrations.
 */
#ifdef _INW_DEFINED_WINDOWS
# undef _WINDOWS
# undef _INW_DEFINED_WINDOWS
#endif /* _INW_DEFINED_WINDOWS */

/* ---- Our headers ----- */

#include "MyTraceClient.h"

/* ----- START HERE ----- */

class CinwEventSink : public CDecEventSink
{
public:
                      CinwEventSink   (MyTraceClient       *tc       = NULL);
                      ~CinwEventSink  ();
          void        rootFile        (std::string          rootFile);
          std::string rootFile        ();
          void        outputDir       (std::string          outDir);
          std::string outputDir       ();
          boolean     callbackHappened();

  virtual HRESULT     OnNew           (IDecObject          *pObject,
                                       WORD                *pwResult,
                                       LPTSTR               pszNewDataFile,
                                       BOOL                *pbContinue);
  virtual HRESULT     OnIdentified    (IDecContainerObject *pObject,
                                       BOOL                *pbContinue);
  virtual HRESULT     OnChildFound    (IDecContainerObject *pObject,
                                       LPCTSTR              pszChildName,
                                       BOOL                *pbContinue);
  virtual HRESULT     OnBadChild      (IDecContainerObject *pObject,
                                       HRESULT              hr,
                                       LPCTSTR              pszChildName,
                                       BOOL                *pbContinue);
  virtual HRESULT     OnNotIdentified (IDecObject          *pObject,
                                       WORD                *pwResult,
                                       LPTSTR               pszNewDataFile);
  virtual HRESULT     OnBusy          ();
  virtual HRESULT     OnEngineError   (IDecContainerObject *pObject,
                                       HRESULT              hr,
                                       WORD                *pwResult,
                                       LPTSTR               pszNewDataFile);
private:
  MyTraceClient *_tc;
  std::string    _rootFile;
  std::string    _outDir;
  boolean        _callbackHappened;
};

/* ---- Include guard ----- */

#endif /* __SYMCSINK_DOT_H__ */

/* ---- Transmission Ends ----- */
