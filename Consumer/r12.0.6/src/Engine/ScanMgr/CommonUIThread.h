#pragma once

#include "ccthread.h"
#include "AVCommonUILoader.h"
#include "AutoProtectWrapper.h"

class CCommonUIThread : public ccLib::CThread
{
public:
    CCommonUIThread(void);
    virtual ~CCommonUIThread(void);

    // CThread
    int Run (void);

    // ICommonUI methods
    HRESULT ShowCUIDlg( CommonUIType UIType, 
                        ccEraser::IAnomalyList* pAnomalyList, 
                        ICommonUISink* pSink, 
                        const long lDamageControlItemsRemaining );
    HRESULT SetParentWindow( HWND hWndParent );
    HRESULT SetScanComplete(ccEraser::IAnomalyList* pAnomalyList, const long lDamageControlItemsRemaining );
    HRESULT SetEmailInfo(LPCTSTR szSubject, LPCTSTR szSender, LPCTSTR szRecipient);
    HRESULT SetMemoryScanStatus(MemoryScanStatus MemScanStatus);
    HRESULT SetScanCompressedFiles(bool bScanCompressedFiles);
    HRESULT SetScanTime(unsigned long ulScanTime);
    HRESULT SetRepairProgress(bool bRepairProgress);
    HRESULT SetScanNonViralThreats(bool bScanNonViralThreats);
    HRESULT SetQuickScan(bool bQuickScan);
    HRESULT UpdateRepairProgress(unsigned long nItemsRemaining, bool bFailuresOccured, bool& bAbort);
    HRESULT ShowMemTerminationDlg(bool &bTerminate, bool bEnableTimeout);

protected:
    void reset();

    // CommonUI Loader
    AV::AVCommonUI_ICommonUIFactory m_CommonUILoader;

    // The real Common UI object
    CSymPtr<ICommonUI> m_spCommonUI;

    unsigned long m_ulScanTime;
    bool m_bScanCompressedFiles;
    CommonUIType m_UIType;
    ccEraser::IAnomalyListPtr m_spAnomalyList;
    CSymPtr<ICommonUISink> m_spSink;
    long m_lDamageControlItems;
    bool m_bEmailScan;
    CAtlString m_strSubject, m_strSender, m_strRecipient;
    MemoryScanStatus m_MemScanStatus;
    bool m_bRepairProgress;
    bool m_bScanNonViralThreats;
    bool m_bQuickScan;
    HWND m_hWndParent;
};
