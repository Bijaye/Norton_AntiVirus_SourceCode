////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ccEraserInterface.h"

class CAVScanObject;

class CEraserScanSink :
	public ccScanw::IScanwSink,
    public ccScanw::IScanwProperties,
    public ccScanw::IScanwItems,
    public ccEraser::ICallback,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(ccScanw::IID_ScanwSink, ccScanw::IScanwSink)
        SYM_INTERFACE_ENTRY(ccScanw::IID_ScanwProperties, ccScanw::IScanwProperties )
		SYM_INTERFACE_ENTRY(ccScanw::IID_ScanwItems, ccScanw::IScanwItems )
    SYM_INTERFACE_MAP_END()

protected:
	CEraserScanSink();
	virtual ~CEraserScanSink();

public:
	// This is how to create a CEraserScanSink object:
	static SYMRESULT Create( CEraserScanSink*& pSink, CAVScanObject* pScanObj = NULL );

	void AddItem(const WCHAR* szItem)
	{
		ccLib::CExceptionInfo exInfo;
		try
		{
			m_vItems.push_back(szItem);
		}
		CCCATCHMEM(exInfo);
	}

    // IScanwSink overrides.
    virtual ccScanw::SCANWSTATUS OnBusyW();

    virtual ccScanw::SCANWSTATUS OnError(ccScanw::IScanwError* pError);

    virtual ccScanw::SCANWSTATUS OnNewItem(const WCHAR* pszItem);

    virtual ccScanw::SCANWSTATUS OnNewFile(const WCHAR* pszLongName, ccScanw::ScanwFileAttribute lAttribute);
	virtual ccScanw::SCANWSTATUS OnNewDirectory(const WCHAR* pszLongName, ccScanw::ScanwFileAttribute lAttribute);

    virtual ccScanw::SCANWSTATUS OnInfectionFound(ccScanw::IScanwInfection* pInfection);

    virtual ccScanw::SCANWSTATUS OnRepairFailed(ccScanw::IScanwInfection* pInfection);

    virtual ccScanw::SCANWSTATUS OnRemoveMimeComponent(ccScanw::IScanwInfection* pInfection, WCHAR* pszMessage,
        int iMessageBufferSize);

    virtual ccScanw::SCANWSTATUS LockVolume(const WCHAR* szVolume, bool bLock);

    virtual ccScanw::SCANWSTATUS OnCleanFile( const wchar_t * pszFileName,
        const wchar_t * pszTempFileName, ccScanw::ScanwFileAttribute lAttribute, 
        ccScanw::IScanwCompressedFile *pCompressedFile ) ;


    // IScanwSink overrides
    virtual ccScanw::SCANWSTATUS OnBeginThreadW();

    virtual ccScanw::SCANWSTATUS OnEndThreadW();

	virtual ccScanw::SCANWSTATUS OnInfectionHandlingComplete(ccScanw::IScanwInfection* pInfection);

	virtual ccScanw::SCANWSTATUS OnDeleteFailed( ccScanw::IScanwInfection* pInfection ) ;

	virtual ccScanw::SCANWSTATUS OnPostProcessContainer( const wchar_t * pszFileName ) ;

	virtual ccScanw::SCANWSTATUS OnNewCompressedChild( ::decABI::IDecomposerContainerObject * pIDecomposerContainerObject, 
														const wchar_t * pszChildName ) ;

    // IScanwProperties overrides
    virtual ccScanw::SCANWSTATUS GetProperty( const WCHAR* szProperty, int iDefault, int &riProperty ) const;

  	virtual ccScanw::SCANWSTATUS GetProperty( const WCHAR* szProperty, const WCHAR * szDefault, const wchar_t *&prwszProperty ) const;
	
    // IScanwItems overrides
    ccScanw::SCANWSTATUS GetScanItemCount( int &riScanItemCount ) const ;
	ccScanw::SCANWSTATUS GetScanItemPath( int iIndex, const wchar_t *&prwszScanItemPath ) const;

	// ICallback implementation.
    virtual ccEraser::eResult PreDetection(ccEraser::IDetectionAction* pDetectAction, const ccEraser::IContext* pContext) throw();
    
    virtual ccEraser::eResult PostDetection(ccEraser::IDetectionAction* pDetectAction, ccEraser::eResult eDetectionResult, const ccEraser::IContext* pContext) throw();
    
    virtual ccEraser::eResult OnAnomalyDetected(ccEraser::IAnomaly* pAnomaly, const ccEraser::IContext* pContext) throw();

    // Abort Scan mechanism
	virtual bool IsAborted();

	// CEraserScanSink:
	SYMRESULT			Register( CAVScanObject* pScanObj );
	void				UnRegister();


protected:
    std::vector<std::wstring>	m_vItems;
	CAVScanObject*				m_pScanObj;
};


// Dtor calls UnRegister(), as well as Release():
class CEraserScanSinkPtr
{
public:
	CEraserScanSinkPtr() {};
	~CEraserScanSinkPtr()
	{
		if ( m_ptr.m_p != NULL )
		{
			m_ptr->UnRegister();
		}
	}

    CEraserScanSinkPtr(const CEraserScanSink* p) throw()
	{
		*this = p;
	}
	CEraserScanSink* operator =(const CEraserScanSink* p) throw()
	{
		return m_ptr = p;
	}
	operator CEraserScanSink*() const throw()
	{
		return (CEraserScanSink*) m_ptr;
	}
    bool operator ==(CEraserScanSink* p) const throw()
    {
        return m_ptr == p;
    }
    operator CEraserScanSink*&() throw()
    {
        return m_ptr;
    }

public:
	CSymPtr<CEraserScanSink>	m_ptr;
};
