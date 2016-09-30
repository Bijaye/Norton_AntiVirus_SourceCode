#pragma once

#include "ccLogViewerCategoryHelper.h"
#include "ccSymMultiEventFactoryHelper.h"

class CLogViewerSymNetiCategory : public ccEvtMgr::CLogViewerCategoryHelper
{
public:
    CLogViewerSymNetiCategory(long nEventType,
                              long nCategoryId,
							  long nHelpId);
    virtual ~CLogViewerSymNetiCategory();

private:
    CLogViewerSymNetiCategory();
    CLogViewerSymNetiCategory(const CLogViewerSymNetiCategory&);
    CLogViewerSymNetiCategory& operator =(const CLogViewerSymNetiCategory&);

public:
    virtual BOOL Create(); 
    virtual void Destroy();

public:
	virtual HRESULT GetImageList(HIMAGELIST& hImageList);
	virtual HRESULT GetColumnCount(int& iColumnCount);
    virtual HRESULT GetCategoryID(int& iCategoryID);
	virtual HRESULT LaunchHelp();
    virtual HRESULT GetEventCount(int& iEventCount);
    virtual HRESULT SetLogFileSize(int iLogFileSize);
	virtual HRESULT SetLoggingState(BOOL bEnabled);
    virtual HRESULT ClearCategory();

protected:
    virtual HRESULT GetCategoryName(HMODULE& hModule, 
                                    UINT& nId);
    virtual HRESULT GetCategoryDescription(HMODULE& hModule, 
                                           UINT& nId);
    virtual HRESULT GetColumnName(int iColumn, 
                                  HMODULE& hModule, 
                                  UINT& nId);
    virtual HRESULT CopyEvent(const ccEvtMgr::CEventEx& Event, 
                              const ccEvtMgr::CLogFactoryEx::EventInfo& Info,
							  cc::ILogViewerEvent*& pLogViewerEvent);

	// FinalCreate()?
	virtual BOOL VerifyShouldCreate();
	
	// this probably doesn't belong here
	// it should probably be in a derived class
	// however, close to ship, wish to reduce
	// code velocity.
	virtual BOOL IsIWPFirewallSettingEnabled();

public :
    BOOL WrapEvent(ccEvtMgr::CEventEx* pEvent, 
		cc::ILogViewerEvent*& pLogViewerEvent);

protected:
    HIMAGELIST m_hImageList;
    UINT m_uImageListId;
    HMODULE m_hSymNeti;
    long m_nCategoryId;
	long m_nHelpId;
	CSymStaticRefCount<CSymMultiEventFactoryHelper> m_EventFactoryCreator;	

protected:
    static BOOL m_bInit;
    static BOOL m_bUserColumn;
    static BOOL m_bLicenseValid;
    static DWORD m_dwUserLevel;
	static DWORD m_dwProductType;

protected:
    static const LPCTSTR m_szSymNetiDLL;
    static const LPCSTR m_szSymNetiCreateFactory;
    static const LPCSTR m_szSymNetiDeleteFactory;
};
