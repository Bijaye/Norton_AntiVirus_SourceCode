#pragma once

#include "ccEraserInterface.h"

class CScanManager;

class CEraserScanSink :
    public IScanSink2,
    public IScanProperties,
    public IScanItems,
    public ccEraser::ICallback,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(IID_ScanSink2, IScanSink2)
        SYM_INTERFACE_ENTRY(IID_ScanProperties, IScanProperties )
		SYM_INTERFACE_ENTRY(IID_ScanItems, IScanItems )
    SYM_INTERFACE_MAP_END()

	CEraserScanSink() {m_bAbort = false;}
	virtual ~CEraserScanSink() {}
    
	void AddItem(const char* szItem)
	{
		m_vItems.push_back(szItem);
	}

    // IScanSink overrides.
    virtual SCANSTATUS OnBusy();

    virtual SCANSTATUS OnError(IScanError* pError);

    virtual SCANSTATUS OnNewItem(const char* pszItem);

    virtual SCANSTATUS OnNewFile(const char* pszLongName, const char* pszShortName);
    virtual SCANSTATUS OnNewDirectory(const char* pszLongName, const char* pszShortName);

    virtual SCANSTATUS OnInfectionFound(IScanInfection* pInfection);

    virtual SCANSTATUS OnRepairFailed(IScanInfection* pInfection);

    virtual SCANSTATUS OnRemoveMimeComponent(IScanInfection* pInfection, char* pszMessage,
        int iMessageBufferSize);

    virtual SCANSTATUS LockVolume(const char* szVolume, bool bLock);

    virtual SCANSTATUS OnCleanFile(const char* pszFileName, const char* pszTempFileName);

    // IScanSink2 overrides
    virtual SCANSTATUS OnBeginThread();

    virtual SCANSTATUS OnEndThread();

    // IScanProperties overrides
    virtual int GetProperty( const char* szProperty, int iDefault );

  	virtual const char * GetProperty( const char* szProperty, const char * szDefault );
	
    // IScanItems overrides
	virtual int GetScanItemCount();
	
	virtual const char * GetScanItemPath( int iIndex );

	// ICallback implementation.
    virtual ccEraser::eResult PreDetection(ccEraser::IDetectionAction* pDetectAction, const ccEraser::IContext* pContext) throw();
    
    virtual ccEraser::eResult PostDetection(ccEraser::IDetectionAction* pDetectAction, ccEraser::eResult eDetectionResult, const ccEraser::IContext* pContext) throw();
    
    virtual ccEraser::eResult OnAnomalyDetected(ccEraser::IAnomaly* pAnomaly, const ccEraser::IContext* pContext) throw();

    // Abort Scan mechanism
    virtual void SetAbortFlag();

	virtual bool IsAborted();

protected:
    std::vector<std::string> m_vItems;
	bool m_bAbort;
};
