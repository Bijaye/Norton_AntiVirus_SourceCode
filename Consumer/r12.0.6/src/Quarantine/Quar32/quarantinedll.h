//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// QuarantineDLL.h - Contains interface definitions for Quarantine DLL

#ifndef _QUARANTINEDLL_H_
#define _QUARANTINEDLL_H_

#include "IQuaran.h"

// Taken from QuarAdd_cc.h
#include "..\..\SDKs\AutoProtect\include\apquar.h"
#include "avtypes.h"

#include "qspak.h"
#include <time.h>
#include "AutoProtectWrapper.h"
#include "ccsynclock.h"
#include "QScanner.h"

class CQuarantineDLL : public IQuarantineDLL2,
                       public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(IID_QuarantineDLL, IQuarantineDLL)
        SYM_INTERFACE_ENTRY(IID_QuarantineDLL2, IQuarantineDLL2)
	SYM_INTERFACE_MAP_END()

    // IQuarantineDLL
    STDMETHOD_( UINT, GetVersion() );

	STDMETHOD(Initialize());

    STDMETHOD(SetScanner(IScanner* pScanner));

    STDMETHOD(GetQuarantineFolder(LPSTR lpszFolder, UINT uBufferSize));

    //STDMETHOD(GetServerName(LPSTR lpszName, UINT uBufferSize));

	STDMETHOD(CreateNewQuarantineItemFromFile(LPSTR lpszSourceFileName,
                                    DWORD dwFileType,
                                    DWORD dwItemStatus,
                                    IQuarantineItem **pNewItem));

	STDMETHOD (LoadQuarantineItemFromFilenameVID(LPCSTR lpszSourceFileName, 
												DWORD dwVirusID, 
												IQuarantineItem **pMatchingQuarItem));

    STDMETHOD(LoadQuarantineItemFromUUID(UUID uniqueID,
                                         IQuarantineItem **pMatchingQuarItem));

	STDMETHOD(CreateNewQuarantineItemWithoutFile(DWORD dwItemStatus, IQuarantineItem **pNewItem));

    STDMETHOD(ProcessIncomingFile(LPSTR lpszSourceFileName));
    STDMETHOD(ProcessAllIncomingFiles());

    STDMETHOD(Enum(IEnumQuarantineItems** pEnumObj));

    STDMETHOD(GetVirusInformation(unsigned long ulVirusID, IScanVirusInfo** ppInfo));

	STDMETHOD(TakeSystemSnapshot());
	STDMETHOD(AddSnapshotToQuarantineItem(IQuarantineItem *pQuarItem));

    // Construction;
    CQuarantineDLL();
    ~CQuarantineDLL();

protected:
	HRESULT ScanNewQuarantineMainFile(LPSTR lpszDestQuarItemPath,IQuarantineItem **pNewItem);
    BOOL FetchQuarantineOptionsPath(LPSTR  lpzsPathBuf, DWORD  dwBufSize);
    BOOL InitializeQuarOptsObject(IQuarantineOpts**  pQuarOpts);
	HRESULT GenerateUniqueDummyFileInQuarantineFolder(LPTSTR szDummyFilePath);

	HRESULT SetOriginalScanDefsDateToCurrent(HQSERVERITEM hQuarItem);

private:
	// Helper Function to Convert Date/Time Formats

	void QSDateToSysTime(QSPAKDATE stQSDate, SYSTEMTIME* stSysTime);
	QSPAKDATE SysTimeToQSDate(SYSTEMTIME stSysTime);

    // This HINSTANCE is used to keep the APCOMM dll loaded.  That DLL is used to
    // dissable AP while this process is running.  If we unload the DLL while AP is
    // dissabled then it asserts.
    //HINSTANCE m_hAPDll;
    //BOOL m_bDissabledAP;
	
	bool m_bInitialized;

    // Quarantine folder name.
    char m_szQuarantineFolder[MAX_PATH];

    // AutoProtect wrapper
    CAutoProtectWrapper m_APWrapper;

    // Synchronization object to make calls to this object thread safe
    ccLib::CCriticalSection m_critSec;

    // QScanner class to handle file scans
	CQScanner* m_pQScanner;

	bool m_bSnapshot;

    HRESULT DoScan(IQuarantineItem* pItem);
};

////////////////////////////////////////////////////////////////////////////

#endif
