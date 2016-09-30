// ThreadControler.h: interface for the ThreadController class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADCONTROLER_H__1CFA1B01_CA59_11D2_AD5F_00A0C9C71BBC__INCLUDED_)
#define AFX_THREADCONTROLER_H__1CFA1B01_CA59_11D2_AD5F_00A0C9C71BBC__INCLUDED_

#if _MSC_VER > 1000
# pragma once
#endif /* _MSC_VER > 1000 */

/* ----- */

#include <CMclThread.h>

#include "QuarantineMonitor.h"
#include "ScannerSubmittor.h"
#include "StatusMonitor.h"
#include "RegistryMonitor.h"
#include "BlessedMonitor.h"
#include "JobPackager.h"

/* ----- */

class ThreadController : public CMclThreadHandler
{
  public :
              ThreadController();
    virtual   ~ThreadController();
    unsigned  ThreadHandlerProc();

    /*
     * Do NOT rearrange these.  The original author, in his infinite
     * wisdom, made the order in which the constructors for these
     * objects are called very important.  As we all know (at
     * least, as we all know after checking with Stroustrup),
     * member objects are initialised in the order of their
     * declaration.
     *
     * [inw 2000-06-05]
     */
    bool              started;
    RegistryMonitor   registryMonitor;
    BlessedMonitor    blessedMonitor;
    QuarantineMonitor quarantineMonitor;
    StatusMonitor     statusMonitor;
    JobPackager       jobPackager;
    ScannerSubmittor  scannerSubmittor;
};

#endif /* !defined(AFX_THREADCONTROLER_H__1CFA1B01_CA59_11D2_AD5F_00A0C9C71BBC__INCLUDED_) */
